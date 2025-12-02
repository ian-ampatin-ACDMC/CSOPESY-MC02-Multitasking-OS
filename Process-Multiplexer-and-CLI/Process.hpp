#pragma once

// ----- << External Library >> ----- //
#include "DataSection.hpp"
#include "Heap.hpp"
#include "Stack.hpp"
#include "TextSection.hpp"

#include "LogicalDataSection.hpp"

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a system process
 */
class Process
{
private:
	// User-defined Data Members

	// Primitive Data Members
	unsigned long long processID;

	// Member Objects
	DataSection dataSection;
	Heap heap;							// Currently not used
	Stack stack;						// Currently not used
	TextSection textSection;

	LogicalDataSection logicalDataSection;
public:
	/**
	 * @brief	Class constructor
	 */
	Process(
		unsigned long long ID,
		size_t heapSize,
		std::vector<std::unique_ptr<Instruction>>& instructions
	);

	/**
	 * @brief	Copy constructor (deleted)
	 */
	Process(const Process&) = delete;

	/**
	 * @brief	Copy assignment operator (deleted)
	 */
	Process& operator=(const Process&) = delete;

	/**
	 * @brief	Move constructor
	 */
	Process(Process&& source) noexcept;

	/**
	 * @brief	Move assignment operator
	 */
	Process& operator=(Process&& source) noexcept;

	/**
	 * @brief	Class destructor
	 */
	~Process();

	/**
	 * @brief	Get the data section of the process
	 * 
	 * @return	A reference to the data section
	 */
	DataSection& getDataSection();

	/**
	 * @brief	Get the heap of the process
	 * 
	 * @return	A reference to the heap 
	 */
	Heap& getHeap();

	/**
	 * @brief	Get the logical data section of the process
	 * 
	 * @return	A reference to the logical data section
	 */
	LogicalDataSection& getLogicalDataSection();

	/**
	 * @brief	Get the stack of the process
	 * 
	 * @return	A reference to the stack
	 */
	Stack& getStack();

	/**
	 * @brief	Get the text section of the process
	 * 
	 * @return	A reference to the text section
	 */
	TextSection& getTextSection();
};