// ----- << External Library >> ----- //
#include "Instruction_READ.hpp"

// ----- << Implementation >> ----- //
Instruction_READ::Instruction_READ(std::string memoryLocation, std::string destinationVariable)
	: memoryLocation(memoryLocation), destinationVariable(destinationVariable)
{
	setInstructionType(READ);
}

Instruction_READ::Instruction_READ(const Instruction_READ& source) 
	: memoryLocation(source.memoryLocation), destinationVariable(source.destinationVariable)
{
	setInstructionType(READ);
}

std::unique_ptr<Instruction> Instruction_READ::clone() const
{
	return std::make_unique<Instruction_READ>(*this);
}

std::string Instruction_READ::getAddress() const
{
	return memoryLocation;
}

std::string Instruction_READ::getDestination() const
{
	return destinationVariable;
}