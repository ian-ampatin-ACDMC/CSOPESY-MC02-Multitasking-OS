// ----- << External Library >> ----- //
#include "MMU.hpp"
#include <iostream>
#include <stdexcept>

// ----- << Private Member Function Implementation >> ----- //
std::vector<std::string> MMU::dataFormat(std::string raw)
{
	for (char character : raw)
	{
		if (!std::isdigit(character) && !(character >= 'A' && character <= 'F'))
		{
			throw std::runtime_error("\nERROR: Invalid characters found in raw string\n");
		}
	}

	if (raw.size() % 2 != 0)
	{
		raw.insert(raw.begin(), '0');
	}

	std::vector<std::string> formatted;

	for (size_t i = 0; i < raw.size(); i += 2)
	{
		formatted.push_back(raw.substr(i, 2));
	}

	return formatted;
}

MMU::Page* MMU::getPage(unsigned long long ID)
{
	for (auto pair = masterTable.begin(); pair != masterTable.end(); pair++)
	{
		for (Page& p : pair->second.pages)
		{
			if (p.frameID == ID)
			{
				return &p;
			}
		}
	}

	return nullptr;
}

size_t MMU::getPageIndex(unsigned long long frameID)
{
	for (auto pair = masterTable.begin(); pair != masterTable.end(); pair++)
	{
		PageTable table = pair->second;
		for (size_t i = 0; i < table.pages.size(); i++)
		{
			if (table.pages[i].frameID == frameID)
			{
				return i;
			}
		}
	}

	return 0;
}

bool MMU::isValid(unsigned long long processID, unsigned long long frameID)
{
	auto pair = masterTable.find(processID);

	if (pair == masterTable.end())
	{
		std::cerr << "\nError: Process page table not contained\n";
		return false;
	}

	PageTable& table = masterTable.at(processID);

	for (Page& page : table.pages)
	{
		if (page.frameID == frameID)
		{
			if (page.valid)
			{
				return true;
			}
		}
	}

	return false;
}

bool MMU::validAccess(unsigned long long processID, size_t physicalMemoryLocation)
{
	PageTable table = masterTable.at(processID);
	std::optional<unsigned long long> frameID = physicalMemory.getFrameID(physicalMemoryLocation);

	if (frameID)
	{
		for (Page& page : table.pages)
		{
			if (page.frameID == frameID)
			{
				return true;
			}
		}

		return false;
	}
	else
	{
		return false;
	}
}

// ----- << Public Member Function Implementation >> -----//
MMU::MMU(size_t maximumMemoryPerProcess, size_t maximumOverallMemory, size_t memoryPerFrame, size_t minimumMemoryPerProcess, bool& breaker)
	: physicalMemory(static_cast<size_t>(1) << memoryPerFrame, static_cast<size_t>(1) << maximumOverallMemory),
	availableMemory(static_cast<size_t>(1) << maximumOverallMemory),				framesCreated(0),
	maximumMemoryPerProcess(static_cast<size_t>(1) << maximumMemoryPerProcess),		maximumOverallMemory(static_cast<size_t>(1) << maximumOverallMemory),
	memoryPerFrame(static_cast<size_t>(1) << memoryPerFrame),						minimumMemoryPerProcess(static_cast<size_t>(1) << minimumMemoryPerProcess),
	dataPlaceholder(""),															breaker(breaker),
	pagesIn(0),																		pagesOut(0)
{
	numFramesPerProcess = maximumMemoryPerProcess / memoryPerFrame;
	limitHigh = maximumMemoryPerProcess;

	size_t bytes = static_cast<size_t>(1) << memoryPerFrame;

	for (size_t i = 0; i < bytes; i++)
	{
		dataPlaceholder += "00";
	}
}

MMU::~MMU() = default;

size_t MMU::countValid(unsigned long long processID)
{
	auto pair = masterTable.find(processID);

	if (pair == masterTable.end())
	{
		std::cerr << "Error: Process page table not contained\n";
		throw std::runtime_error("Error: Process page table not contained\n");
	}

	PageTable& table = masterTable.at(processID);

	size_t count = 0;
	for (const auto& page : table.pages)
	{
		if (page.valid)
		{
			count++;
		}
	}

	return count;
}

void MMU::createPages(unsigned long long processID)
{
	std::unique_lock<std::mutex> createPageLock(createPageMutex);

	PageTable table;
	table.pages.resize(numFramesPerProcess);
	table.limit = limitHigh;

	size_t limit = framesCreated + numFramesPerProcess;
	size_t pagesIndex = 0;
	for (; framesCreated < limit; ++framesCreated)
	{
		if (breaker) break;

		physicalMemory.writeBackingStore(framesCreated, dataPlaceholder);
		table.pages[pagesIndex++] = { false, false, framesCreated };
	}

	masterTable.emplace(processID, std::move(table));
}

void MMU::createPages(unsigned long long processID, size_t requiredMemory)
{
	std::unique_lock<std::mutex> createPageLock(createPageMutex);
	size_t pagesRequired;

	if (requiredMemory <= memoryPerFrame)
	{
		pagesRequired = 1;
	}
	else
	{
		pagesRequired = requiredMemory / memoryPerFrame;
	}

	PageTable table;
	table.pages.resize(pagesRequired);
	table.limit = requiredMemory;
	
	size_t limit = framesCreated + pagesRequired;
	size_t pagesIndex = 0;

	for (; framesCreated < limit; ++framesCreated)
	{
		physicalMemory.writeBackingStore(framesCreated, dataPlaceholder);
		table.pages[pagesIndex++] = { false, false, framesCreated };
	}

	masterTable.emplace(processID, std::move(table));
}

size_t MMU::getAvailableMemory() const
{
	return availableMemory;
}

size_t MMU::getPagesIn() const
{
	return pagesIn;
}

size_t MMU::getPagesOut() const
{
	return pagesOut;
}

void MMU::handlePageFault(unsigned long long processID, unsigned long long requestedPage)
{
	std::unique_lock<std::mutex> pageFaultLock(pageFaultMutex);

	auto mTable = masterTable.find(processID);
	if (mTable == masterTable.end())
	{
		throw std::runtime_error("ERROR: Process not in master table\n");
	}

	PageTable& table = mTable->second;
	if (requestedPage >= table.pages.size())
	{
		throw std::out_of_range("ERROR: Requested page index is out of range\n");
	}

	Page& page = table.pages[requestedPage];

	std::optional<size_t> freeKey = physicalMemory.findFreeFrame();

	if (!freeKey)
	{
		size_t victimKey = physicalMemory.getVictimKey();

		std::optional<unsigned long long> victimFrameID = physicalMemory.getFrameID(victimKey);

		if (victimFrameID)
		{
			Page* victimPage = getPage(*victimFrameID);

			if (victimPage)
			{
				if (victimPage->dirty)
				{
					physicalMemory.overwriteBackingStore(victimKey);
					victimPage->dirty = false;
				}

				victimPage->valid = false;
			}
		}
		else
		{
			std::cerr << "\nError: Victim Frame not in memory\n";
			return;
		}

		physicalMemory.updateFrame(page.frameID, victimKey);
		page.valid = true;

		pagesIn++;
		pagesOut++;
	}
	else
	{
		if (!physicalMemory.find(page.frameID))
		{
			physicalMemory.updateFrame(page.frameID, *freeKey);
			page.valid = true;

			availableMemory -= memoryPerFrame;

			pagesIn++;
		}
	}
}

bool MMU::loadProcess(unsigned long long processID)
{
	std::unique_lock<std::recursive_mutex> readWriteLock(readWriteMutex);
	auto check = masterTable.find(processID);

	if (check == masterTable.end())
	{
		throw std::runtime_error("Error: Process page table not contained in master table\n");
	}

	PageTable& table = masterTable.at(processID);

	for (auto& page : table.pages)
	{
		handlePageFault(processID, getPageIndex(page.frameID));
	}

	for (auto& page : table.pages)
	{
		if (!page.valid)
		{
			return false;
		}
	}

	return true;
}

void MMU::printFrames()
{
	physicalMemory.printFrames();
}

void MMU::printMasterTable()
{
	std::cout << "---------- Master Table ----------\n";

	for (const auto& [processID, table] : masterTable)
	{
		std::cout << "Process ID: " << processID << "\n";

		if (table.pages.empty())
		{
			std::cout << "     (No pages)\n";
			continue;
		}

		for (size_t i = 0; i < table.pages.size(); ++i)
		{
			const Page& page = table.pages[i];
			std::cout << "     Page[" << i << "]"
				<< " FrameID=" << page.frameID
				<< " Valid=" << (page.valid ? "true" : "false")
				<< " Dirty=" << (page.dirty ? "true" : "false")
				<< "\n";
		}

	}
}

std::optional<std::string> MMU::protectedRead(unsigned long long processID, std::string address, size_t bytesToRead)
{
	std::unique_lock<std::recursive_mutex> rReadLock(readWriteMutex);

	auto pair = masterTable.find(processID);

	if (pair == masterTable.end())
	{
		std::cerr << "\nError: Process page table not contained\n";
		return std::nullopt;
	}

	PageTable& table = masterTable.at(processID);
	size_t base = physicalMemory.convertHexToDecimal(address);

	if (base >= table.limit)
	{
		//std::cerr << "\nError: Invalid address\n";
		return std::nullopt;
	}

	size_t location = base / memoryPerFrame;
	size_t newAddress = 0;
	size_t offset = 0;

	unsigned long long requestedFrame = table.pages[location].frameID;
	std::optional<size_t> readLocation = physicalMemory.find(requestedFrame);

	if (readLocation)
	{
		offset = base % memoryPerFrame;
		newAddress = *(readLocation)*memoryPerFrame + offset;

		return read(processID, physicalMemory.convertDecToHex(newAddress), bytesToRead);
	}
	else
	{
		//std::cout << "\n\nREAD Page Fault Occured\n\n";
		Page* page = getPage(requestedFrame);

		if (page)
		{
			handlePageFault(processID, getPageIndex(page->frameID));
		}
		else
		{
			std::cerr << "\nError: Page not in master table\n";
		}
		return protectedRead(processID, address, bytesToRead);
	}
}

bool MMU::protectedWrite(unsigned long long processID, std::string address, std::string data)
{
	std::unique_lock<std::recursive_mutex> rWriteLock(readWriteMutex);

	auto pair = masterTable.find(processID);

	if (pair == masterTable.end())
	{
		std::cerr << "\nError: Process page table not contained\n";
		return false;
	}

	PageTable& table = masterTable.at(processID);
	size_t base = physicalMemory.convertHexToDecimal(address);

	if (base >= table.limit)
	{
		//std::cerr << "\nError: Invalid address\n";
		return false;
	}

	size_t location = base / memoryPerFrame;
	
	unsigned long long requestedFrame = table.pages[location].frameID;
	std::optional<size_t> writeLocation = physicalMemory.find(requestedFrame);

	// Address translation
	size_t offset = 0;
	size_t newAddress = 0;

	if (writeLocation)
	{
		offset = base % memoryPerFrame;
		newAddress = *(writeLocation)*memoryPerFrame + offset;

		return write(processID, physicalMemory.convertDecToHex(newAddress), data);
	}
	else
	{
		//std::cout << "\n\nWRITE Page Fault Occured" << processID << "\n\n";
		Page* page = getPage(requestedFrame);

		if (page)
		{
			handlePageFault(processID, getPageIndex(page->frameID));
		}
		else
		{
			std::cerr << "\nError: Page not in master table\n";
		}

		return protectedWrite(processID, address, data);
	}
}

std::optional<std::string> MMU::read(unsigned long long processID, std::string address, size_t bytesToRead)
{
	std::unique_lock<std::mutex> readLock(readMutex);

	size_t base = physicalMemory.convertHexToDecimal(address);
	size_t location = base / memoryPerFrame;

	if (!validAccess(processID, location))
	{
		//std::cerr << "\nError: Invalid access\n";
		return std::nullopt;
	}

	if ((base + bytesToRead - 1) > maximumOverallMemory)
	{
		//std::cerr << "\nError: Invalid Address]n";
		return std::nullopt;
	}

	std::string data = "";

	for (size_t i = 0; i < bytesToRead; i++)
	{
		auto byte = physicalMemory.read(physicalMemory.convertDecToHex(base++));

		if (byte)
		{
			data += *byte;
			continue;
		}

		//std::cerr << "\nError: Physical Memory Read unsuccessful\n";
		return std::nullopt;
	}

	return data;
}

void MMU::remove(unsigned long long processID)
{
	auto pair = masterTable.find(processID);

	if (pair == masterTable.end())
	{
		std::cerr << "\nError: Process page table not contained\n";
		return;
	}

	PageTable table = masterTable[processID];
	std::vector<unsigned long long> rmVector;

	for (auto& page : table.pages)
	{
		page.valid = false;
		rmVector.push_back(page.frameID);
	}

	size_t removed = physicalMemory.remove(rmVector);

	availableMemory += removed * memoryPerFrame;
	pagesOut += removed;
}

bool MMU::write(unsigned long long processID, std::string address, std::string data)
{
	std::unique_lock<std::mutex> writeLock(writeMutex);

	size_t base = physicalMemory.convertHexToDecimal(address);
	size_t location = base / memoryPerFrame;

	if (!validAccess(processID, location))
	{
		//std::cout << "\nError: Invalid access\n";
		return false;
	}

	std::vector<std::string> formattedData = dataFormat(data);

	if ((base + formattedData.size() - 1) > maximumOverallMemory)
	{
		//std::cout << "\nError: Invalid Address\n";
		return false;
	}

	for (std::string formatted : formattedData)
	{
		if (physicalMemory.write(physicalMemory.convertDecToHex(base++), formatted))
		{
			//physicalMemory.updateFrame()
			continue;
		}

		//std::cerr << "\nError: Physical Memory Write unsuccessful\n";
		return false;
	}

	auto frameID = physicalMemory.getFrameID(location);
	if (frameID)
	{
		Page* page = getPage(*frameID);
		if (page)
		{
			page->dirty = true;
		}
	}

	return true;
}