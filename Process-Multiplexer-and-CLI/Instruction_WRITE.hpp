#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @breif	Derived class for the WRITE instruction used to write
 *			into a memory location
 */

class Instruction_WRITE : public Instruction
{
private:
	// User-defined Data Members
	uint16_t data;
	std::string memoryLocation;

public:
	/**
	 * @brief	Class constructor
	 */
	Instruction_WRITE(std::string memoryLocation, uint16_t data);

	/**
	 * @brief	Copy constructor
	 */
	Instruction_WRITE(const Instruction_WRITE& source);

	/**
	 * @brief	Clones an instruction
	 * 
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the address where the data is to be written
	 * 
	 * @return	The destination address
	 */
	std::string getAddress() const;

	/**
	 * @brief	Get the data that is going to be written to a 
	 *			memory address
	 */
	uint16_t getData() const;
};