// ----- << External Library >> ----- //
#include "Process.hpp"

// ----- << Implementation >> ----- //
Process::Process(unsigned long long ID, size_t heapSize, std::vector<std::unique_ptr<Instruction>>& instructions)
	: logicalDataSection(32),
	processID(ID), heap(heapSize)
{
	for (auto& instruction : instructions)
	{
		textSection.addInstruction(std::move(instruction));
	}
}

Process::Process(Process&& source) noexcept
	: logicalDataSection(std::move(source.logicalDataSection)),
	processID(source.processID), dataSection(std::move(source.dataSection)),
	heap(std::move(source.heap)), stack(std::move(source.stack)),
	textSection(std::move(source.textSection))
{

}

Process& Process::operator=(Process&& source) noexcept
{
	if (this != &source)
	{
		processID			= source.processID;
		dataSection			= std::move(source.dataSection);
		heap				= std::move(source.heap);
		stack				= std::move(source.stack);
		textSection			= std::move(source.textSection);
		logicalDataSection	= std::move(source.logicalDataSection);
	}

	return *this;
}

Process::~Process() = default;

DataSection& Process::getDataSection()
{
	return dataSection;
}

Heap& Process::getHeap()
{
	return heap;
}

LogicalDataSection& Process::getLogicalDataSection()
{
	return logicalDataSection;
}

Stack& Process::getStack()
{
	return stack;
}

TextSection& Process::getTextSection()
{
	return textSection;
}