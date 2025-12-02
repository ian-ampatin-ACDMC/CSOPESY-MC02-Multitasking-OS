#pragma once

// ----- << External Library ----- //
#include "GlobalClock.hpp"
#include "GrowingBooleanVector.hpp"
#include "MMU.hpp"
#include "PCB.hpp"
#include "Process.hpp"

#include <mutex>
#include <queue>
#include <thread>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a CPU core
 */
class Core
{
private:
	// User-defined Data Members
	size_t actualDelay;
	size_t busyTime;
	size_t coreID;
	size_t delayPerExecution;
	size_t idleTime;
	size_t quantumCycle;
	size_t window;

	std::mutex addProcessMutex;
	std::mutex busyMutex;
	std::mutex currentMutex;
	std::mutex instructionMutex;
	std::mutex memoryManagerMutex;
	std::mutex readyQueueMutex;
	std::mutex schedulerMutex;
	std::mutex timingVectorMutex;
	std::mutex updateFrameMutex;
	std::mutex utilizationMutex;
	std::mutex writeInstructionMutex;

	std::thread schedulerThread;

	std::vector<bool> timingVector;	// Grows indefinitely
	
	// Primitive Data Member
	bool isBusy;
	bool schedulerRunning;

	// Object Members
	std::queue<std::shared_ptr<PCB>> readyQueue;
	std::shared_ptr<PCB> currentPCB = nullptr;

	GrowingBooleanVector& booleanVector;
	MMU& memoryManager;
	
	/**
	 * @brief	Conversts a decimal number to a hexadecimal string
	 *
	 * @param	dec --
	 *			The number that is being converted
	 *
	 * @return	An equivalent hexadecimal string representation of
	 *			the decimal number
	 */
	std::string convertDecToHex(size_t dec);

	/**
	 * @brief	Converts a string of hexadecmial numbers to a
	 *			decimal number
	 *
	 * @param	hex --
	 *			The string of hexadecimal characters is being
	 *			converted
	 *
	 * @return	Decimal equivalent of the hexadecimal string
	 */
	size_t convertHexToDecimal(std::string hex);

	/**
	 * @brief	Executes an instruction from the process 
	 */
	void executeInstruction(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Executes all of the process in the process
	 *			associated with currentPCB
	 */
	void executeProcess();

	/**
	 * @brief	Executes a limited number of instructions in
	 *			the process
	 *
	 * @param	programCounterIncrease --
	 *			The number of instructions that is going to be
	 *			executed
	 */
	 void executeProcessLimited(size_t programCounterIncrease);

	/**
	 * @brief	If a variable is not contained in the data section,
	 *			add it as an uninitialized variable
	 * 
	 * @param	variableName --
	 *			Name of the variable that is being added if the
	 *			data section does not contain it
	 */
	void fillDataSection(
		Process& process,
		std::string variableName
	);

	/**
	 * @brief	If a variable is not contained in the logical data 
	 *			section, add it as an uninitialized variable
	 *
	 * @param	variableName --
	 *			Name of the variable that is being added if the
	 *			data section does not contain it
	 * 
	 * @return	0 == variable already in logical data section
	 *			
	 *			1 == variable is not in logical data section but
	 *			it is already full
	 * 
	 *			2 == variable is not in logical data section but
	 *			insertion is successful
	 */
	size_t fillLogicalDataSection(
		Process& process,
		std::string variableName
	);

	/**
	 * @brief	Schedules processes based on the
	 *			First-Come-First-Serve scheduling algorithm
	 */
	void schedulerFCFS();

	/**
	 * @brief	Sceduler processes based on the
	 *			Round Robing scheduling algorithm
	 */
	void schedulerRR();

public:
	/**
	 * @brief	Class constructor
	 */
	Core(
		std::string algorithm, 
		size_t coreID,
		size_t delayPerExecution, 
		size_t quantum, 
		MMU& memoryManager,
		GrowingBooleanVector& booleanVector
	);

	/**
	 * @brief	Class destructor
	 */
	~Core();

	/**
	 * @brief	Adds a PCB to the queue
	 * 
	 * @param	processControlBlock --
	 *			The PCB that is going to be added to the queue
	 */
	void assign(std::shared_ptr<PCB> processControlBlock);

	/**
	 * @brief	Get the processID of the process the core is
	 *			currently executing
	 * 
	 * @return	ID of the process currently being executed.
	 *			Otherwise, if the core is not currently executing
	 *			a process, it will return -1
	 */
	std::pair<unsigned long long, std::optional<std::string>> 
		getCurrentID();

	/**
	 * @brief	Get the utilization of the Core
	 *
	 * @return	The utilization as a percentage
	 */
	double getUtilization();

	/**
	 * @brief	Checks if the core is busy
	 * 
	 * @return	True if the core is busy. Otherwise, false.
	 */
	bool isCoreBusy() const;

	/**
	 * @brief	Joins the scheduler thread
	 */
	void joinSchedulerThread();

	/**
	 * @brief	Shutdowns the scheduler thread
	 */
	void shutdown();

protected:
	/**
	 * @brief	Attempts to update the logical data section
	 *			of a process
	 */

	/**
	 * @brief	Execute the ADD instruction
	 * 
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 * 
	 * @return	true if instruction is successfully executed
	 */
	bool execute_ADD(
		Process& process, 
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the DECLARE instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_DECLARE(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the FOR instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_FOR(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the PRINT instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_PRINT(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the READ instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_READ(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the SLEEP instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_SLEEP(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the SUBTRACT instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_SUBTRACT(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);

	/**
	 * @brief	Execute the WRITE instruction
	 *
	 *			WARNING: Enusure that caller establishes a
	 *			lock when calling the function
	 *
	 * @return	true if instruction is successfully executed
	 */
	bool execute_WRITE(
		Process& process,
		const std::unique_ptr<Instruction>& instruction
	);
};