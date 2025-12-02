#pragma once

// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Class >> ----- //

/**
 * @brief	Derived class for the SLEEP instruction that shuts the 
 *			Core down for N number of ticks
 */
class Instruction_SLEEP : public Instruction
{
private:
	// User-defined Data Members
	uint8_t sleepDuration;
public:
	/**
	 * @brief	Class constructor
	 */
	Instruction_SLEEP(uint8_t duration);

	/**
	 * @brief	Clones the instruction
	 *
	 * @return	A clone of the instruction
	 */
	std::unique_ptr<Instruction> clone() const override;

	/**
	 * @brief	Get the sleep duration
	 * 
	 * @return	Duration of the sleep
	 */
	uint8_t getSleepDuration() const;
};