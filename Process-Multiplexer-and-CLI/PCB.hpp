#pragma once

// ----- << External Library >> ----- //
#include "Process.hpp"
#include <mutex>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a Process Control Block
 */
class PCB
{
public:
	/**
	 * @brief	Enumeration containing all the possible states a
	 *			process may have
	 */
	enum PROCESS_STATE
	{
		NEW,
		READY,
		RUNNING,
		WAITING,
		TERMINATED
	};

private:
	// Primitive Data Members
	unsigned long long processID;

	// User-defined Data Members
	size_t memoryRequired;
	size_t priority;
	size_t programCounter;

	std::string processLog = "Log:\n";
	std::string processName;

	PROCESS_STATE processState;

	// Member Objects

	Process process;
public:
	/**
	 * @brief	Class constructor
	 */
	PCB(
		unsigned long long ID,
		size_t heapSize,
		std::vector<std::unique_ptr<Instruction>> instructions,
		size_t priority,
		size_t memoryRequired
	);

	/**
	 * @brief	Class destructor
	 */
	~PCB();

	/**
	 * @brief	Appends a string to the log
	 * 
	 * @param	log --
	 *			The string that is going to be appended to the log
	 */
	void appendLog(std::string log);

	/**
	 * @brief	Get the log
	 * 
	 * @return	The log of the process
	 */
	std::string getLog() const;

	/**
	 * @brief	Get the required memory for the process
	 * 
	 * @return	The memory requirements of the process
	 */
	size_t getMemoryRequired() const;

	/**
	 * @brief	Get the name of the process
	 * 
	 * @return	Name of the process
	 */
	std::string getName() const;

	/**
	 * @brief	Get the priority of the process
	 * 
	 * @return	Priority of the process
	 */
	size_t getPriority() const;

	/**
	 * @brief	Get the process associated with the PCB
	 * 
	 * @return	A reference to the process
	 */
	Process& getProcess();

	/**
	 * @brief	Get the process ID of the process associated 
	 *			with the PCB
	 * 
	 * @return	Process ID of the process
	 */
	unsigned long long getProcessID() const;

	/**
	 * @brief	Get the program counter
	 * 
	 * @return	The program counter
	 */
	size_t getProgramCounter() const;

	/**
	 * @brief	Get the state of the process
	 * 
	 * @return	The state of the process associated with the PCB
	 */
	PROCESS_STATE getState() const;

	/**
	 * @brief	Increment the program counter
	 */
	void incrementProgramCounter();

	/**
	 * @brief	Set the processName field of the PCB to a string
	 * 
	 * @param	name --
	 *			The string that will be set as the name of the PCB
	 */
	void setName(std::string name);

	/**
	 * @brief	Set the state field of the PCB
	 * 
	 * @param	newState --
	 *			The new state that is going to be set
	 */
	void setState(PROCESS_STATE newState);
};