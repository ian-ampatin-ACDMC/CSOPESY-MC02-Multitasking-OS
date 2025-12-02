// ----- << External Library >> ----- //
#include "Instruction_PRINT.hpp"

// ----- << Implementation >> ------ //
Instruction_PRINT::Instruction_PRINT(std::string message)
	: message(std::move(message)), variablePrint(false)
{
	setInstructionType(PRINT);
}

Instruction_PRINT::Instruction_PRINT(std::string message, std::string name)
	: message(std::move(message)), variableName(name), variablePrint(true)
{
	setInstructionType(PRINT);
}

std::unique_ptr<Instruction> Instruction_PRINT::clone() const
{
	return std::make_unique<Instruction_PRINT>(*this);
}

bool Instruction_PRINT::containsVariable() const
{
	return variablePrint;
}

std::string Instruction_PRINT::getMessage() const
{
	return message;
}

std::string Instruction_PRINT::getVariableName() const
{
	if (variablePrint)
	{
		return variableName;
	}
	else
	{
		throw std::runtime_error("A variable was not included in making this instruction.");
	}
}