// ----- << External Library >> ----- //
#include "Instruction_ADD.hpp"

// ----- << Implementation >> ------ //
Instruction_ADD::Instruction_ADD( std::string destination, std::string first, std::string second) 
	: destination(destination), firstVariable(first), secondVariable(second), 
	firstLiteral(0), secondLiteral(0), parameterCombination(VARIABLE)
{
	setInstructionType(ADD);
}

Instruction_ADD::Instruction_ADD(std::string destination, uint16_t first, uint16_t second)
	: destination(destination), firstLiteral(first), secondLiteral(second),
	firstVariable(""), secondVariable(""), parameterCombination(LITERAL)
{
	setInstructionType(ADD);
}

Instruction_ADD::Instruction_ADD(std::string destination, std::string first, uint16_t second)
	: destination(destination), firstVariable(first), secondLiteral(second),
	firstLiteral(0), secondVariable(""), parameterCombination(MIXED)
{
	setInstructionType(ADD);
}

Instruction_ADD::Instruction_ADD(const Instruction_ADD& source)
	: destination(source.destination), firstLiteral(source.firstLiteral), secondLiteral(source.secondLiteral),
	firstVariable(source.firstVariable), secondVariable(source.secondVariable), parameterCombination(source.parameterCombination)
{
	setInstructionType(ADD);
}

std::unique_ptr<Instruction> Instruction_ADD::clone() const
{
	return std::make_unique<Instruction_ADD>(*this);
}

uint16_t Instruction_ADD::getLiteralFirst() const
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

uint16_t Instruction_ADD::getLiteralSecond() const
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

std::string Instruction_ADD::getVariableFirst() const
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

std::string Instruction_ADD::getVariableSecond() const
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