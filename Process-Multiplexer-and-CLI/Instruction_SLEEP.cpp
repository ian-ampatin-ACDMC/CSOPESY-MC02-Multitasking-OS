// ----- << External Library >> ----- //
#include "Instruction_SLEEP.hpp"

// ----- << Implementation >> ----- //
Instruction_SLEEP::Instruction_SLEEP(uint8_t duration)
	: sleepDuration(duration)
{
	setInstructionType(SLEEP);
}

std::unique_ptr<Instruction> Instruction_SLEEP::clone() const 
{
	return std::make_unique<Instruction_SLEEP>(*this);
}

uint8_t Instruction_SLEEP::getSleepDuration() const
{
	return sleepDuration;
}
