/****************************************************************************************************
 * 
 * For instruction token issues: see instructionTokenizer and split
 * 
 * NOTE:	Nested For Loop still does not have a limit implemented (Theoretically, an infinite 
 *			number of nested For Loops can exist.
 *			
 * NOTE:	Due to the way the instructionTokenizer(...) function is implemented, it can not properly
 *			parse nested FOR instructions.
 ****************************************************************************************************/


// ----- << External Library >> ----- //
#include "CPU.hpp"
#include "Instruction_ADD.hpp"
#include "Instruction_DECLARE.hpp"
#include "Instruction_FOR.hpp"
#include "Instruction_PRINT.hpp"
#include "Instruction_READ.hpp"
#include "Instruction_SLEEP.hpp"
#include "Instruction_SUBTRACT.hpp"
#include "Instruction_WRITE.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>

// ----- << Randomizer Functions >> ----- //
static std::string randomIdentifier()
{
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_int_distribution dist(0, 25);

	return std::string(1, alphabet[dist(gen)]);
}

static std::string randomNumber(int maximum = 100)
{
	return std::to_string(rand() % maximum);
}

static std::string randomHex()
{
	std::ostringstream stream;
	stream << std::hex << std::uppercase << (rand() % 0xFFFF);
	return stream.str();
}

static std::vector<std::string> randomInstructionTokens()
{
	int choice = rand() % 7;

	switch (choice)
	{
	case 0: return { "ADD", randomIdentifier(), randomNumber(), randomNumber() };
	case 1: return { "SUBTRACT", randomIdentifier() , randomNumber() , randomNumber() };
	case 2: return { "DECLARE", randomIdentifier(), randomNumber() };
	case 3: return { "PRINT", randomIdentifier() };
	case 4: return { "READ", randomIdentifier(), randomHex() };
	case 5: return { "WRITE", randomHex(), randomNumber() };
	case 6: return { "SLEEP", randomNumber(10) };
	default: return { "DECLARE", randomIdentifier() };
	}
}

// ----- << Static Functions >> ----- //
static inline void trimAndUnquote(std::string& string)
{
	if (!string.empty())
	{
		string.erase(0, string.find_first_not_of(" \t"));
		string.erase(string.find_last_not_of(" \t") + 1);
	}

	if (string.size() >= 2 && string.front() == '"' && string.back() == '"')
	{
		string = string.substr(1, string.size() - 2);
	}

	if (string.size() >= 2 && string.front() == '[' && string.back() == ']')
	{
		string = string.substr(1, string.size() - 2);
	}
}

static std::vector<std::string> splitTopLevel(const std::string& string)
{
	std::vector<std::string> result;
	std::string current;
	
	int depth = 0;
	int bracket = 0;

	for (char c : string)
	{
		if (c == '(')
		{
			depth++;
		}
		else if (c == ')')
		{
			depth--;
		}
		else if (c == '[')
		{
			bracket++;
		}
		else if (c == ']')
		{
			bracket--;
		}

		if (c == ';' && depth == 0 && bracket == 0)
		{
			if (!current.empty())
			{
				result.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
	{
		result.push_back(current);
	}

	return result;
}

static std::string centerString(const std::string& string, size_t width)
{
	if (string.size() >= width)
	{
		return string;
	}

	size_t totalPadding = width - string.size();
	size_t lPadding = totalPadding / 2;
	size_t rPadding = totalPadding - lPadding;

	return std::string(lPadding, ' ') + string + std::string(rPadding, ' ');
}
// ----- << Private Member Function Implementation >> ------ //
void CPU::createProcess(std::string name, size_t heapSize, std::vector<std::unique_ptr<Instruction>>&& instructions, size_t priority, size_t memoryRequired)
{
	size_t ID;
	{
		std::unique_lock<std::mutex> processCountLock(processCountMutex);
		ID = processesGenerated;
		processesGenerated++;
	}

	auto processControlBlock = std::make_shared<PCB>(ID, heapSize, std::move(instructions), priority, memoryRequired);
	processControlBlock->setName(name);
	
	cores[processControlBlock->getProcessID() % cores.size()]->assign(processControlBlock);
	
	{
		std::unique_lock<std::mutex> masterListLock(masterListMutex);
		masterListPCB.push_back(processControlBlock);
	}
}

void CPU::createProcess_Generator(unsigned long long ID, size_t heapSize, std::vector<std::unique_ptr<Instruction>>&& instructions, size_t priority, size_t memoryRequired) // Generator Function only
{
	if (!breaker)
	{
		auto processControlBlock = std::make_shared<PCB>(ID, heapSize, std::move(instructions), priority, memoryRequired);

		{
			processControlBlock->setName("Process_" + std::to_string(processesGenerated));
		}

		{
			std::unique_lock<std::mutex> coreLock(coreMutex);
			cores[ID % cores.size()]->assign(processControlBlock);
		}

		{
			std::unique_lock<std::mutex> masterListLock(masterListMutex);
			masterListPCB.push_back(processControlBlock);
		}

	}
}

void CPU::generatorFunction()
{
	while (generatorRunning)
	{
		GlobalClock::getInstance().waitForTick(config.batchProcessFrequency);

		std::vector<std::unique_ptr<Instruction>> instructionVector;

		/*for (size_t i = 0; i < generateRandomNumber(config.minimumInstructions, config.maximumInstructions); i++)
		{
			if (breaker) break;
			
			instructionVector.push_back(std::make_unique<Instruction_PRINT>("Hello World!"));
		}*/

		instructionVector = std::move(generateRandomInstructions(generateRandomNumber(config.minimumInstructions, config.maximumInstructions)));

		{
			std::unique_lock<std::mutex> processCountLock(processCountMutex);
			createProcess_Generator(processesGenerated, 256, std::move(instructionVector), 0, (static_cast<size_t>(1) << generateRandomNumber(config.minimumMemoryPerProcess, config.maximumMemoryPerProcess))); // TODO: Edit later for randomized required memory
			processesGenerated++;
		}
	}
}

size_t CPU::generateRandomNumber(size_t minimum, size_t maximum)
{
	static thread_local std::mt19937_64 RNG{ std::random_device{}() };

	std::uniform_int_distribution<size_t> distribution(minimum, maximum);

	return distribution(RNG);
}

std::vector<std::vector<std::string>> CPU::instructionTokenizer(std::string rawInstructionString)
{
	std::vector<std::vector<std::string>> parsed;
	std::vector<std::string> instructionList = splitTopLevel(rawInstructionString);

	for (auto& instruction : instructionList)
	{
		trimAndUnquote(instruction);

		std::vector<std::string> tokens;
		
		size_t openParenthesis = instruction.find('(');
		if (openParenthesis == std::string::npos)
		{
			tokens.push_back(instruction);
		}
		else
		{
			std::string instructionName = instruction.substr(0, openParenthesis);
			trimAndUnquote(instructionName);
			tokens.push_back(instructionName);

			size_t closeParenthesis = instruction.find_last_of(')');
			if (closeParenthesis == std::string::npos || closeParenthesis <= openParenthesis)
			{
				throw std::runtime_error("Error: Malformed instruction: missing closing parenthesis\n");
			}

			std::string parameterString = instruction.substr(openParenthesis + 1, closeParenthesis - openParenthesis - 1);

			std::vector<std::string> parameters;
			std::string current;
			int depth = 0;

			for (char c : parameterString)
			{
				if (c == '[')
				{
					depth++;
				}
				else if (c == ']')
				{
					depth--;
				}

				if (c == ',' && depth == 0)
				{
					trimAndUnquote(current);
					if (!current.empty())
					{
						parameters.push_back(current);
					}
					current.clear();
				}
				else
				{
					current += c;
				}
			}

			if (!current.empty())
			{
				trimAndUnquote(current);
				parameters.push_back(current);
			}

			for (auto& param : parameters)
			{
				tokens.push_back(param);
			}
		}

		parsed.push_back(tokens);
	}

	return parsed;
}

bool CPU::isAllDigits(const std::string& string)
{
	return !string.empty() && std::all_of(string.begin(), string.end(), [](unsigned char c)
		{
			return std::isdigit(c);
		});
}

bool CPU::isHexString(const std::string& string)
{
	if (string.empty())
	{
		return false;
	}

	size_t start = 0;
	if (string.size() > 2 && string[0] == '0' && (string[1] == 'x' || string[1] == 'X'))
	{
		start = 2;
	}

	if (start == string.size())
	{
		return false;
	}

	for (size_t i = start; i < string.size(); ++i)
	{
		if (!std::isxdigit(static_cast<unsigned char>(string[i])))
		{
			return false;
		}
	}

	return true;
}

bool CPU::isValidIdentifier(const std::string& string)
{
	if (string.empty())
	{
		std::cerr << "Error: Empty string for identifier validation\n";
		return false;
	}

	char firstCharacter = string[0];
	if (!std::isalpha(static_cast<unsigned char>(firstCharacter)))
	{
		std::cerr << "Error: First character not an alphabet character\n";
		return false;
	}

	for (char c : string)
	{
		if (std::isspace(static_cast<unsigned char>(c)))
		{
			std::cerr << "Error: Space detected in string\n";
			return false;
		}
	}

	return true;
}

CONFIGURATION CPU::readConfig()
{
	// Return this
	CONFIGURATION rtrn;

	// Variables
	std::ifstream configFile;
	std::string firstToken;
	std::string line;
	std::vector<std::string> tokens;

	// Open the file
	configFile.open("config.txt");

	// Check if the file was successfully opened
	if (!configFile.is_open())
	{
		std::cerr << "ERROR: Config.txt was not opened\n";
		return rtrn;
	}

	// Read from the file
	while (std::getline(configFile, line))
	{
		// Tokenize the line that was read from file
		tokens = tokenize(line, ' ');
		firstToken = tokens.at(0);

		if (firstToken == "batch-process-frequency")
		{
			if (tokens.size() > 2)
			{
				rtrn.batchProcessFrequency = 1;
				continue;
			}

			size_t frequency = std::stoul(tokens.at(1));
			rtrn.batchProcessFrequency = std::clamp(frequency, static_cast<size_t>(1), static_cast<size_t>(UINT32_MAX));
		}
		else if (firstToken == "num-cores")
		{
			if (tokens.size() > 2)
			{
				rtrn.coreCount = 1;
				continue;
			}

			size_t count = std::stoul(tokens.at(1));
			rtrn.coreCount = std::clamp(count, static_cast<size_t>(1), static_cast<size_t>(128));
		}
		else if (firstToken == "delay-per-execution")
		{
			if (tokens.size() > 2)
			{
				rtrn.delayPerInstructionExecution = 0;
				continue;
			}

			size_t delay = std::stoul(tokens.at(1));
			rtrn.delayPerInstructionExecution = std::clamp(delay, static_cast<size_t>(0), static_cast<size_t>(UINT32_MAX));
		}
		else if (firstToken == "maximum-instructions")
		{
			if (tokens.size() > 2)
			{
				rtrn.maximumInstructions = 1;
				continue;
			}

			size_t maximum = std::stoul(tokens.at(1));
			rtrn.maximumInstructions = std::clamp(maximum, static_cast<size_t>(1), static_cast<size_t>(UINT32_MAX));
		}
		else if (firstToken == "minimum-instructions")
		{
			if (tokens.size() > 2)
			{
				rtrn.minimumInstructions = 1;
				continue;
			}

			size_t minimum = std::stoul(tokens.at(1));
			rtrn.minimumInstructions = std::clamp(minimum, static_cast<size_t>(0), static_cast<size_t>(UINT32_MAX));
		}
		else if (firstToken == "quantum-cycles")
		{
			if (tokens.size() > 2)
			{
				rtrn.quantumCycle = 1;
				continue;
			}

			size_t cycles = std::stoul(tokens.at(1));
			rtrn.quantumCycle = std::clamp(cycles, static_cast<size_t>(1), static_cast<size_t>(UINT32_MAX));
		}
		else if (firstToken == "scheduling-alogrithm")
		{
			if (tokens.size() > 2)
			{
				rtrn.schedulerAlgorithm = "FCFS";
				continue;
			}

			if (tokens.at(1) == "RR")
			{
				rtrn.schedulerAlgorithm = tokens.at(1);
			}
			else
			{
				rtrn.schedulerAlgorithm = "FCFS";
			}
		}
		else if (firstToken == "maximum-memory-per-process")
		{
			if (tokens.size() > 2)
			{
				rtrn.maximumMemoryPerProcess = static_cast<size_t>(6);
				continue;
			}

			size_t value = std::stoul(tokens.at(1));

			if (value == 0 || (value & (value - 1)) != 0)
			{
				throw std::invalid_argument("Value must be a power of 2");
			}

			size_t exponent = static_cast<size_t>(std::log2(value));

			rtrn.maximumMemoryPerProcess = exponent;
		}
		else if (firstToken == "maximum-overall-memory")
		{
			if (tokens.size() > 2)
			{
				rtrn.maximumOverallMemory = static_cast<size_t>(6);
				continue;
			}

			size_t value = std::stoul(tokens.at(1));

			if (value == 0 || (value & (value - 1)) != 0)
			{
				throw std::invalid_argument("Value must be a power of 2");
			}

			size_t exponent = static_cast<size_t>(std::log2(value));

			rtrn.maximumOverallMemory = exponent;
		}
		else if (firstToken == "memory-per-frame")
		{
			if (tokens.size() > 2)
			{
				rtrn.memoryPerFrame = static_cast<size_t>(6);
				continue;
			}

			size_t value = std::stoul(tokens.at(1));

			if (value == 0 || (value & (value - 1)) != 0)
			{
				throw std::invalid_argument("Value must be a power of 2");
			}

			size_t exponent = static_cast<size_t>(std::log2(value));

			rtrn.memoryPerFrame = exponent;
		}
		else if (firstToken == "minimum-memory-per-process")
		{
			if (tokens.size() > 2)
			{
				rtrn.minimumMemoryPerProcess = static_cast<size_t>(6);
				continue;
			}

			size_t value = std::stoul(tokens.at(1));
			
			if (value == 0 || (value & (value - 1)) != 0)
			{
				throw std::invalid_argument("Value must be a power of 2");
			}

			size_t exponent = static_cast<size_t>(std::log2(value));

			rtrn.minimumMemoryPerProcess = exponent;
		}
	}

	return rtrn;
}

std::vector<std::string> CPU::tokenize(std::string line, char delimeter)
{
	// Variables
	std::vector<std::string> tokens;
	std::string collector;
	std::stringstream stream(line);

	// Tokenization Loop
	while (getline(stream, collector, delimeter))
	{
		tokens.push_back(collector);
	}

	return tokens;
}

// ----- << Public Member Function Implementation >> ------ //
CPU::CPU()
	: config(readConfig()),
	memoryManager(config.maximumMemoryPerProcess, config.maximumOverallMemory, config.memoryPerFrame, config.minimumMemoryPerProcess, breaker),
	generatorRunning(false), processesGenerated(0)
{
	// Start the clock
	GlobalClock::getInstance().startClock();

	// MMU Object

	// Generate the cores
	for (size_t i = 0; i < config.coreCount; i++)
	{
		cores.push_back(std::make_unique<Core>(config.schedulerAlgorithm, i, config.delayPerInstructionExecution, config.quantumCycle, memoryManager, booleanVector));
	}
}

CPU::~CPU()
{
	for (auto& core : cores)
	{
		core->shutdown();
	}

	for (auto& core : cores)
	{
		core->joinSchedulerThread();
	}

	generatorRunning = false;

	if (processGeneratorThread.joinable())
	{
		processGeneratorThread.join();
	}
}

bool CPU::isProcessContained(std::string name)
{
	std::unique_lock<std::mutex> masterListLock(masterListMutex);

	for (auto& processControlBlock : masterListPCB)
	{
		if (processControlBlock->getName() == name)
		{
			return true;
		}
	}

	return false;
}

bool CPU::isProcessRunning(std::string name)
{
	std::unique_lock<std::mutex> masterListLock(masterListMutex);

	for (auto& processControlBlock : masterListPCB)
	{
		if (processControlBlock->getName() == name)
		{
			if (processControlBlock->getState() == PCB::PROCESS_STATE::TERMINATED)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

void CPU::generateProcess(std::string name, size_t memoryRequired)
{
	std::vector<std::unique_ptr<Instruction>> instructionVector;

	for (size_t i = 0; i < generateRandomNumber(config.minimumInstructions, config.maximumInstructions); i++)
	{
		instructionVector.push_back(std::make_unique<Instruction_PRINT>("Hello World!"));
	}

	createProcess(name, 256, std::move(instructionVector), 0, memoryRequired); // Overload already locks the required variables
}

void CPU::generateProcess(std::string name, size_t memoryRequired, std::string unparsedInstructions)
{
	std::vector<std::unique_ptr<Instruction>> instructionVector = std::move(instructionInterpreter(unparsedInstructions));

	createProcess(name, 256, std::move(instructionVector), 0, memoryRequired);
}

std::vector<std::unique_ptr<Instruction>> CPU::generateRandomInstructions(size_t count)
{
	std::vector<std::unique_ptr<Instruction>> result;
	
	for (size_t i = 0; i < count; ++i)
	{
		auto tokens = randomInstructionTokens();
		std::optional<std::unique_ptr<Instruction>> instruction;

		if (tokens[0] == "ADD")				instruction = interpretADD(tokens);
		else if (tokens[0] == "SUBTRACT")	instruction = interpretSUBTRACT(tokens);
		else if (tokens[0] == "DECLARE")	instruction = interpretDECLARE(tokens);
		else if (tokens[0] == "PRINT")		instruction = interpretPRINT(tokens);
		else if (tokens[0] == "READ")		instruction = interpretREAD(tokens);
		else if (tokens[0] == "WRITE")		instruction = interpretWRITE(tokens);
		else if (tokens[0] == "SLEEP")		instruction = interpretSLEEP(tokens);

		if (instruction)
		{
			result.push_back(std::move(*instruction));
		}
	}

	return result;
}

double CPU::getAverageUtilization()
{
	double utilization = 0.0;

	{
		std::unique_lock<std::mutex> coreLock(coreMutex);
		for (auto& core : cores)
		{
			utilization += core->getUtilization();
		}
	}

	return utilization / static_cast<double>(config.coreCount);
}

std::vector<std::unique_ptr<Instruction>> CPU::instructionInterpreter(std::string rawInstructionString)
{
	std::vector<std::unique_ptr<Instruction>> interpretedInstructions;
	std::vector<std::vector<std::string>> instructionTokens = instructionTokenizer(rawInstructionString);

	for (auto& tokens : instructionTokens)
	{
		if (tokens.at(0) == "ADD")
		{
			auto instruction = interpretADD(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "DECLARE")
		{
			auto instruction = interpretDECLARE(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "FOR")
		{

			auto instruction = interpretFOR(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "PRINT")
		{
			auto instruction = interpretPRINT(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "READ")
		{
			auto instruction = interpretREAD(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "SLEEP")
		{
			auto instruction = interpretSLEEP(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "SUBTRACT")
		{
			auto instruction = interpretSUBTRACT(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
		else if (tokens.at(0) == "WRITE")
		{
			auto instruction = interpretWRITE(tokens);
			if (instruction)
			{
				interpretedInstructions.push_back(std::move(*instruction));
			}
		}
	}

	return interpretedInstructions;
}

void CPU::printProcessLog(std::string name)
{
	size_t coresAvailable = 0;
	size_t coresUsed = 0;

	std::string output = "";
	

	{
		std::unique_lock<std::mutex> coreLock(coreMutex);
		for (auto& core : cores)
		{
			if (core->isCoreBusy())
			{
				coresUsed++;
			}
			else
			{
				coresAvailable++;
			}
		}
	}

	output += "\nCores Used: " + std::to_string(coresUsed) + "\n";
	output += "\nCores Available: " + std::to_string(coresAvailable) + "\n";

	{
		std::unique_lock<std::mutex> masterListLock(masterListMutex);
		for (auto& processControlBlock : masterListPCB)
		{
			if (processControlBlock->getName() == name)
			{
				// ID and Name
				output += "\nProcess ID: " + std::to_string(processControlBlock->getProcessID());
				output += "\nProcess Name: " + processControlBlock->getName() + "\n\n";

				// Collect the log
				output += processControlBlock->getLog() + "\n";

				if (processControlBlock->getState() == PCB::PROCESS_STATE::TERMINATED)
				{
					output += "\n\nTERMINATED\n\n";
				}
				break;
			}
		}
	}

	std::cout << output;
}

void CPU::printProcessStatus()
{
	// Variables
	size_t coresAvailable = 0;
	size_t coresUsed = 0;

	std::string output = "";
	std::string running = "";
	std::string terminated = "";

	{
		std::unique_lock<std::mutex> coreLock(coreMutex);
		for (auto& core : cores)
		{
			if (core->isCoreBusy())
			{
				coresUsed++;
			}
			else
			{
				coresAvailable++;
			}
		}
	}

	output += "\n\nCores Used: " + std::to_string(coresUsed);
	output += "\nCores Available: " + std::to_string(coresAvailable);
	output += "\n\nUtilization: " + std::to_string(getAverageUtilization());

	{
		std::unique_lock<std::mutex> masterListLock(masterListMutex);
		for (auto& processControlBlock : masterListPCB)
		{
			// Implement logic here
			if (processControlBlock->getState() == PCB::PROCESS_STATE::TERMINATED)
			{
				terminated += processControlBlock->getName() + "\n";
			}
			else
			{
				running += processControlBlock->getName() + "\n";
			}
		}
	}

	std::cout << output + "\n\n";
	std::cout << "Running Processes:\n";
	std::cout << running;
	std::cout << "\nTerminated Processes:\n";
	std::cout << terminated;
}

std::vector<std::string> CPU::split(std::string raw, char delimiter)
{
	std::vector<std::string> tokens;
	std::string current;
	bool inBrackets = false;

	for (char c : raw)
	{
		if (c == '[')
		{
			inBrackets = true;
			current += c;
		}
		else if (c == ']')
		{
			inBrackets = false;
			current += c;
		}
		else if (c == delimiter && !inBrackets)
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if (!current.empty())
	{
		tokens.push_back(current);
	}

	return tokens;
}

void CPU::startGenerator()
{
	if (!generatorRunning)
	{
		generatorRunning = true;
		breaker = false;
		processGeneratorThread = std::thread(&CPU::generatorFunction, this);
	}
}

void CPU::stopGenerator()
{
	generatorRunning = false;
	breaker = true;
	if (processGeneratorThread.joinable())
	{
		processGeneratorThread.join();
	}
}

void CPU::writeReport()
{
	std::ofstream reportFile("Process-Report.txt");

	if (!reportFile.is_open())
	{
		std::cerr << "Error: Target file was not opened.\n";
		return;
	}

	// Variables
	size_t coresAvailable = 0;
	size_t coresUsed = 0;

	std::string output = "";
	std::string running = "";
	std::string terminated = "";

	{
		std::unique_lock<std::mutex> coreLock(coreMutex);
		for (auto& core : cores)
		{
			if (core->isCoreBusy())
			{
				coresUsed++;
			}
			else
			{
				coresAvailable++;
			}
		}
	}

	output += "\n\nCores Used: " + std::to_string(coresUsed);
	output += "\nCores Available: " + std::to_string(coresAvailable);
	output += "\n\nUtilization: " + std::to_string(getAverageUtilization());

	{
		std::unique_lock<std::mutex> masterListLock(masterListMutex);
		for (auto& processControlBlock : masterListPCB)
		{
			// Implement logic here
			if (processControlBlock->getState() == PCB::PROCESS_STATE::TERMINATED)
			{
				terminated += processControlBlock->getName() + "\n";
			}
			else
			{
				running += processControlBlock->getName() + "\n";
			}
		}
	}

	reportFile << output + "\n\n";
	reportFile << "Running Processes:\n";
	reportFile << running;
	reportFile << "\nTerminated Processes:\n";
	reportFile << terminated;

	reportFile.close();
}

void CPU::visualizeMemoryDetailed()
{
	size_t lineLength = 60;
	size_t columnLength = lineLength / 2;
	std::ostringstream output;
	
	output << std::setw(lineLength) << std::setfill('-') << "\n";
	output << "|" <<  centerString("Detailed Memory Visualization", lineLength - 3) << "|\n";
	output << std::setw(lineLength) << std::setfill('-') << "\n";

	output << std::setfill(' ');

	size_t total = static_cast<size_t>(1) << config.maximumOverallMemory;
	// Total Memory
	output << std::left << std::setw(columnLength) << "Total Memory"
		<< std::left << std::setw(columnLength) << total
		<< "\n";

	// Used Memory
	output << std::left << std::setw(columnLength) << "Used Memory"
		<< std::left << std::setw(columnLength) << (total - memoryManager.getAvailableMemory())
		<< "\n";

	// Free Memory
	output << std::left << std::setw(columnLength) << "Free Memory"
		<< std::left << std::setw(columnLength) << memoryManager.getAvailableMemory()
		<< "\n";

	auto ticks = booleanVector.query();

	// Idle CPU ticks
	output << std::left << std::setw(columnLength) << "Idle CPU Ticks"
		<< std::left << std::setw(columnLength) << (ticks.second - ticks.first)
		<< "\n";

	// Active CPU ticks
	output << std::left << std::setw(columnLength) << "Active CPU Ticks"
		<< std::left << std::setw(columnLength) << ticks.first
		<< "\n";

	// Total CPU ticks
	output << std::left << std::setw(columnLength) << "Total CPU Ticks"
		<< std::left << std::setw(columnLength) << ticks.second
		<< "\n";

	// Number of Paged In
	output << std::left << std::setw(columnLength) << "Number of Paged In"
		<< std::left << std::setw(columnLength) << memoryManager.getPagesIn()
		<< "\n";

	// Number of Paged Out
	output << std::left << std::setw(columnLength) << "Number of Paged Out"
		<< std::left << std::setw(columnLength) << memoryManager.getPagesOut()
		<< "\n";

	output << std::setw(lineLength) << std::setfill('-') << "\n";

	std::cout << output.str();
}

void CPU::visualizeMemorySummary()
{
	size_t lineLength = 60;
	size_t columnLength = lineLength / 2;
	std::ostringstream output;

	output << std::setw(lineLength) << std::setfill('-') << "\n";
	output << "|" << centerString("PROCESS-SMI", lineLength - 3) << "|\n";
	output << std::setw(lineLength) << std::setfill('-') << "\n";

	output << std::setfill(' ');

	// CPU utilization
	std::ostringstream utilizationString;
	utilizationString << ": ";
	utilizationString << std::fixed << std::setprecision(2) << getAverageUtilization() << "%";
	
	output << std::left << std::setw(columnLength) << "CPU Util"
		<< std::left << std::setw(columnLength) << utilizationString.str()
		<< "\n";

	size_t total = static_cast<size_t>(1) << config.maximumOverallMemory;
	size_t used = (total - memoryManager.getAvailableMemory());
	// Memory Uage
	std::ostringstream memoryUsageString;
	memoryUsageString << ": ";
	memoryUsageString << used << " bytes";
	memoryUsageString << " / " << total << " bytes";

	output << std::left << std::setw(columnLength) << "Memory Usage"
		<< std::left << std::setw(columnLength) << memoryUsageString.str()
		<< "\n";

	// Memory Utilization
	double dTotal = static_cast<double>(total);
	double dUsed = static_cast<double>(used);
	std::ostringstream memoryUtilizationString;
	memoryUtilizationString << ": ";
	memoryUtilizationString << std::fixed << std::setprecision(2) << static_cast<double>(dUsed / dTotal * 100.0) << "%";

	output << std::left << std::setw(columnLength) << "Memory Util"
		<< std::left << std::setw(columnLength) << memoryUtilizationString.str()
		<< "\n\n";

	// Running Processes
	output << std::setw(lineLength) << std::setfill('=') << "\n";
	output << "\n|" << centerString("Running Processes and Memory Usage", lineLength - 3) << "|";
	output << std::setw(lineLength) << std::setfill('-') << "\n";

	output << std::setfill(' ');

	{
		std::unique_lock<std::mutex> coreLock(coreMutex);

		for (auto& core : cores)
		{
			auto current = core->getCurrentID();

			if (current.second)
			{
				size_t usedMemory = memoryManager.countValid(current.first) * (static_cast<size_t>(1) << config.memoryPerFrame);
				std::string printLine = std::to_string(usedMemory) + " bytes";
				
				if (usedMemory != 0)
				{
					output << '\n';
					output << std::left << std::setw(columnLength) << *current.second
						<< std::left << std::setw(columnLength) << printLine;
				}
			}
		}
	}

	output << std::setw(lineLength) << std::setfill('-') << "\n";

	std::cout << output.str() << "\n";
}

void CPU::printConfig() const
{
	std::cout << config.batchProcessFrequency << std::endl;
	std::cout << config.coreCount<< std::endl;
	std::cout << config.delayPerInstructionExecution << std::endl;
	std::cout << config.maximumInstructions << std::endl;
	std::cout << config.minimumInstructions << std::endl;
	std::cout << config.quantumCycle << std::endl;
	std::cout << config.schedulerAlgorithm << std::endl;
}

// ----- << Protected Member Function Implementation >> ----- //

std::optional<std::unique_ptr<Instruction>> CPU::interpretADD(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "ADD" || instructionTokens.size() != 4)
	{
		return std::nullopt;
	}

	if (!isValidIdentifier(instructionTokens.at(1)))
	{
		return std::nullopt;
	}

	std::string dst = instructionTokens.at(1);


	if (isAllDigits(instructionTokens.at(2)) && isAllDigits(instructionTokens.at(3)))
	{
		uint16_t first = static_cast<uint16_t>(std::stoul(instructionTokens.at(2)));
		uint16_t second = static_cast<uint16_t>(std::stoul(instructionTokens.at(3)));
		return std::make_unique<Instruction_ADD>(dst, first, second);
	}
	else if (isValidIdentifier(instructionTokens.at(2)) && isAllDigits(instructionTokens.at(3)))
	{
		uint16_t second = static_cast<uint16_t>(std::stoul(instructionTokens.at(3)));
		return std::make_unique<Instruction_ADD>(dst, instructionTokens.at(2), second);
	}
	else if (isValidIdentifier(instructionTokens.at(3)) && isAllDigits(instructionTokens.at(2)))
	{
		uint16_t first = static_cast<uint16_t>(std::stoul(instructionTokens.at(2)));
		return std::make_unique<Instruction_ADD>(dst, instructionTokens.at(3), first);
	}
	else if (isValidIdentifier(instructionTokens.at(2)) && isValidIdentifier(instructionTokens.at(3)))
	{
		return std::make_unique<Instruction_ADD>(dst, instructionTokens.at(2), instructionTokens.at(3));
	}

	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretDECLARE(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		std::cerr << "Error: No tokens received for DECLARE\n";
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "DECLARE" || !(instructionTokens.size() == 2 || instructionTokens.size() == 3))
	{
		std::cerr << "Error: Incorrcet number of tokens for DECLARE\n";
		return std::nullopt;
	}

	if (!isValidIdentifier(instructionTokens.at(1)))
	{
		//std::cerr << "Error: Not a valid identifier for DECLARE\n";
		//std::cerr << "       [" << instructionTokens.at(1) << "]\n";
		return std::nullopt;
	}

	if (instructionTokens.size() == 2)
	{
		if (isValidIdentifier(instructionTokens.at(1)))
		{
			return std::make_unique<Instruction_DECLARE>(instructionTokens.at(1));
		}
	}
	else if (instructionTokens.size() == 3)
	{
		if (isValidIdentifier(instructionTokens.at(1)) && isAllDigits(instructionTokens.at(2)))
		{
			return std::make_unique<Instruction_DECLARE>(instructionTokens.at(1), static_cast<uint16_t>(std::stoul(instructionTokens.at(2))));
		}
	}

	std::cerr << "Error: Unsuccessful interpretation of DECLARE\n";
	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretFOR(std::vector<std::string> instructionTokens)
{
	std::vector<std::unique_ptr<Instruction>> instructions;

	if (instructionTokens.empty())
	{
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "FOR" || instructionTokens.size() != 3)
	{
		return std::nullopt;
	}

	if (isAllDigits(instructionTokens.at(2)))
	{
		instructions = std::move(instructionInterpreter(instructionTokens.at(1)));
		return std::make_unique<Instruction_FOR>(instructions, static_cast<uint16_t>(std::stoul(instructionTokens.at(2))));
	}

	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretPRINT(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "PRINT" || !(instructionTokens.size() == 2 || instructionTokens.size() == 3))
	{
		return std::nullopt;
	}

	if (instructionTokens.size() == 2)
	{
		return std::make_unique<Instruction_PRINT>(instructionTokens.at(1));
	}

	else if (instructionTokens.size() == 3)
	{
		if (isValidIdentifier(instructionTokens.at(2)))
		{
			return std::make_unique<Instruction_PRINT>(instructionTokens.at(1), instructionTokens.at(2));
		}
	}

	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretREAD(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		std::cerr << "Error: No tokens received for READ\n";
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "READ" || instructionTokens.size() != 3)
	{
		std::cerr << "Error: Incorrect number of tokens received for READ\n";
		return std::nullopt;
	}

	if (isHexString(instructionTokens.at(2)) && isValidIdentifier(instructionTokens.at(1)))
	{
		return std::make_unique<Instruction_READ>(instructionTokens.at(2), instructionTokens.at(1));
	}

	std::cerr << "Error: Failed to interpret READ instruction\n";
	std::cerr << "       Check address and variable\n";
	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretSLEEP(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		std::cerr << "Error: No tokens received for SLEEP\n";
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "SLEEP" || instructionTokens.size() != 2)
	{
		std::cerr << "Error: Incorrect number of tokens received for SLEEP\n";
		return std::nullopt;
	}

	if (isAllDigits(instructionTokens.at(1)))
	{
		return std::make_unique<Instruction_SLEEP>(static_cast<uint8_t>(std::stoul(instructionTokens.at(1))));
	}

	std::cerr << "Error: Failed to interpret SLEEP instruction\n";
	std::cerr << "       Check duration\n";
	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretSUBTRACT(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "SUBTRACT" || instructionTokens.size() != 4)
	{
		return std::nullopt;
	}

	if (!isValidIdentifier(instructionTokens.at(1)))
	{
		return std::nullopt;
	}

	std::string dst = instructionTokens.at(1);


	if (isAllDigits(instructionTokens.at(2)) && isAllDigits(instructionTokens.at(3)))
	{
		uint16_t first = static_cast<uint16_t>(std::stoul(instructionTokens.at(2)));
		uint16_t second = static_cast<uint16_t>(std::stoul(instructionTokens.at(3)));
		return std::make_unique<Instruction_SUBTRACT>(dst, first, second);
	}
	else if (isValidIdentifier(instructionTokens.at(2)) && isAllDigits(instructionTokens.at(3)))
	{
		uint16_t second = static_cast<uint16_t>(std::stoul(instructionTokens.at(3)));
		return std::make_unique<Instruction_SUBTRACT>(dst, instructionTokens.at(2), second);
	}
	else if (isValidIdentifier(instructionTokens.at(3)) && isAllDigits(instructionTokens.at(2)))
	{
		uint16_t first = static_cast<uint16_t>(std::stoul(instructionTokens.at(2)));
		return std::make_unique<Instruction_SUBTRACT>(dst, instructionTokens.at(3), first);
	}
	else if (isValidIdentifier(instructionTokens.at(2)) && isValidIdentifier(instructionTokens.at(3)))
	{
		return std::make_unique<Instruction_SUBTRACT>(dst, instructionTokens.at(2), instructionTokens.at(3));
	}

	return std::nullopt;
}

std::optional<std::unique_ptr<Instruction>> CPU::interpretWRITE(std::vector<std::string> instructionTokens)
{
	if (instructionTokens.empty())
	{
		std::cerr << "Error: No tokens received for WRITE\n";
		return std::nullopt;
	}

	if (instructionTokens.at(0) != "WRITE" || instructionTokens.size() != 3)
	{
		std::cerr << "Error: Incorrect number of tokens received for WRITE\n";
		return std::nullopt;
	}

	if (isHexString(instructionTokens.at(1)) && isAllDigits(instructionTokens.at(2)))
	{
		return std::make_unique<Instruction_WRITE>(instructionTokens.at(1), static_cast<uint16_t>(std::stoul(instructionTokens.at(2))));
	}

	std::cerr << "Error: Failed to interpret WRITE instruction\n";
	std::cerr << "       Check address and variable\n";
	return std::nullopt;
}