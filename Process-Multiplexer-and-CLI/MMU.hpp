#pragma once

// ----- << External Library >> ----- //
#include "Frame.hpp"
#include "PhysicalMemory.hpp"
#include <unordered_map>

// ----- << Class >> ----- //
/**
 * @brief	A class that emulates the functions of a 
 *			Memory Manager Unit
 */
class MMU
{
private:
	/**
	 * @brief	A structure that represents a page
	 */
	struct Page
	{
		bool dirty = false;
		bool valid = false;
		unsigned long long frameID = -1;
	};

	/**
	 * @brief	A structure that represents a page table of an
	 *			individual process
	 */
	struct PageTable
	{
		std::vector<Page> pages;
		size_t limit = 0;
	};
	
	// Primitive Data Members
	bool& breaker;
	unsigned long long framesCreated;

	// User-defined Data Members
	size_t availableMemory;

	size_t limitHigh;

	size_t maximumMemoryPerProcess;
	size_t maximumOverallMemory;
	size_t memoryPerFrame;
	size_t minimumMemoryPerProcess;

	size_t numFramesPerProcess;

	size_t pagesIn;
	size_t pagesOut;

	std::mutex createPageMutex;
	std::mutex pageFaultMutex;
	std::mutex readMutex;
	std::mutex writeMutex;
	std::mutex loadingMutex;

	std::recursive_mutex readWriteMutex;

	std::string dataPlaceholder;

	std::unordered_map<unsigned long long, PageTable> masterTable; 
			// Process ID, Corresponding Page Table
		
	// Object Members
	PhysicalMemory physicalMemory;

	/**
	 * @brief	Formats data into a vector of strings where each
	 *			string consists of 2 characters
	 * 
	 * @param	raw --
	 *			The string of hexadecimal characters that is going
	 *			to be formatted
	 * 
	 * @return	A vector containing the formatted strings
	 */
	std::vector<std::string> dataFormat(std::string raw);

	/**
	 * @brief	Get the page that has the corresponding frameID
	 * 
	 * @param	ID --
	 *			The frameID of the page being searched
	 * 
	 * @return	A pointer to the page
	 */
	Page* getPage(unsigned long long ID);

	/**
	 * @brief	Get the index of a page in a page table give
	 *			a frameID
	 * 
	 * @param	frameID --
	 *			The frameID of the page being search for
	 * 
	 * @return	The index of a page in the page table
	 */
	size_t getPageIndex(unsigned long long ID);

	/**
	 * @brief	Checks if the page is valid given a frameID
	 *
	 * @param	processID --
	 *			ID of the process whose page table is being used
	 *			as a reference
	 *
	 * @param	frameID --
	 *			ID of the frame being checked
	 *
	 * @return	true if the page is valid (in the physical
	 *			memory). Otherwise, true.
	 */
	bool isValid(
		unsigned long long processID,
		unsigned long long frameID
	);

	/**
	 * @brief	Checks if a process is accessing the correct frame
	 *
	 * @param	processID --
	 *			The ID of the process attempting to access memory
	 *
	 * @param	physicalMemoryLocation --
	 *			Loation in the physical memory that the process
	 *			is attempting to access
	 *
	 * @return	true if it is accessing a frame that the process
	 *			owns. Otherwise, false.
	 */
	bool validAccess(
		unsigned long long processID,
		size_t physicalMemoryLocation
	);
public:
	/**
	 * @brief	Class constructor
	 */
	MMU(
		size_t maximumMemoryPerProcess,
		size_t maximumOverallMemory,
		size_t memorPerFrame,
		size_t minimumMemoryPerProcess,
		bool& breaker
	);

	/**
	 * @brief	Class destructor
	 */
	~MMU();

	/**
	 * @brief	Counts how many frames a process owns that are
	 *			currently contained in physical memory
	 * 
	 * @param	processID --
	 *			ID of the process whose frames are being checked
	 * 
	 * @return	The number of frames a process has in
	 *			physical memory
	 */
	size_t countValid(unsigned long long processID);

	/**
	 * @brief	Creates a page for a process when a process is first
	 *			created or scheduled by the CPU
	 * 
	 * @param	processID --
	 *			ID of the process that the MMU will create pages and
	 *			frames for
	 */
	void createPages(unsigned long long processID);

	/**
	 * @brief	Creats a page for a process when a process if first
	 *			created or scheduler by the CPU but has a 
	 *			specified amount of memory that it consumes
	 * 
	 * @param	processID --
	 *			ID of the process that the MMU will create pages and
	 *			frames for
	 * 
	 * @param	requiredMemory --
	 *			The amount of memory that the Process requires to
	 *			operate
	 */
	void createPages(
		unsigned long long processID,
		size_t requriedMemory
	);

	/**
	 * @brief	Get the available memory the MMU has
	 * 
	 * @return	The amount of available memory the MMU can allocated
	 */
	size_t getAvailableMemory() const;

	/**
	 * @brief	Get the number of pages the MMU had to load from
	 *			the backing store
	 * 
	 * @return	The number of times page in operations were executed
	 */
	size_t getPagesIn() const;

	/**
	 * @brief	Get the number of pages the MMU had to store into
	 *			the backing store (from page faults and 
	 *			removing the pages of a process when it termiantes)
	 * 
	 * @return	The number of times page out operations were 
	 *			executed
	 */
	size_t getPagesOut() const;

	/**
	 * @brief	Handles a page fault
	 * 
	 *			WARNING: Does not take into account a Hit or Miss
	 * 
	 * @param	processID --
	 *			The process whose page is not currently in physical
	 *			memory
	 * 
	 * @param	requestedPage --	
	 *			The ID of the page that a process is requesting for
	 */
	void handlePageFault(
		unsigned long long processID,
		unsigned long long reqeustedPage
	);

	/**
	 * @brief	Load all of the frames of a process into the physical
	 *			memory
	 * 
	 * @param	processID --
	 *			ID of the process whose frames need to be loaded
	 *			into physical memory
	 */
	bool loadProcess(unsigned long long processID);

	/**
	 * @brief	Prints the frames that are stored in physical
	 *			memory
	 */
	void printFrames();

	/**
	 * @brief	Prints the master table
	 */
	void printMasterTable();

	/**
	 * @brief	Reada data at the address based on process memory
	 * 
	 *			0x0000 does not necessarily translate to the 0x0000
	 *			address in the physical memory. It can be at the 
	 *			start of any frame since mapping is based on 
	 *			process pages
	 *
	 * @param	processID --
	 *			ID of the process that is trying to read from the
	 *			physical memory
	 *
	 * @param	address --
	 *			Hexadecimal address for the read operation
	 *
	 *			Serves as the starting address if data is larger
	 *			than 1 byte (2 hexadecimal characters)
	 *
	 * @param	bytesToRead --
	 *			The number of bytes to read from the physical memory
	 *
	 * @return	The bytes read if the read operation is successful.
	 *			Otherwise, null.

	 */
	std::optional<std::string> protectedRead(
		unsigned long long processID,
		std::string address,
		size_t bytesToRead
	);

	/**
	 * @brief	Writes data at the address based on process memory
	 * 
	 *			0x0000 does not necessarily translate to the 0x0000
	 *			address in the physical memory. It can be at the 
	 *			start of any frame since mapping is based on 
	 *			process pages
	 *
	 * @param	processID --
	 *			ID of the process that is trying to write into
	 *			physical memory
	 *
	 * @param	address --
	 *			Hexadecimal address for the write operation.
	 *
	 *			Serves as the starting address if data is larger
	 *			than 1 byte (2 hexadecimal characters)
	 *
	 * @param	data --
	 *			The data that is being written into memory.
	 * 
	 *			NOTE: data must be an equivalent hexadecimal
	 *			representation
	 *
	 * @return	true if the write operation is successful.
	 *			Otherwise, false.
	 */
	bool protectedWrite(unsigned long long processID,
		std::string address,
		std::string data
	);

	/**
	 * @brief	Reada data at the address based on physical memory
	 *			addresses
	 * 
	 * @param	processID --
	 *			ID of the process that is trying to read from the 
	 *			physical memory
	 * 
	 * @param	address --
	 *			Hexadecimal address for the read operation
	 * 
	 *			Serves as the starting address if data is larger
	 *			than 1 byte (2 hexadecimal characters)
	 * 
	 * @param	bytesToRead --
	 *			The number of bytes to read from the physical memory
	 * 
	 * @return	The bytes read if the read operation is successful.
	 *			Otherwise, null.
	 */
	std::optional<std::string> read(
		unsigned long long processID,
		std::string address,
		size_t bytesToRead
	);

	/**
	 * @brief	Removes all of the frames belonging to a process
	 *			from the physical memory
	 * 
	 * @param	processID --
	 *			ID of the process whose frames are being removed
	 */
	void remove(unsigned long long processID);

	/**
	 * @brief	Writes data at the address based on physical memory
	 *			addresses
	 * 
	 * @param	processID --
	 *			ID of the process that is trying to write into
	 *			physical memory
	 * 
	 * @param	address --
	 *			Hexadecimal address for the write operation.
	 *		
	 *			Serves as the starting address if data is more than
	 *			1 byte (2 hexadecimal characters)
	 * 
	 * @param	data --
	 *			The data that is being written into memory
	 * 
	 * @return	true if the write operation is successful.
	 *			Otherwise, false.
	 */
	bool write(unsigned long long processID, 
		std::string address, 
		std::string data
	);
};