// ----- << External Library >> ----- //
#include "PhysicalMemory.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

// ----- << Private Member Function Implementation >> ----- //
std::optional< std::string> PhysicalMemory::compileFrameData(unsigned long long ID)
{
	for (size_t i = 0; i < numFrames; i++)
	{
		if (frames[i] && frames[i]->getID() == ID)
		{
			std::ostringstream compiled;
			for (size_t k = 0; k < frameSize; k++)
			{
				compiled << frames[i]->read(convertDecToHex(k));
			}

			return compiled.str();
		}
	}

	return std::nullopt;
}


std::vector<std::string> PhysicalMemory::parseData(std::string raw) const
{
	if (raw.size() % 2 != 0)
	{
		throw std::runtime_error("\nERROR: Backing store data corrupted\n");
	}

	if (raw.size() / 2 > frameSize)
	{
		throw std::runtime_error("\nERROR: Backing store data exceeds allowable hex digits\n");
	}

	std::vector<std::string> chunks;

	for (size_t i = 0; i < raw.size(); i += 2)
	{
		chunks.push_back(raw.substr(i, 2));
	}

	return chunks;
}

std::optional<std::string> PhysicalMemory::readBackingStore(unsigned long long ID)
{
	std::unique_lock<std::recursive_mutex> readBackingStoreLock(backingStoreMutex);

	std::ifstream input("backing-store.txt");

	if (!input)
	{
		std::cerr << "ERROR: Cannot open backing-store.txt for readBackingStore\n\n";
		throw std::runtime_error("Error: Cannot open the backing-store.txt for readBackingStore\n");
	}

	std::string line;
	while (std::getline(input, line))
	{
		unsigned long long existingID;
		std::string existingData;

		std::istringstream inputStream(line);

		if (inputStream >> existingID >> existingData)
		{
			if (existingID == ID)
			{
				input.close();
				return existingData;
			}
		}
	}

	input.close();
	return std::nullopt;
}

void PhysicalMemory::updateData(unsigned long long ID, size_t key)
{
	std::unique_lock<std::recursive_mutex> updateLock(updateMutex);

	std::optional<std::string> data = readBackingStore(ID);
	std::vector<std::string> parsed;

	if (!data)
	{
		std::cerr << "\n                                              ERROR: Frame not in backing store\n";
		throw std::runtime_error("Error: Frame not in backing store\n");
		return;
	}
	else
	{
		parsed = parseData(*data);
	}

	unsigned long long index = 0;
	for (index; index < parsed.size(); index++)
	{
		std::ostringstream stream;
		stream << std::hex << std::uppercase << std::setw(5) << std::setfill('0') << index;
		std::string address = stream.str();

		if (frames[key])
		{
			frames[key]->write(address, parsed[index]);
		}
	}

	for (index; index < frameSize; index++)
	{
		std::ostringstream stream;
		stream << std::hex << std::uppercase << std::setw(5) << std::setfill('0') << index;
		std::string address = stream.str();

		frames[key]->write(address, "00");
	}
}

// ----- << Public Member Function Implementation >> ----- //
PhysicalMemory::PhysicalMemory(size_t frameSize, size_t overallSize)
	: frameSize(frameSize), overallSize(overallSize), dataPlaceholder("")
{
	numFrames = overallSize / frameSize;
	frames = std::vector<std::optional<Frame>>(numFrames, std::nullopt);

	for (size_t i = 0; i < frameSize; i++)
	{
		dataPlaceholder += "00";
	}
}

PhysicalMemory::~PhysicalMemory() = default;


std::string PhysicalMemory::convertDecToHex(size_t dec)
{
	std::ostringstream stream;

	stream << std::hex << std::uppercase;
	stream << std::setw(5) << std::setfill('0') << dec;

	return stream.str();
}

size_t PhysicalMemory::convertHexToDecimal(std::string hex)
{
	size_t result = 0;

	try
	{
		result = std::stoul(hex, nullptr, 16);
	}
	catch (const std::invalid_argument& e)
	{
		throw std::runtime_error("Invalid Hex String: " + hex);
	}
	catch (const std::out_of_range& e)
	{
		throw std::runtime_error("Hex String out of range: " + hex);
	}

	return result;
}

std::optional<size_t> PhysicalMemory::find(unsigned long long ID)
{
	for (size_t i = 0; i < numFrames; i++)
	{
		if (frames[i])
		{
			if (frames[i]->getID() == ID)
			{
				return i;
			}
		}
	}

	return std::nullopt;
}

std::optional<size_t> PhysicalMemory::findFreeFrame()
{
	for (size_t i = 0; i < numFrames; i++)
	{
		if (!frames[i])
		{
			return i;
		}
	}

	return std::nullopt;
}

std::optional<unsigned long long> PhysicalMemory::getFrameID(size_t index)
{
	if (frames[index])
	{
		return frames[index]->getID();
	}

	return std::nullopt;
}

size_t PhysicalMemory::getVictimKey()
{
	if (usageList.empty())
	{
		//throw std::runtime_error("\nERROR: No available frames to evict\n");
	}

	size_t victimKey = usageList.back();
	usageList.pop_back();

	return victimKey;
}

std::optional<size_t> PhysicalMemory::locateFrame(unsigned long long requestedFrame)
{
	for (size_t i = 0; i < numFrames; i++)
	{
		if (frames[i])
		{
			if (frames[i]->getID() == requestedFrame)
			{
				return i;
			}
		}
	}

	return std::nullopt;
}

void PhysicalMemory::overwriteBackingStore(size_t victimKey)
{
	std::unique_lock<std::recursive_mutex> overwriteBackingStoreLock(backingStoreMutex);
	unsigned long long victimID;

	if (frames[victimKey])
	{
		victimID = frames[victimKey]->getID();
	}
	else
	{
		throw std::runtime_error("\nERROR: Overwriting invalid physical memory index\n");
	}
	
	std::optional<std::string> frameData = compileFrameData(victimID);

	if (frameData)
	{
		writeBackingStore(victimID, *frameData);
	}
	else
	{
		std::cerr << "\nERROR: Trying to access frame not in memory for location " << victimKey << "\n";
	}
}

std::optional< std::string> PhysicalMemory::read(std::string address)
{
	std::unique_lock<std::mutex> readLock(operationMutex);

	std::pair<size_t, size_t> translatedAddress = translateAddress(address);
	size_t index = translatedAddress.first;
	size_t offset = translatedAddress.second;

	if (index >= numFrames)
	{
		return std::nullopt;
	}

	if (frames[index])
	{
		usageList.remove(index);
		usageList.push_front(index);

		return frames[index]->read(convertDecToHex(offset));
	}

	return std::nullopt;
}

size_t PhysicalMemory::remove(std::vector<unsigned long long> rmVector)
{
	std::unique_lock<std::mutex> frameLock(frameMutex);

	size_t removedFrames = 0;
	std::optional<size_t> victimFrame;
	for (unsigned long long ID : rmVector)
	{
		victimFrame = find(ID);

		if (victimFrame)
		{
			overwriteBackingStore(*victimFrame);

			frames[*victimFrame] = std::nullopt;
			removedFrames++;
		}
	}

	return removedFrames;
}

std::pair<size_t, size_t> PhysicalMemory::translateAddress(std::string address)
{
	size_t base = convertHexToDecimal(address);
	size_t pageNumber = base / frameSize;
	size_t offset = base % frameSize;

	//std::cout << "PageNumber: " << pageNumber << "\nOffset: " << offset << "\n";

	if (pageNumber >= numFrames)
	{
		throw std::out_of_range("\nERROR: Invalid address\n");
	}

	return std::make_pair(pageNumber, offset);
}

void PhysicalMemory::updateFrame(unsigned long long ID, size_t index)
{
	std::unique_lock<std::recursive_mutex> updateLock(updateMutex);
	// Update physical memory
	frames[index].emplace(ID, frameSize);

	// Fill the frame with the data from the backing store
	updateData(ID, index);

	usageList.push_front(index);
}

bool PhysicalMemory::write(std::string address, std::string data)
{
	std::unique_lock<std::mutex> writeLock(operationMutex);

	std::pair<size_t, size_t> translatedAddress = translateAddress(address);
	size_t index = translatedAddress.first;
	size_t offset = translatedAddress.second;

	if (data.size() > 2)
	{
		std::cout << "\nERROR: Data is larger than 1 byte @PhysicalMemory::write\n\n";
		return false;
	}

	if (index >= numFrames)
	{
		return false;
	}

	if (frames[index])
	{
		frames[index]->write(convertDecToHex(offset), data);

		usageList.remove(index);
		usageList.push_front(index);
		return true;
	}

	return false;
}

void PhysicalMemory::writeBackingStore(unsigned long long ID, std::string data)
{
	std::unique_lock<std::recursive_mutex> writeBackingStoreLock(backingStoreMutex);

	std::ifstream input("backing-store.txt");
	std::ofstream temp("backing-store.tmp", std::ios::trunc);

	if (!temp)
	{
		throw std::runtime_error("Error: cannot open temporary file for write");
	}

	bool inserted = false;
	bool updated = false;

	std::string line;

	if (input)
	{
		while (std::getline(input, line))
		{
			std::istringstream stream(line);
			unsigned long long existingID;
			std::string existingData;

			if (stream >> existingID >> existingData)
			{
				if (existingID == ID)
				{
					temp << ID << " " << data << "\n";
					updated = true;
				}
				else if (!updated && !inserted && existingID > ID)
				{
					temp << ID << " " << data << "\n";
					temp << line << "\n";
					inserted = true;
				}
				else
				{
					temp << line << "\n";
				}
			}
			else
			{
				temp << line << "\n";
			}
		}
	}

	if (!updated && !inserted)
	{
		temp << ID << " " << data << "\n";
	}

	input.close();
	temp.flush();
	temp.close();

	std::remove("backing-store.txt");
	std::rename("backing-store.tmp", "backing-store.txt");
}

void PhysicalMemory::print(size_t location)
{
	if (frames[location])
	{
		frames[location]->print();
	}
	else
	{
		std::cout << "ERROR: Trying to access empty physical memory frame\n";
	}
}

void PhysicalMemory::printFrames()
{
	std::unique_lock<std::mutex> frameLock(frameMutex);

	std::string str;
	for (size_t i = 0; i < numFrames; ++i)
	{
		if (frames.at(i))
		{
			str = "Location [" + std::to_string(i) + "]";
			std::cout << std::left << std::setw(20) << str << "stores frame " << frames[i]->getID() << "\n";
		}
		else
		{
			str = "Location [" + std::to_string(i) + "]";
			std::cout << std::left << std::setw(20) << str << "stores no frame\n";
		}
	}
}

void PhysicalMemory::lf()
{

}