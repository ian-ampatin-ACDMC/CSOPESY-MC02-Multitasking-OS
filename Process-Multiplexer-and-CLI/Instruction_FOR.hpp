#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"
#include <vector>

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the FOR instruction used to iterate 
 *			over a set of instructions N number of times
 */
class Instruction_FOR : public Instruction
{
private:
	// User-defined Data Members
	uint16_t repetitions;
	
	// Primitive Data Members

	// Member Objects
	std::vector<std::unique_ptr<Instruction>> instructions;
public:
	/**
	 * @brief	Class constructor
	 */
	Instruction_FOR(
		std::vector<std::unique_ptr<Instruction>>& newInstructions,
		uint16_t repeat
	);

	/**
	 * @brief	Copy constructor
	 */
	Instruction_FOR(const Instruction_FOR& source);

	/**
	 * @brief	Clone the instruction
	 * 
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the instructions repeated by the FOR instruction
	 * 
	 * @return	Instructions repeated by the instruction
	 */
	const std::vector<std::unique_ptr<Instruction>>& getInstructions();

	/**
	 * @brief	Get the number of times the FOR instruction will be 
	 *			repeating its instructions
	 * 
	 * @return	Repetitions of the FOR instruction
	 */
	uint16_t getRepetitions() const;
};