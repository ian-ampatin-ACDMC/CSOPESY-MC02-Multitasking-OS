// ----- << External Library >> ----- //
#include "Instruction_SUBTRACT.hpp"

// ----- << Implementation >> ------ //
Instruction_SUBTRACT::Instruction_SUBTRACT(std::string destination, std::string first, std::string second)
	: destination(destination), firstVariable(first), secondVariable(second),
	firstLiteral(0), secondLiteral(0), parameterCombination(VARIABLE)
{
	setInstructionType(SUBTRACT);
}

Instruction_SUBTRACT::Instruction_SUBTRACT(std::string destination, uint16_t first, uint16_t second)
	: destination(destination), firstLiteral(first), secondLiteral(second),
	firstVariable(""), secondVariable(""), parameterCombination(LITERAL)
{
	setInstructionType(SUBTRACT);
}

Instruction_SUBTRACT::Instruction_SUBTRACT(std::string destination, std::string first, uint16_t second)
	: destination(destination), firstVariable(first), secondLiteral(second),
	firstLiteral(0), secondVariable(""), parameterCombination(MIXED)
{
	setInstructionType(SUBTRACT);
}

Instruction_SUBTRACT::Instruction_SUBTRACT(const Instruction_SUBTRACT& source)
	: destination(source.destination), firstLiteral(source.firstLiteral), secondLiteral(source.secondLiteral),
	firstVariable(source.firstVariable), secondVariable(source.secondVariable), parameterCombination(source.parameterCombination)
{
	setInstructionType(SUBTRACT);
}

std::unique_ptr<Instruction> Instruction_SUBTRACT::clone() const
{
	return std::make_unique<Instruction_SUBTRACT>(*this);
}

std::string Instruction_SUBTRACT::getDestination() const
{
	return destination;
}

PARAMETER_COMBINATION_VLM Instruction_SUBTRACT::getInstructionCombination() const
{
	return parameterCombination;
};

uint16_t Instruction_SUBTRACT::getLiteralFirst() const
{
	if (parameterCombination == VARIABLE || parameterCombination == MIXED)
	{
		throw std::runtime_error("First Literal not used to create instruction.");
	}
	else
	{
		return firstLiteral;
	}
}

uint16_t Instruction_SUBTRACT::getLiteralSecond() const
{
	if (parameterCombination == VARIABLE)
	{
		throw std::runtime_error("Second Literal not used to create instruction.");
	}
	else
	{
		return secondLiteral;
	}
}

std::string Instruction_SUBTRACT::getVariableFirst() const
{
	if (parameterCombination == LITERAL)
	{
		throw std::runtime_error("First Variable not used to create instruction.");
	}
	else
	{
		return firstVariable;
	}
}

std::string Instruction_SUBTRACT::getVariableSecond() const
{
	if (parameterCombination == LITERAL || parameterCombination == MIXED)
	{
		throw std::runtime_error("Second Variable not used to create instruction.");
	}
	else
	{
		return secondVariable;
	}
}