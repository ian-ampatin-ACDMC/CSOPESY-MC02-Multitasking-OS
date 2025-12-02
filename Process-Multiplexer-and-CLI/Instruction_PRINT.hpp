#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the PRINT instruction used to
 *			print text in the interface or have the message added
 *			to the log
 */
class Instruction_PRINT : public Instruction
{
private:
	// User-defined Data Members
	std::string message;
	std::string variableName;

	// Primitive Data Members
	bool variablePrint;

public:
	/**
	 * @brief	Class constructor when only a message needs to be
	 *			printed
	 */
	Instruction_PRINT(std::string message);

	/**
	 * @brief	Class constructor when both a message and a variable
	 *			needs to be printed
	 */
	Instruction_PRINT(std::string message, std::string name);

	/**
	 * @brief	Clone method
	 * 
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Checks if the instruction prints variable data
	 * 
	 * @return	True if the instruction prints variable data.
	 *			Otherwise, false.
	 */
	bool containsVariable() const;

	/**
	 * @brief	Get the message
	 * 
	 * @return	The message that the instruction needs to print
	 */
	std::string getMessage() const;

	/**
	 * @brief	Get the name of the variable
	 * 
	 * @return	Name of the variable whose contents need to be
	 *			printed
	 */
	std::string getVariableName() const;
};

