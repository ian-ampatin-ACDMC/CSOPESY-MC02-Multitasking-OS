#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the READ instruction which reads from
 *			the memory and stores it into a variable
 */

class Instruction_READ : public Instruction
{
private:
	// User-defined Data Members
	std::string memoryLocation;
	std::string destinationVariable;
public:
	/**
	 * @brief	Class constructor
	 */
	Instruction_READ(
		std::string memoryLocation, 
		std::string destinationVariable
	);

	/**
	 * @brief	Copy constructor
	 */
	Instruction_READ(const Instruction_READ& source);

	/**
	 * @brief	Clone the instruction
	 * 
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the address for the READ instruction
	 */
	std::string getAddress() const;

	/**
	 * @brief	Get the destination variable for the READ
	 */
	std::string getDestination() const;
};