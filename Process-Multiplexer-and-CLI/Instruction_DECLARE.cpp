// ----- << External Library >> ----- //
#include "Instruction_DECLARE.hpp"

// ----- << Implementation >> ------ //
Instruction_DECLARE::Instruction_DECLARE(std::string name)
	: variableName(name), variableData(0), initialized(false)
{
	setInstructionType(DECLARE);
}

Instruction_DECLARE::Instruction_DECLARE(std::string name, uint16_t data)
	: variableName(name), variableData(data), initialized(true)
{
	setInstructionType(DECLARE);
}

Instruction_DECLARE::Instruction_DECLARE(const Instruction_DECLARE& source)
	: variableName(source.variableName), variableData(source.variableData),
	initialized(source.initialized)
{
	setInstructionType(DECLARE);
}

std::unique_ptr<Instruction> Instruction_DECLARE::clone() const
{
	return std::make_unique<Instruction_DECLARE>(*this);
}