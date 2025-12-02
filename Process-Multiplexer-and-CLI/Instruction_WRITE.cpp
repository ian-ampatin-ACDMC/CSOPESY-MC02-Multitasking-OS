// ----- << External Library >> ----- //
#include "Instruction_WRITE.hpp"

// ----- << Implementation >> ----- //
Instruction_WRITE::Instruction_WRITE(std::string memoryLocation, uint16_t data)
	: memoryLocation(memoryLocation), data(data)
{
	setInstructionType(WRITE);
}

Instruction_WRITE::Instruction_WRITE(const Instruction_WRITE& source)
	: memoryLocation(source.memoryLocation), data(source.data)
{
	setInstructionType(WRITE);
}

std::unique_ptr<Instruction> Instruction_WRITE::clone() const
{
	return std::make_unique<Instruction_WRITE>(*this);
}

std::string Instruction_WRITE::getAddress() const
{
	return memoryLocation;
}

uint16_t Instruction_WRITE::getData() const
{
	return data;
}