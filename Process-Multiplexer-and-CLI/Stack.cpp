// ----- << External Libraries >> ---- //
#include "Stack.hpp"
#include <stdexcept>

// ----- << Implementation >> ----- //
Stack::Stack() = default;

Stack::~Stack() = default;

void Stack::addLocalVariable(uint16_t variable)
{
	if (stack.empty())
	{
		throw std::runtime_error("Stack is empty");
	}

	stack.back().localVariables.push_back(variable);
}

bool Stack::isEmpty()
{
	return stack.empty();
}

const STACK_FRAME& Stack::getFrame()
{
	if (stack.empty())
	{
		throw std::runtime_error("Stack is empty");
	}

	return stack.back();
}

void Stack::popFrame()
{
	if (stack.empty())
	{
		throw std::runtime_error("Trying to pop an empty stack");
	}

	stack.pop_back();
}

void Stack::pushFrame(std::string functionName, std::vector<uint16_t> parameters, uint16_t returnAddress)
{
	STACK_FRAME frame;

	frame.functionName = functionName;
	frame.parameters = parameters;
	frame.returnAddress = returnAddress;

	stack.push_back(frame);
}