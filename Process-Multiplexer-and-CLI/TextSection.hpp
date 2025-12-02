#pragma once

// ----- << External Library >> ----- // 
#include "Instruction.hpp"
#include <vector>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates the text section of a process
 *			which are the instructions that the process will 
 *			sequentially execute
 */
class TextSection
{
private:
	// User-defined Data Members

	// Primitive Data Members

	// Object Members
	std::vector<std::unique_ptr<Instruction>> instructions;
public:
	/**
	 * @brief	Class constructor
	 */
	TextSection();

	/**
	 * @brief	Copy constructor (deleted)
	 */
	TextSection(const TextSection&) = delete;

	/**
	 * @brief	Copy assignment operator (deleted)
	 */
	TextSection& operator=(const TextSection&) = delete;

	/**
	 * @brief	Move constructor
	 */
	TextSection(TextSection&& source) noexcept;

	/**
	 * @brief	Move assignment operator
	 */
	TextSection& operator=(TextSection&& source) noexcept;

	/**
	 * @brief	Class destructor
	 */
	~TextSection();

	/**
	 * @brief	Add an instruction to the text section
	 * 
	 * @param	instruction --
	 *			The instructions that is going to be added
	 */
	void addInstruction(std::unique_ptr<Instruction> instruction);

	/**
	 * @brief	Get one instruction from the text section
	 * 
	 * @param	index --
	 *			Location of the instruction
	 * 
	 * @return	A reference to the selected instruction
	 */
	const std::unique_ptr<Instruction>& 
		getInstruction(size_t index) const;

	/**
	 * @brief	Get a reference to the instructions stored in the 
	 *			text section
	 * 
	 * @return	A reference to the instructions of the text section
	 */
	const std::vector<std::unique_ptr<Instruction>>& 
		getInstructionAll() const;
};