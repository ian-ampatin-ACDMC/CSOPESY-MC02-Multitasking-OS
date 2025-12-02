#pragma once

// ----- << External Libraray >> ----- //
#include "Frame.hpp"
#include <condition_variable>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

// ----- << Class >> ----- //
/**
 * @brief	A class that emulates Physical Memory that makes use 
 *			of paging (i.e., has frames)
 */
class PhysicalMemory
{
private:
	// Primitive Data Members
	size_t frameSize;
	size_t numFrames;
	size_t overallSize;

	// User-defined Data Members
	std::mutex frameMutex;
	std::mutex overwriteBackingStoreMutex;
	std::mutex printMutex;
	std::mutex readBackingStoreMutex;
	std::mutex writeBackingStoreMutex;
	std::mutex operationMutex;

	std::recursive_mutex backingStoreMutex;
	std::recursive_mutex updateMutex;

	std::string dataPlaceholder;


	std::list<unsigned long long> usageList;
	
	std::vector<std::optional<Frame>> frames;

private:
	/**
	 * @brief	Compiles the entire frame data into a single
	 *			string of hex digits
	 *
	 * @param	ID --
	 *			The unique ID that identifies a frame
	 *
	 * @return	A string of hex digits representing the data of the
	 *			frame starting with address location 0x00000
	 */
	std::optional<std::string>
		compileFrameData(unsigned long long ID);

	/**
	 * @brief	Parse raw data representation from backing store 
	 *			into 2 hexadecimal character chunks
	 * 
	 * @param	raw --
	 *			The raw hexadecimal character string from the 
	 *			backing store
	 * 
	 * @return	A vector containing the parsed data
	 */
	std::vector<std::string> parseData(std::string raw) const;

	/**
	 * @brief	Read a frame from the backing store
	 *
	 * @param	ID --
	 *			The unique ID that identifies a frame
	 */
	std::optional<std::string> readBackingStore(
		unsigned long long ID
	);

	/**
	 * @brief	Reads the backing store and overwrites the contents
	 *			of a frame in the physical memory
	 *
	 * @param	ID --
	 *			The unique ID that identifies a frame
	 *
	 * @param	memoryIndex --
	 *			The frame that gets replaced
	 */
	void updateData(unsigned long long ID, size_t memoryIndex);

public:
	/**
	 * @brief	Class constructor
	 */
	PhysicalMemory(size_t frameSize, size_t overallSize);

	/**
	 * @brief	Class destructor
	 */
	~PhysicalMemory();

	/**
	 * @brief	Conversts a decimal number to a hexadecimal string
	 *
	 * @param	dec --
	 *			The number that is being converted
	 *
	 * @return	An equivalent hexadecimal string representation of
	 *			the decimal number
	 */
	std::string convertDecToHex(size_t dec);

	/**
	 * @brief	Converts a string of hexadecmial numbers to a
	 *			decimal number
	 *
	 * @param	hex --
	 *			The string of hexadecimal characters is being
	 *			converted
	 *
	 * @return	Decimal equivalent of the hexadecimal string
	 */
	size_t convertHexToDecimal(std::string hex);

	/**
	 * @brief	Checks if a frame is in the Physical Memory.
	 *
	 * @param	ID --
	 *			The unique ID of the frame that is being searched
	 * 
	 * @return	null if the frame is not in the Physical Memory.
	 *			The key to the frame in the Physical Memory
	 */
	std::optional<size_t> find(unsigned long long ID);

	/**
	 * @brief	Find a free frame in the Physical Memory
	 *
	 * @return	Returns a key to a free frame if it finds
	 *			a free frame. Otherwise, a std::nullopt
	 */
	std::optional<size_t> findFreeFrame();

	/**
	 * @brief	Get the ID of a frame 
	 * 
	 * @param	index --
	 *			Location of the desired frame in the physical memory
	 * 
	 * @return	null if index has no frame loaded. 
	 *			The ID if found.
	 */
	std::optional<unsigned long long> getFrameID(size_t index);

	/**
	 * @brief	Get the key that is going to be evicted
	 * 
	 * @return	The victim key
	 */
	size_t getVictimKey();

	/**
	 * @brief	Locates a frame in the physical memory
	 * 
	 * @param	requestedFrame --
	 *			ID of the frame being requested
	 * 
	 * @return	The index of the frame in the physical memory.
	 *			null if the frame is not in the physical memory.
	 */
	std::optional<size_t> locateFrame(
		unsigned long long requestedFrame
	);

	/**
	 * @brief	Overwrite an existing copy of a frame in the backing
	 *			store with the current data in phyical memory
	 * 
	 * @param	victimKey --
	 *			The location of the frame, in physical memory, that
	 *			is going to be loaded into the backing store
	 */
	void overwriteBackingStore(size_t victimKey);

	/**
	 * @brief	Read from a memory location in the Physical Memory
	 *
	 * @param	address --
	 *			The address where the data is to be read from
	 * 
	 * @return	The data that was read
	 */
	std::optional<std::string> read(std::string address);

	/**
	 * @brief	Removes the frames with the corresponding IDs
	 * 
	 * @param	rmVector --
	 *			A vector containing the IDs of the frams that
	 *			need to be removed
	 */
	size_t remove(std::vector<unsigned long long> rmVector);

	/**
	 * @brief	Tranlsates raw memory address to a page number and
				page offset pair
	 *
	 * @param	address --
	 *			The raw address is being accessed
	 *
	 * @return	A string value pair containing the page number
	 *			and the offset
	 */
	std::pair<size_t, size_t> translateAddress(std::string address);

	/**
	 * @brief	Sequence of instructions to update physical memory
	 *			with a new frame and frame data
	 *
	 * @param	backingStoreID --
	 *			The unique ID that identifies a frame in the 
	 *			backing store
	 *
	 * @param	index --
	 *			The index of the frame, in physical memory, that is
	 *			treated as the victim frame
	 */
	void updateFrame(
		unsigned long long backingStoreID,
		size_t index
	);

	/**
	 * @brief	Write to a memory location in the Physical Memory
	 * 
	 * @param	address --
	 *			The address that is going to be written to
	 * 
	 * @param	data --
	 *			The data that is going to be written
	 */
	bool write(std::string address, std::string data);

	/**
	 * @brief	Write a frame to the backing store and remove
	 *			it from physical memory
	 *
	 * @param	ID --
	 *			The unique ID that identifies a frame
	 *
	 * @param	data --
	 *			Data that a frame contains
	 */
	void writeBackingStore(
		unsigned long long ID, 
		std::string data
	);

	/**
	 * @brief	Prints the data in a frame
	 * 
	 * @param	location --
	 *			The index of the frame that is being printed
	 */
	void print(size_t location);

	/**
	 *
	 */
	void printFrames();

	void lf();
};