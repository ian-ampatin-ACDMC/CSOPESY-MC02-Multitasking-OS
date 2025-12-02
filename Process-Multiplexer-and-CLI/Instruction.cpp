// ----- << External Library >> ----- //
#include "Instruction.hpp"

// ----- << Protected Member Function Implementation >> ----- //
void Instruction::setInstructionType(INSTRUCTION_TYPE type)
{
	instructionType = type;
}

// ----- << Public Member Function Implementation >> ----- //
Instruction::Instruction() : instructionType(_UNSET)
{

}

Instruction::Instruction(const Instruction& source) :
	instructionType(source.instructionType)
{

}

Instruction::~Instruction() = default;

INSTRUCTION_TYPE Instruction::getInstructionType() const
{
	return instructionType;
}