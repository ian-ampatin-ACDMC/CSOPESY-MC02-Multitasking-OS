// ----- << External Library >> ----- //
#include "PCB.hpp"

// ----- << Implementation >> ----- //
PCB::PCB(unsigned long long ID, size_t heapSize, std::vector<std::unique_ptr<Instruction>> instructions, size_t priority, size_t memoryRequired)
	: processID(ID), processState(PROCESS_STATE::NEW), programCounter(0),
	priority(priority), process(ID, heapSize, instructions), memoryRequired(memoryRequired)
{
	processName = std::to_string(ID);
}

PCB::~PCB() = default;

void PCB::appendLog(std::string log)
{
	processLog += log + "\n";
}

std::string PCB::getLog() const
{
	return processLog;
}

size_t PCB::getMemoryRequired() const
{
	return memoryRequired;
}

std::string PCB::getName() const
{
	return processName;
}

size_t PCB::getPriority() const
{
	return priority;
}

Process& PCB::getProcess()
{
	return process;
}

unsigned long long PCB::getProcessID() const
{
	return processID;
}

size_t PCB::getProgramCounter() const
{
	return programCounter;
}

PCB::PROCESS_STATE PCB::getState() const
{
	return processState;
}

void PCB::incrementProgramCounter()
{
	programCounter++;
}

void PCB::setName(std::string name)
{
	processName = name;
}

void PCB::setState(PROCESS_STATE newState)
{
	processState = newState;
}