// ----- << External Library >> ----- //
#include "Instruction_FOR.hpp"

// ----- << Implementation >> ------ //
Instruction_FOR::Instruction_FOR(std::vector<std::unique_ptr<Instruction>>& newInstructions, uint16_t repeat)
	: repetitions(repeat)
{
	for (auto& instruction : newInstructions)
	{
		instructions.push_back(std::move(instruction));
	}

	setInstructionType(FOR);
}

Instruction_FOR::Instruction_FOR(const Instruction_FOR& source)
	: repetitions(source.repetitions)
{
	for (const auto& instruction : source.instructions)
	{
		instructions.push_back(instruction->clone());
	}

	setInstructionType(FOR);
}

std::unique_ptr<Instruction> Instruction_FOR::clone() const
{
	return std::make_unique<Instruction_FOR>(*this);
}

const std::vector<std::unique_ptr<Instruction>>& Instruction_FOR::getInstructions()
{
	return instructions;
}

uint16_t Instruction_FOR::getRepetitions() const
{
	return repetitions;
}