#pragma once

// ----- << External Library >> ----- //
#include "data.hpp"
#include <memory>
#include <stdexcept>

// ----- << Class >> ----- //

/**
 * @brief	Base class for the various instructions that the system
 *			can perform
 */
class Instruction
{
private:
	// User-derfined Data Members

	// Primitive Data Members

protected:
	// User-derfined Data Members
	INSTRUCTION_TYPE instructionType;

	/**
	 * @brief	Set the type of the instruction
	 * 
	 * @param	type --
	 *			Is the INSTRUCTION_TYPE that will be set to the
	 *			instruction
	 */
	void setInstructionType(INSTRUCTION_TYPE type);

public:
	/**
	 * @brief	Class constructor
	 */
	Instruction();

	/**
	 * @brief	Copy constructor
	 */
	Instruction(const Instruction& source);

	/**
	 * @brief	Virtual class destructor
	 */
	virtual ~Instruction();

	/**
	 * @brief	Pure virtual clone function
	 */
	virtual std::unique_ptr<Instruction> clone() const = 0;

	/**
	 * @brief	Get the type of the instruction
	 *
	 * @return	The type of the instruction
	 */
	INSTRUCTION_TYPE getInstructionType() const;
};