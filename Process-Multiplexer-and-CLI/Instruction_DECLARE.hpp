#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the DECLARE instruction used to add
 *			a variable to the data section of a process
 */
class Instruction_DECLARE : public Instruction
{
private:
	// User-defined Data Members
	std::string variableName;
	uint16_t variableData;

	// Primitive Data Members
	bool initialized;

public:
	/**
	 * @brief	Class constructor with only the variable name given
	 */
	Instruction_DECLARE(std::string name);

	/**
	 * @brief	Class constructor with the variable name and data
	 *			given
	 */
	Instruction_DECLARE(std::string name, uint16_t data);

	/**
	 * @brief	Copy constructor
	 */
	Instruction_DECLARE(const Instruction_DECLARE& source);

	/**
	 * @brief	Clone the instruction
	 *
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the data of the variable
	 *
	 * @return	Data of the variable
	 */
	uint16_t getData() const
	{
		return variableData;
	}

	/**
	 * @brief	Get the name of the variable
	 *
	 * @return	Name of the variable
	 */
	std::string getName() const
	{
		return variableName;
	}

	/**
	 * @brief	Checks if the declared variable is initialized
	 *			or uninitialized
	 * 
	 * @return	True if the the instruction is declaring an 
	 *			initialized variable. Otherwise, false.
	 */
	bool variableInitialized() const
	{
		return initialized;
	}
};