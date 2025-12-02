#pragma once

// ----- << External Library >> ----- //
#include "Core.hpp"
#include <iostream>
#include <vector>

// ----- << Class >> ----- //
class CPU
{
private:
	// User-defined Data Members
	CONFIGURATION config;

	std::mutex coreMutex;
	std::mutex masterListMutex;
	std::mutex memoryMangerMutex;
	std::mutex processCountMutex;

	std::thread processGeneratorThread;
	
	// Primitive Data Members
	bool breaker = false;
	bool generatorRunning;
	unsigned long long processesGenerated;

	// Member Objects
	std::vector<std::unique_ptr<Core>> cores;
	std::vector<std::shared_ptr<PCB>> masterListPCB;

	GrowingBooleanVector booleanVector;
	MMU memoryManager;

	/**
	 * @brief	Generates one single process and assigns it to
	 *			a Core
	 *
	 * @param	name --
	 *			The string that will be set as the name of the
	 *			process
	 *
	 * @param	heapSize --
	 *			The size of the process heap
	 *
	 * @param	instructions --
	 *			The instructions that make up the process
	 *
	 * @param	priority --
	 *			The priority level of the process
	 * 
	 * @param	memoryRequired --
	 *			The amount of memory the process requires to 
	 *			execute
	 */
	void createProcess(
		std::string name,
		size_t heapSize,
		std::vector<std::unique_ptr<Instruction>>&& instructions,
		size_t priority,
		size_t memoryRequired
	);

	/**
	 * @brief	Generates one single process and assigns it to 
	 *			a core
	 * 
	 *			WARNING: generatorFunction use only
	 *
	 * @param	ID --
	 *			The unique identification number assigned to
	 *			a process
	 *
	 * @param	heapSize --
	 *			The size of the process heap
	 *
	 * @param	instructions --
	 *			The instructions that make up the process
	 *
	 * @param	priority --
	 *			The priority level of the process
	 * 
	 * @param	memoryRequired --
	 *			The amount of memory the process requires to 
	 *			execute
	 */
	void createProcess_Generator(
		unsigned long long ID,
		size_t heapSize,
		std::vector<std::unique_ptr<Instruction>>&& instructions,
		size_t priority,
		size_t memoryRequired
	);

	/**
	 * @brief	Continuously assign instructions to the cores
	 */
	void generatorFunction();

	/**
	 * @brief	Generate a random number
	 * 
	 * @param	minimum --
	 *			Lowest possible generated value
	 * 
	 * @param	maximum --
	 *			Highest possible generated value
	 * 
	 * @return	A number between minimum and maximum inclusive
	 */
	size_t generateRandomNumber(size_t minimum, size_t maximum);

public:
	/**
	 * @brief	Parses instructions
	 * 
	 * @return	A vector containing all of the parsed instructions
	 */
	std::vector<std::vector<std::string>> instructionTokenizer(
		std::string rawInstructionString
	);

private:

	/**
	 * @brief	Checks if a string only contians numberic characters
	 *			[0 1 2 3 4 5 6 7 8 9]
	 * 
	 * @param	string --
	 *			The string being checked
	 * 
	 * @return	true if the string only contains numberic
	 *			characters. Otherwise, false.
	 */
	bool isAllDigits(const std::string& string);

	/**
	 * @brief	Checks if a string only contains valid hexadecimal
	 *			characters [0 1 2 3 4 5 6 7 8 9 A B C D E F]
	 * 
	 * @param	string--
	 *			The string being checked
	 * 
	 * @return	true if the string only contains heaxedecimal
	 *			characters. Otherwise, false.
	 */
	bool isHexString(const std::string& string);

	/**
	 * @brief	Checks the first character of a string and if it
	 *			contains any spaces
	 * 
	 * @param	string --
	 *			The string being checked
	 * 
	 * @return	true if the first character is an alphabet AND if
	 *			the string cointains no spaces. Otherwise, false
	 */
	bool isValidIdentifier(const std::string& string);

	/**
	 * @brief	Reads the config.txt
	 * 
	 * @return	A CONFIGURATION that has the values read from the
	 *			config text file
	 */
	CONFIGURATION readConfig();

	/**
	 * @brief	Tokenizes a string
	 * 
	 * @param	line --
	 *			The string that is being tokenized
	 * 
	 * @param	delimeter -- 
	 *			The character the separates individual tokens
	 * 
	 * @return	A vector of strings that contains all of the 
	 *			individual tokens
	 */
	std::vector<std::string> tokenize(
		std::string line, 
		char delimeter
	);
public:
	/**
	 * @brief	Class constructor
	 */
	CPU();

	/**
	 * @brief	Class destructor
	 */
	~CPU();

	/**
	 * @brief	Checks if the process is terminated
	 *
	 * @param	name --
	 *			Name of the process that is going to be checked
	 *
	 * @return	True if the CPU contains the process.
	 *			Otherwise, false.
	 */
	bool isProcessContained(std::string name);

	/**
	 * @brief	Checks if the process is terminated
	 * 
	 * @param	name --
	 *			Name of the process that is going to be checked
	 * 
	 * @return	True if the process is still running. 
	 *			Otherwise, false.
	 */
	bool isProcessRunning(std::string name);

	/**
	 * @brief	Generate a process and assigns it to 
	 *			a core
	 * 
	 * @param	name --
	 *			Name given to the generated process
	 * 
	 * @param	memoryRequired --
	 *			The amount of memory a process requires to
	 *			execute
	 */
	void generateProcess(
		std::string name, 
		size_t memoryRequired
	);

	/**
	 * @brief	Generate a process and assigns it to
	 *			a core
	 *
	 * @param	name --
	 *			Name given to the generated process
	 *
	 * @param	memoryRequired --
	 *			The amount of memory a process requires to
	 *			execute
	 * 
	 * @param	unparsedInstruction --
	 *			The instructions that the process will execute.
	 *			However, it is in its raw unparsed string form
	 */
	void generateProcess(
		std::string name,
		size_t memoryRequired,
		std::string unparsedInstructions
	);

	/**
	 * @beief	Generate random instructions
	 * 
	 * @param	count --
	 *			The number of instructions to be generated
	 * 
	 * @return	A vector containing all of the generated
	 *			instructions
	 */
	std::vector<std::unique_ptr<Instruction>>
		generateRandomInstructions(size_t count);

	/**
	 * @brief	Get the average utilization of all of the Cores
	 *			in the CPU
	 * 
	 * @return	Average utilization as a percentage
	 */
	double getAverageUtilization();

	/**
	 * @brief
	 */
	std::vector<std::unique_ptr<Instruction>>
		instructionInterpreter(std::string rawInstructionString);

	/**
	 * @brief	Print process log
	 *
	 * @param	name --
	 *			Name of the process whose log will be printed
	 */
	void printProcessLog(std::string name);

	/**
	 * @brief	Prints the status of the processes
	 */
	void printProcessStatus();

	/**
	 * @brief	Splits a string into substrings
	 * 
	 * @param	raw --
	 *			The string that is going to be split
	 * 
	 * @param	delimiter --
	 *			The character that indicates where the string will
	 *			be split
	 * 
	 * @return	Substrings of the string
	 */
	std::vector<std::string> split(std::string raw, char delimiter);

	/**
	 * @brief	Start generating random processes
	 */
	void startGenerator();

	/**
	 * @brief	Stop generating random processes
	 */
	void stopGenerator();

	/**
	 * @brief	Provide a detailed view of the memory
	 */
	void visualizeMemoryDetailed();

	/**
	 * @brief	Provide a summarized view of the memory usage
	 *			and core utilization of the CPU
	 */
	void visualizeMemorySummary();

	/**
	 * @brief	Write the report in the report file
	 */
	void writeReport();

	/**
	 * @brief	Prints out the contents of the config.text file
	 */
	void printConfig() const;

protected: 
	// Mostly helper functions to assists in debugging
	// Move to public to use these member functions
	/**
	 * @brief	CPU will wait until an N number of ticks have passed
	 *			and prints the number of ticks before execution is 
	 *			returned to the thread that called this function
	 * 
	 * @param	numTicks --
	 *			Is the number of ticks that must pass before 
	 *			execution can return to the original thread
	 *			the function was called on
	 */
	void testClock(size_t numTicks)
	{
		GlobalClock::getInstance().waitForTick(10);
		std::cout << GlobalClock::getInstance().getTicks() 
			<< std::endl;
	}

	/**
	 * @brief	Interprets an ADD instruction given a set of tokens
	 * 
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 * 
	 * @return	A completed ADD instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretADD(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an DECLARE instruction given a set of 
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed DECLARE instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretDECLARE(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an FOR instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed FOR instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretFOR(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an PRINT instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed PRINT instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretPRINT(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an READ instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed READ instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretREAD(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an SLEEP instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed SLEEP instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretSLEEP(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an SUBTRACT instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed SUBTRACT instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretSUBTRACT(
		std::vector<std::string> instructionTokens
	);

	/**
	 * @brief	Interprets an WRITE instruction given a set of
	 *			tokens
	 *
	 * @param	instructionTokens --
	 *			A vector containing the tokens to form the
	 *			instruction
	 *
	 * @return	A completed WRITE instruction
	 */
	std::optional<std::unique_ptr<Instruction>> interpretWRITE(
		std::vector<std::string> instructionTokens
	);
};