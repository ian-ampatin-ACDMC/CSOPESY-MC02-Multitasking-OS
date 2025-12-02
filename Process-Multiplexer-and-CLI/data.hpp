#pragma once

// ----- << External Library >> ----- //
#include <string>

// ----- << Structures >> ----- //

/**
 * @brief	Structure that contains all the default parameters
 *			of the system
 *			
 */

struct CONFIGURATION
{
	// Process Scheduler Fields
	size_t batchProcessFrequency = 1;
	size_t coreCount = 1;
	size_t delayPerInstructionExecution = 0;
	size_t maximumInstructions = 1;
	size_t minimumInstructions = 1;
	size_t quantumCycle = 1;

	std::string schedulerAlgorithm = "FCFS";

	// Memory Manager Fields
	size_t maximumMemoryPerProcess = 6;
	size_t maximumOverallMemory = 6;
	size_t memoryPerFrame = 6;
	size_t minimumMemoryPerProcess = 6;
};

// ----- << Enumerations >> ----- //
/** 
 * @brief	Enumeration containing all the possible instructions
 *			the system can process/generate
 */
enum INSTRUCTION_TYPE 
{
	_UNSET,
	ADD,
	DECLARE,
	FOR,
	PRINT,
	READ,
	SLEEP,
	SUBTRACT,
	WRITE
};

/**
 * @brief	Enumeration containing the possible combination of 
 *			parameters 
 * 
 *			Possible Combinations:
 * 
 *			VARIABLE (string and string)
 * 
 *			LITERAL (literal and literal)
 *		
 *			MIXED (string and literal) or (literal and string)
 */
enum PARAMETER_COMBINATION_VLM
{
	VARIABLE,
	LITERAL,
	MIXED
};