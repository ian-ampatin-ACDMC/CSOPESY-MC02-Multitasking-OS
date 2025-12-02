// ----- << External Library >> ----- //
#include "Core.hpp"
#include "Instruction_ADD.hpp"
#include "Instruction_DECLARE.hpp"
#include "Instruction_FOR.hpp"
#include "Instruction_PRINT.hpp"
#include "Instruction_READ.hpp"
#include "Instruction_SLEEP.hpp"
#include "Instruction_SUBTRACT.hpp"
#include "Instruction_WRITE.hpp"
#include <iomanip>
#include <sstream>

// TODO: Fix RR scheduler, context switch overhead is too large

// ----- << Private Member Function Implementation >> ----- //
std::string Core::convertDecToHex(size_t dec)
{
	std::ostringstream stream;

	stream << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << dec;

	return stream.str();
}

size_t Core::convertHexToDecimal(std::string hex)
{
	size_t result = 0;

	try
	{
		result = std::stoul(hex, nullptr, 16);
	}
	catch (const std::invalid_argument& e)
	{
		throw std::runtime_error("Invalid Hex String: " + hex);
	}
	catch (const std::out_of_range& e)
	{
		throw std::runtime_error("Hex String out of range: " + hex);
	}

	return result;
}

void Core::executeInstruction(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	INSTRUCTION_TYPE typeIdentifier = instruction->getInstructionType();

	// Include delay in executions
	GlobalClock::getInstance().waitForTick(actualDelay);

	// Instruction execution logic
	if (typeIdentifier == ADD)
	{
		execute_ADD(process, instruction);
	}
	else if (typeIdentifier == DECLARE)
	{
		execute_DECLARE(process, instruction);
	}
	else if (typeIdentifier == FOR)
	{
		execute_FOR(process, instruction);
	}
	else if (typeIdentifier == PRINT)
	{
		execute_PRINT(process, instruction);
	}
	else if (typeIdentifier == READ)
	{
		execute_READ(process, instruction);
	}
	else if (typeIdentifier == SLEEP)
	{
		execute_SLEEP(process, instruction);
	}
	else if (typeIdentifier == SUBTRACT)
	{
		execute_SUBTRACT(process, instruction);
	}
	else if (typeIdentifier == WRITE)
	{
		execute_WRITE(process, instruction);
	}

	{
		std::unique_lock<std::mutex> timingVectorLock(timingVectorMutex);

		if (timingVector.size() > window)
		{
			timingVector.erase(timingVector.begin());
		}

		timingVector.push_back(true);

		booleanVector.appendElement(coreID, true);
	}
}

void Core::executeProcess()
{
	std::shared_ptr<PCB> substitute;

	{
		std::unique_lock<std::mutex> readyLock(readyQueueMutex);
		if (readyQueue.empty())
		{
			return;
		}
		substitute = readyQueue.front();
		readyQueue.pop();
	}

	currentPCB = substitute;

	{
		std::unique_lock<std::mutex> memoryManagerLock(memoryManagerMutex);

		if (!memoryManager.loadProcess(currentPCB->getProcessID()))
		{
			{
				currentPCB->setState(PCB::PROCESS_STATE::READY);
				readyQueue.push(currentPCB);
			}

			currentPCB.reset();
			return;
		}
	}

	Process& process = currentPCB->getProcess();
	auto& instructions = process.getTextSection().getInstructionAll();

	currentPCB->setState(PCB::PROCESS_STATE::READY);

	for (auto& instruction : instructions)
	{
		if (!schedulerRunning)
		{
			break;
		}
		{
			std::unique_lock<std::mutex> instructionLock(instructionMutex);
			executeInstruction(process, instruction);
		}
		currentPCB->incrementProgramCounter();
	}

	{
		std::unique_lock<std::mutex> memoryMangerLock(memoryManagerMutex);
		memoryManager.remove(currentPCB->getProcessID());
	}
	currentPCB->setState(PCB::PROCESS_STATE::TERMINATED);
	currentPCB.reset();
}

void Core::executeProcessLimited(size_t programCounterIncrease)
{
	/*
	currentPCB = readyQueue.front();

	{
		std::unique_lock<std::mutex> memoryMangerLock(memoryManagerMutex);
		if (currentPCB->getMemoryRequired() > memoryManager.getAvailableMemory())
		{

			currentPCB.reset();
			return;
		}
	}

	readyQueue.pop();
	*/

	std::shared_ptr<PCB> substitute;

	{
		std::unique_lock<std::mutex> readyLock(readyQueueMutex);
		if (readyQueue.empty())
		{
			return;
		}
		substitute = readyQueue.front();
		readyQueue.pop();
	}

	currentPCB = substitute;

	{
		std::unique_lock<std::mutex> memoryManagerLock(memoryManagerMutex);
		

		if (!memoryManager.loadProcess(currentPCB->getProcessID()))
		{
			{
				currentPCB->setState(PCB::PROCESS_STATE::READY);
				readyQueue.push(currentPCB);
			}

			currentPCB.reset();
			return;
		}
	}


	Process& process = currentPCB->getProcess();
	auto& instructions = process.getTextSection().getInstructionAll();

	size_t startIndex = currentPCB->getProgramCounter();
	size_t endIndex = std::min(startIndex + programCounterIncrease, instructions.size());

	currentPCB->setState(PCB::PROCESS_STATE::READY);

	for (size_t i = startIndex; i < endIndex; i++)
	{
		if (!schedulerRunning)
		{
			break;
		}
		{
			std::unique_lock<std::mutex> instructionLock(instructionMutex);
			executeInstruction(process, instructions.at(i));
		}
		currentPCB->incrementProgramCounter();
	}

	if (currentPCB->getProgramCounter() >= instructions.size())
	{
		{
			std::unique_lock<std::mutex> memoryMangerLock(memoryManagerMutex);
			memoryManager.remove(currentPCB->getProcessID());
		}

		currentPCB->setState(PCB::PROCESS_STATE::TERMINATED);
		currentPCB.reset();
	}
	else
	{
		
		{
			currentPCB->setState(PCB::PROCESS_STATE::READY);
			readyQueue.push(currentPCB);
		}

		currentPCB.reset();
	}
}

void Core::fillDataSection(Process& process, std::string variableName)
{
	if (!process.getDataSection().contains(variableName))
	{
		process.getDataSection().addUninitializedVariable(variableName);
	}
}

size_t Core::fillLogicalDataSection(Process& process, std::string variableName)
{
	if (process.getLogicalDataSection().getVariableAddress(variableName))
	{
		return 0;
	}

	if (process.getLogicalDataSection().insertVariable(variableName))
	{
		return 2;
	}

	return 1;

}

void Core::schedulerFCFS()
{
	// Variables
	size_t startTime, endTime;
	while (schedulerRunning)
	{

		{
			std::unique_lock<std::mutex> readyLock(readyQueueMutex);
			if (readyQueue.empty())
			{
				GlobalClock::getInstance().waitForTick(10);
				{
					std::unique_lock<std::mutex> utilizationLock(utilizationMutex);
					idleTime++;
				}
				{
					std::unique_lock<std::mutex> timingVectorLock(timingVectorMutex);

					if (timingVector.size() > window)
					{
						timingVector.erase(timingVector.begin());
					}

					timingVector.push_back(false);

					booleanVector.appendElement(coreID, false);
				}
				continue;
			}
		}

		{
			std::unique_lock<std::mutex> busyLock(busyMutex);
			isBusy = true;
		}
		startTime = GlobalClock::getInstance().getTicks();
		executeProcess();
		endTime = GlobalClock::getInstance().getTicks();
		{
			std::unique_lock<std::mutex> busyLock(busyMutex);
			isBusy = false;
		}

		{
			std::unique_lock<std::mutex> utilizationLock(utilizationMutex);
			busyTime += (endTime - startTime);
		}
	}
}

void Core::schedulerRR()
{
	size_t startTime, endTime;
	while (schedulerRunning)
	{
		{
			std::unique_lock<std::mutex> readyLock(readyQueueMutex);
			if (readyQueue.empty())
			{
				GlobalClock::getInstance().waitForTick();
				{
					std::unique_lock<std::mutex> utilizationLock(utilizationMutex);
					idleTime++;
				}
				{
					std::unique_lock<std::mutex> timingVectorLock(timingVectorMutex);

					if (timingVector.size() > window)
					{
						timingVector.erase(timingVector.begin());
					}

					timingVector.push_back(false);

					booleanVector.appendElement(coreID, false);
				}
				continue;
			}
		}

		{
			std::unique_lock<std::mutex> busyLock(busyMutex);
			isBusy = true;
		}
		startTime = GlobalClock::getInstance().getTicks();
		executeProcessLimited(quantumCycle);
		endTime = GlobalClock::getInstance().getTicks();
		{
			std::unique_lock<std::mutex> busyLock(busyMutex);
			isBusy = false;
		}

		{
			std::unique_lock<std::mutex> utilizationLock(utilizationMutex);
			busyTime += (endTime - startTime);
		}
	}
}

// ----- << Public Member Function Implementation >> ----- //
Core::Core(std::string algorithm, size_t coreID, size_t delayPerExecution, size_t quantum, MMU& memoryManager, GrowingBooleanVector& booleanVector)
	: delayPerExecution(delayPerExecution), coreID(coreID), quantumCycle(quantum), isBusy(false), schedulerRunning(true),
	memoryManager(memoryManager), booleanVector(booleanVector), busyTime(0), idleTime(0), window(10)
{
	actualDelay = delayPerExecution + 1;

	if (algorithm == "FCFS")
	{
		schedulerThread = std::thread(&Core::schedulerFCFS, this);
	}
	else if (algorithm == "RR")
	{
		schedulerThread = std::thread(&Core::schedulerRR, this);
	}
	else
	{
		throw std::runtime_error("Scheduling Algorithm not recognized.");
	}

	std::vector<bool> vect;
	booleanVector.addVector(vect);
}

Core::~Core() = default;

// TODO: Double check page creation -> Monitor backing-store.txt
void Core::assign(std::shared_ptr<PCB> processControlBlock)
{
	std::unique_lock<std::mutex> lock(addProcessMutex);
	processControlBlock->setState(PCB::PROCESS_STATE::READY);

	{
		std::unique_lock<std::mutex> memoryManagerLock(memoryManagerMutex);
		memoryManager.createPages(processControlBlock->getProcessID(), processControlBlock->getMemoryRequired());
	}

	readyQueue.push(processControlBlock);
}

std::pair<unsigned long long, std::optional<std::string>> Core::getCurrentID()
{
	std::unique_lock<std::mutex> currentLock(currentMutex);
	if (currentPCB)
	{
		return { currentPCB->getProcessID(), currentPCB->getName() };
	}

	return { -1 , std::nullopt };
}

double Core::getUtilization()
{
	/*
	if (busyTime + idleTime == 0)
	{
		return 0.0;
	}
	return (static_cast<double>(busyTime) / static_cast<double>(busyTime + idleTime)) * 100.0;
	*/
	double busyStates = 0.0;
	double idleStates = 0.0;

	{
		std::unique_lock<std::mutex> timingVectorLock(timingVectorMutex);

		for (bool state : timingVector)
		{
			if (state)
			{
				busyStates += 1.0;
			}
			else
			{
				idleStates += 1.0;
			}
		}
	}

	if (busyStates == 0.0)
	{
		return 0.0;
	}

	return (busyStates / (busyStates + idleStates)) * 100.0;
}

bool Core::isCoreBusy() const
{
	return isBusy;
}

void Core::joinSchedulerThread()
{
	if (schedulerThread.joinable())
	{
		schedulerThread.join();
	}
}

void Core::shutdown()
{
	schedulerRunning = false;
}

// ----- << Protecte Member Function Implementation >> ----- //
bool Core::execute_ADD(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_ADD*>(instruction.get()))
	{
		// Check destination
		if (fillLogicalDataSection(process, INST->getDestination()) == 1)
		{
			std::cerr << "Error: Symbol table is full and destination variable cannot be inserted\n";
			return false;
		}

		PARAMETER_COMBINATION_VLM combination = INST->getInstructionCombination();

		if (combination == VARIABLE)
		{
			if (fillLogicalDataSection(process, INST->getVariableFirst()) == 1)
			{
				std::cerr << "Error: Symbol table is full and first variable cannot be inserted\n";
				return false;
			}

			if (fillLogicalDataSection(process, INST->getVariableSecond()) == 1)
			{
				std::cerr << "Error: Symbol table is full and second variable cannot be inserted\n";
				return false;
			}

			auto first		= process.getLogicalDataSection().getData(INST->getVariableFirst());
			auto second		= process.getLogicalDataSection().getData(INST->getVariableSecond());

			if (first && second)
			{
				if (process.getLogicalDataSection().setValue(INST->getDestination(), *first + *second))
				{
					return true;
				}
				else
				{
					std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
					return false;
				}
			}
			else
			{
				std::cerr << "Error: Unable to retrieve data from symbol table\n";
				return false;
			}
		}
		else if (combination == LITERAL)
		{
			uint16_t first		= static_cast<uint16_t>(INST->getLiteralFirst());
			uint16_t second		= static_cast<uint16_t>(INST->getLiteralSecond());

			if (process.getLogicalDataSection().setValue(INST->getDestination(), first + second))
			{
				return true;
			}
			else
			{
				std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
				return false;
			}
		}
		else if (combination == MIXED)
		{
			if (fillLogicalDataSection(process, INST->getVariableFirst()) == 1)
			{
				std::cerr << "Error: Symbol table is full and first variable cannot be inserted\n";
				return false;
			}

			auto first = process.getLogicalDataSection().getData(INST->getVariableFirst());
			uint16_t second = static_cast<uint16_t>(INST->getLiteralSecond());

			if (first)
			{
				if (process.getLogicalDataSection().setValue(INST->getDestination(), *first + second))
				{
					return true;
				}
				else
				{
					std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
					return false;
				}
			}
			else
			{
				std::cerr << "Error: Unable to retrieve data from symbol table\n";
				return false;
			}
		}
	}

	std::cerr << "Error: Unsuccessful cast for ADD\n";
	return false;
}

bool Core::execute_DECLARE(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_DECLARE*>(instruction.get()))
	{
		if (!process.getLogicalDataSection().getVariableAddress(INST->getName()))
		{
			bool insertStatus = process.getLogicalDataSection().insertVariable(INST->getName());

			if (!insertStatus)
			{
				std::cerr << "Error: Failure to insert variable\n";
				return false;
			}

			if (INST->variableInitialized())
			{
				if (process.getLogicalDataSection().setValue(INST->getName(), INST->getData()))
				{
					//process.getLogicalDataSection().print();
					return true;
				}
				else
				{
					std::cerr << "Error: Setting variable value unsuccessful\n";
				}
			}
			else
			{
				if (process.getLogicalDataSection().setValue(INST->getName(), static_cast<uint16_t>(0)))
				{
					//process.getLogicalDataSection().print();
					return true;
				}
				else
				{
					std::cerr << "Error: Setting variable value unsuccessful\n";
				}
			}
		}
		else
		{
			return false;
		}
	}

	std::cerr << "Error: Unsuccessful cast for ADD\n";
	return false;
}

bool Core::execute_FOR(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_FOR*>(instruction.get()))
	{
		for (size_t i = 0; i < INST->getRepetitions(); i++)
		{
			for (auto& instruction : INST->getInstructions())
			{
				executeInstruction(process, instruction);
			}
		}

		return true;
	}

	std::cerr << "Error: Unsuccessful cast for FOR instruction\n";
	return false;
}

bool Core::execute_PRINT(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_PRINT*>(instruction.get()))
	{
		if (INST->containsVariable())
		{
			auto dataString = process.getLogicalDataSection().getData(INST->getVariableName());

			if (dataString)
			{
				currentPCB->appendLog(INST->getMessage());
				currentPCB->appendLog(std::to_string(*dataString));

				// Debugging
				//std::cout << INST->getMessage() << *dataString << "\n";

				return true;
			}
			else
			{
				std::cerr << "Error: Trying to print variable that is not in symbol table\n";
				return false;
			}
		}
		else
		{
			currentPCB->appendLog(INST->getMessage());

			// Debugging
			//std::cout << INST->getMessage() << "\n";

			return true;
		}
	}

	std::cerr << "Error: Unsuccessful cast for PRINT\n";
	throw std::runtime_error("Error: Unsuccessful cast for PRINT\n");
	return false;
}

bool Core::execute_READ(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_READ*>(instruction.get()))
	{
		std::optional<std::string> read;
		{
			std::unique_lock<std::mutex> memoryManagerLock(memoryManagerMutex);
			read = memoryManager.protectedRead(currentPCB->getProcessID(), INST->getAddress(), 2);	
		}

		if (read)
		{
			if (process.getLogicalDataSection().setValue(INST->getDestination(), static_cast<uint16_t>(convertHexToDecimal(*read))))
			{
				// Debugging
				//process.getLogicalDataSection().print();

				return true;
			}
			else
			{
				//std::cerr << "Error: Setting variable value in symbol table for READ unsuccessful\n";
				return false;
			}
		}
		else
		{
			//std::cerr << "Error: Unsuccessful READ in MMU\n";
			return false;
		}
	}

	std::cerr << "Error: Unsuccessful cast for READ\n";
	return false;
}

bool Core::execute_SLEEP(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_SLEEP*>(instruction.get()))
	{
		GlobalClock::getInstance().waitForTick(INST->getSleepDuration());
		idleTime += INST->getSleepDuration();

		{
			std::unique_lock<std::mutex> timingVectorLock(timingVectorMutex);
			for (uint8_t i = 0; i < INST->getSleepDuration(); i++)
			{
				if (timingVector.size() > window)
				{
					timingVector.erase(timingVector.begin());
				}

				timingVector.push_back(false);

				booleanVector.appendElement(coreID, false);
			}
		}

		{
			std::unique_lock<std::mutex> utilizationLock(utilizationMutex);
			idleTime += INST->getSleepDuration();
		}

		return true;
	}

	std::cerr << "Error: Unsuccessful cast for SLEEP\n";
	return false;
}

bool Core::execute_SUBTRACT(Process& process, const std::unique_ptr<Instruction>& instruction)
{
	if (auto INST = dynamic_cast<Instruction_SUBTRACT*>(instruction.get()))
	{
		// Check destination
		if (fillLogicalDataSection(process, INST->getDestination()) == 1)
		{
			std::cerr << "Error: Symbol table is full and destination variable cannot be inserted\n";
			return false;
		}

		PARAMETER_COMBINATION_VLM combination = INST->getInstructionCombination();

		if (combination == VARIABLE)
		{
			if (fillLogicalDataSection(process, INST->getVariableFirst()) == 1)
			{
				std::cerr << "Error: Symbol table is full and first variable cannot be inserted\n";
				return false;
			}

			if (fillLogicalDataSection(process, INST->getVariableSecond()) == 1)
			{
				std::cerr << "Error: Symbol table is full and second variable cannot be inserted\n";
				return false;
			}

			auto first = process.getLogicalDataSection().getData(INST->getVariableFirst());
			auto second = process.getLogicalDataSection().getData(INST->getVariableSecond());

			if (first && second)
			{
				if (process.getLogicalDataSection().setValue(INST->getDestination(), *first - *second))
				{
					return true;
				}
				else
				{
					std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
					return false;
				}
			}
			else
			{
				std::cerr << "Error: Unable to retrieve data from symbol table\n";
				return false;
			}
		}
		else if (combination == LITERAL)
		{
			uint16_t first = static_cast<uint16_t>(INST->getLiteralFirst());
			uint16_t second = static_cast<uint16_t>(INST->getLiteralSecond());

			if (process.getLogicalDataSection().setValue(INST->getDestination(), first - second))
			{
				return true;
			}
			else
			{
				std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
				return false;
			}
		}
		else if (combination == MIXED)
		{
			if (fillLogicalDataSection(process, INST->getVariableFirst()) == 1)
			{
				std::cerr << "Error: Symbol table is full and first variable cannot be inserted\n";
				return false;
			}

			auto first = process.getLogicalDataSection().getData(INST->getVariableFirst());
			uint16_t second = static_cast<uint16_t>(INST->getLiteralSecond());

			if (first)
			{
				if (process.getLogicalDataSection().setValue(INST->getDestination(), *first - second))
				{
					return true;
				}
				else
				{
					std::cerr << "Error: Unsuccessful in setting variable value for ADD\n";
					return false;
				}
			}
			else
			{
				std::cerr << "Error: Unable to retrieve data from symbol table\n";
				return false;
			}
		}
	}

	std::cerr << "Error: Unsuccessful cast for ADD\n";
	return false;
}

bool Core::execute_WRITE(Process& process, const std::unique_ptr<Instruction>& instruction)
{

	if (auto INST = dynamic_cast<Instruction_WRITE*>(instruction.get()))
	{
		bool write;
		
		{
			std::unique_lock<std::mutex> memoryManagerLock(memoryManagerMutex);
			write = memoryManager.protectedWrite(currentPCB->getProcessID(), INST->getAddress(), convertDecToHex(INST->getData()));
		}

		if (write)
		{
			//std::cout << "Notice: WRITE operation successful\n";
			//std::cout << "        Writing " << INST->getData() << " to " << INST->getAddress() << "\n";
			return true;
		}
		else
		{
			//std::cerr << "Error: Unsuccessful WRITE in MMU\n";
			return false;
		}
	}

	std::cerr << "Error: Unsuccessful cast for READ\n";
	return false;
}





		/*
		auto ITE = dynamic_cast<Instruction_ADD*>(instruction.get());

		fillDataSection(process, ITE->getDestination());

		if (ITE->getInstructionCombination() == VARIABLE)
		{
			fillDataSection(process, ITE->getVariableFirst());
			fillDataSection(process, ITE->getVariableSecond());

			process.getDataSection().updateVariable(ITE->getDestination(),
				process.getDataSection().getData(ITE->getVariableFirst()) +
				process.getDataSection().getData(ITE->getVariableSecond()));
		}
		else if (ITE->getInstructionCombination() == LITERAL)
		{
			process.getDataSection().updateVariable(ITE->getDestination(),
				ITE->getLiteralFirst() + ITE->getLiteralSecond());
		}
		else
		{
			// Mixed -> Variable + Literal Combination
			fillDataSection(process, ITE->getVariableFirst());

			process.getDataSection().updateVariable(ITE->getDestination(),
				process.getDataSection().getData(ITE->getVariableFirst()) +
				ITE->getLiteralSecond());
		}

		auto ITE = dynamic_cast<Instruction_DECLARE*>(instruction.get());

		if (ITE->variableInitialized())
		{
			process.getDataSection().addInitializedVariable(ITE->getName(), ITE->getData());
		}
		else
		{
			process.getDataSection().addUninitializedVariable(ITE->getName());
		}

		// Debugging
		//std::cout << "EXECUTING DECLARE\n";
		
		auto ITE = dynamic_cast<Instruction_PRINT*>(instruction.get());

		std::string instructionLog = "PRINT INSTRUCTION |  ";

		if (ITE->containsVariable())
		{
			instructionLog += ITE->getMessage() + std::to_string(process.getDataSection().getData(ITE->getVariableName()));
		}
		else
		{
			instructionLog += ITE->getMessage();
		}

		currentPCB->appendLog(instructionLog);

		// Debugging
		//std::cout << "EXECUTING PRINT\n";

		auto ITE = dynamic_cast<Instruction_SLEEP*>(instruction.get());
		GlobalClock::getInstance().waitForTick(ITE->getSleepDuration());
		idleTime += ITE->getSleepDuration();
		
		auto ITE = dynamic_cast<Instruction_SUBTRACT*>(instruction.get());

		fillDataSection(process, ITE->getDestination());

		if (ITE->getInstructionCombination() == VARIABLE)
		{
			fillDataSection(process, ITE->getVariableFirst());
			fillDataSection(process, ITE->getVariableSecond());

			process.getDataSection().updateVariable(ITE->getDestination(),
				process.getDataSection().getData(ITE->getVariableFirst()) -
				process.getDataSection().getData(ITE->getVariableSecond()));
		}
		else if (ITE->getInstructionCombination() == LITERAL)
		{
			process.getDataSection().updateVariable(ITE->getDestination(),
				ITE->getLiteralFirst() - ITE->getLiteralSecond());
		}
		else
		{
			// Mixed -> Variable + Literal Combination
			fillDataSection(process, ITE->getVariableFirst());

			process.getDataSection().updateVariable(ITE->getDestination(),
				process.getDataSection().getData(ITE->getVariableFirst()) -
				ITE->getLiteralSecond());
		}
		*/