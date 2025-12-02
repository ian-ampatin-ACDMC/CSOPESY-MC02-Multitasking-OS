#pragma once

// ----- << External Library >> ----- //
#include <map>
//#include <mutex>
#include <string>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a frame for PHYSICAL MEMORY
 */
class Frame
{
private:
	// Primitive Data Members
	unsigned long long frameID;

	// User-defined Data Members
	size_t frameSize; // in bytes

	std::map<std::string, std::string> frameData;
			// Address in Hex : Byte


	// User-defined Data Members
	//std::mutex frameWriteMutex;
public:
	/**
	 * @brief	Class constructor
	 */
	Frame(unsigned long long ID, size_t size);

	/**
	 * @brief	Class destructor
	 */
	~Frame();

	/**
	 * @brief	Get the ID of the frame
	 * 
	 * @return	The ID of the frame
	 */
	unsigned long long getID() const;

	/**
	 * @brief	Read data from the frame
	 * 
	 * @param	frameAddress --
	 *			Determines which address in the frame is
	 *			going to be read
	 * 
	 * @return	Data that was read from the frame
	 */
	std::string read(std::string frameAddress);

	/**
	 * @brief	Write data into one of the addresses of the frame
	 * 
	 * @param	frameAddress --
	 *			Determines etermine which address in the frame the 
	 *			data is going to be written in 
	 * 
	 * @param	data --
	 *			The data that is going to be written into the 
	 *			address location
	 */
	void write(std::string frameAddress, std::string data);

	// ----- || Operators || ----- //

	/**
	 * @brief	Equality operator for frame objects
	 * 
	 * @param	frame --
	 *			The Frame object that is being compared to
	 *			this instance
	 * 
	 * @return	true if both instances are considered equal
	 *			Otherwise, false
	 */
	bool operator==(const Frame& frame) const;

// TODO: Put under protected once debugging is finished

	/**
	 * @brief	Prints the contents of the frame
	 */
	void print() const;
};

/**
 * @brief	Customized hash function for the Frame class
 */
struct FrameHash
{
	std::size_t operator()(const Frame& frame) const;
};