#pragma once

// ----- << External Library >> ----- //
#include <string>
#include <vector>

// ----- << Stack Frame >> ----- //
struct STACK_FRAME
{
	std::string functionName;
	std::vector<uint16_t> parameters;
	std::vector<uint16_t> localVariables;

	uint16_t returnAddress = 0;
};

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a stack, a
 *			last-in-first-out data structure
 */
class Stack
{
private:
	std::vector<STACK_FRAME> stack;

public:
	/**
	 * @brief	Class constructor
	 */
	Stack();

	/**
	 * @brief	Class destructor
	 */
	~Stack();

	/**
	 * @brief	Adds a local variable to the current STACK_FRAME
	 * 
	 * @param	localVariable --
	 *			The variable that is going to be added to the stack
	 *			frame
	 */
	void addLocalVariable(uint16_t variable);

	/**
	 * @brief	Checks if the stack is empty
	 * 
	 * @return	True if the stack is empty. Otherwise, false
	 */
	bool isEmpty();

	/**
	 * @brief	Get the current STACK_FRAME in the stack
	 * 
	 * @return	A reference to the STACK_FRAME
	 */
	const STACK_FRAME& getFrame();

	/**
	 * @brief	Pops a STACK_FRAME from the stack
	 */
	void popFrame();

	/**
	 * @brief	Push a STACK_FRAME into the stack
	 * 
	 * @param	functionName --
	 *			Name of the function that's is being pushed into the
	 *			stack
	 * 
	 * @param	parameters --
	 *			A vector that contains all of the parameters needed
	 *			to execute a function
	 * 
	 * @param	returnAddress --
	 *			Location where the program returns after the called
	 *			function finishes
	 */
	void pushFrame(
		std::string functionName,
		std::vector<uint16_t> parameters,
		uint16_t returnAddress
	);
};