#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the SUBTRACT instruction used to 
 *			subtract two variables, two literals, or a variable
 *			and a literal
 */
class Instruction_SUBTRACT : public Instruction
{
private:
	// User-defined Data Members
	std::string destination;
	std::string firstVariable;
	std::string secondVariable;
	uint16_t firstLiteral;
	uint16_t secondLiteral;

	PARAMETER_COMBINATION_VLM parameterCombination;

public:
	/**
	 * @brief	Class constructor for two variables
	 */
	Instruction_SUBTRACT(
		std::string destination,
		std::string first,
		std::string second
	);

	/**
	 * @brief	Class constructor for two literals
	 */
	Instruction_SUBTRACT(
		std::string destination,
		uint16_t first,
		uint16_t second
	);

	/**
	 * @brief	Class constructor for a variable and literal
	 */
	Instruction_SUBTRACT(
		std::string destination,
		std::string first,
		uint16_t second
	);

	/**
	 * @brief	Copy constructor
	 */
	Instruction_SUBTRACT(const Instruction_SUBTRACT& source);

	/**
	 * @brief	Clones the instruction
	 *
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the destination of the instruction
	 *
	 * @return	Name of the destination variable
	 */
	std::string getDestination() const;

	/**
	 * @brief	Get the parameter combination of the instruction
	 *
	 * @return	The parameter combination of the instruction
	 */
	PARAMETER_COMBINATION_VLM getInstructionCombination() const;

	/**
	 * @brief	Get the first literal
	 *
	 * @return	Value of the first literal
	 */
	uint16_t getLiteralFirst() const;

	/**
	 * @brief	Get the second literal
	 *
	 * @return	Value of the second literal
	 */
	uint16_t getLiteralSecond() const;

	/**
	 * @brief	Get the first variable
	 *
	 * @return	Name of the first variable
	 */
	std::string getVariableFirst() const;

	/**
	 * @brief	Get the second variable
	 *
	 * @return	Name of the second variable
	 */
	std::string getVariableSecond() const;
};
