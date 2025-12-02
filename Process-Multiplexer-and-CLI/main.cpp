#include <iostream>
#include <thread>
#include <chrono>
#include "CPU.hpp"

std::string inst = "DECLARE(VAR, 1); DECLARE(VB, 0); WRITE(00000, 45); FOR([FOR([ADD(VAR, VAR, 1)], 4);], 4); PRINT(\"Value of VAR: \", VAR); READ(VB, 00000); PRINT(\"Value of VB: \", VB);";
//std::string inst = "DECLARE(VAR, 0); WRITE(00000, 45); READ(VAR, 00000); PRINT(\"Value of VAR: \", VAR);";
// FOR([FOR([ADD(VAR, VAR, 1)], 4);], 4);

void clearConsole()
{
	std::cout << "\033\[2J\033[1;H";
}

std::vector<std::string> split(std::string raw, char delimiter)
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

std::vector<std::string> splitCommandA(const std::string& input)
{
	std::vector<std::string> tokens;
	std::istringstream stream(input);
	std::string token;

	while (stream >> std::ws)
	{
		if (stream.peek() == '"')
		{
			stream.get();
			std::getline(stream, token, '"');
			tokens.push_back(token);
		}
		else
		{
			stream >> token;
			tokens.push_back(token);
		}
	}

	return tokens;
}

static void _Screen_Commands(std::vector<std::string>& input, CPU& dev)
{
	if (input.size() == 2 && input.at(1) == "-ls")		dev.printProcessStatus();
	else if (input.size() == 5 && input.at(1) == "-c")	dev.generateProcess(input.at(2), std::stoul(input.at(3)), input.at(4));
	else if (input.size() == 3)
	{
		std::string processName = input.at(2);
		std::string command;

		if (input.at(1) == "-r")
		{
			if (dev.isProcessContained(processName))
			{
				if (dev.isProcessRunning(processName))
				{
					bool innerLoop = true;

					clearConsole();

					while (innerLoop)
					{
						clearConsole();
						std::cout << "---------------------------------------------\n";
						std::cout << "DEVLEOPER: Ampatin, Ian Kenneth J.\n\n";
						std::cout << "Last Update: December 2, 2025\n";
						std::cout << "---------------------------------------------\n";
						std::cout << processName + "\\: ";
						std::getline(std::cin, command);

						if (!command.empty())
						{
							if (command == "process-smi")	dev.printProcessLog(processName);
							else if (command == "exit")		innerLoop = false;
						}
					}
				}
				else std::cout << "\n\nProcess [" + processName + "] not found\n\n";
			}
			else std::cout << "\n\nProcess [" + processName + "] not found\n\n";
		}
	}
	else if (input.size() == 4 && input.at(1) == "-s")
	{
		std::string processName = input.at(2);
		std::string command;

		dev.generateProcess(processName, std::stoul(input.at(3)));
		clearConsole();

		bool innerLoop = true;

		while (innerLoop)
		{
			clearConsole();
			std::cout << "---------------------------------------------\n";
			std::cout << "DEVLEOPER: Ampatin, Ian Kenneth J.\n\n";
			std::cout << "Last Update: December 2, 2025\n";
			std::cout << "---------------------------------------------\n";
			std::cout << processName + "\\: ";
			std::getline(std::cin, command);

			if (!command.empty())
			{
				if (command == "process-smi")		dev.printProcessLog(processName);
				else if (command == "exit")			innerLoop = false;
			}
		}
	}
}

static void systemInitialized()
{
	bool isInitialized = true;

	std::string lineCommand;
	std::vector<std::string> tokens;

	CPU MC;

	while (isInitialized)
	{
		clearConsole();
		std::cout << "---------------------------------------------\n";
		std::cout << "DEVLEOPER: Ampatin, Ian Kenneth J.\n\n";
		std::cout << "Last Update: December 2, 2025\n";
		std::cout << "---------------------------------------------\n";
		std::cout << "\ninitialized\\: ";
		std::getline(std::cin, lineCommand);

		if (!lineCommand.empty())
		{
			if (lineCommand == "scheduler-test")		MC.startGenerator();
			else if (lineCommand == "scheduler-stop")	MC.stopGenerator();
			else if (lineCommand == "vmstat")			MC.visualizeMemoryDetailed();
			else if (lineCommand == "process-smi")		MC.visualizeMemorySummary();
			else if (lineCommand == "exit")				isInitialized = false;
			else
			{
				tokens = splitCommandA(lineCommand);

				if (tokens.at(0) == "screen")			_Screen_Commands(tokens, MC);
			}
		}
	}

}


int main()
{
	
	bool isRunning = true;

	std::string line;
	while (isRunning)
	{
		clearConsole();
		std::cout << "---------------------------------------------\n";
		std::cout << "DEVLEOPER: Ampatin, Ian Kenneth J.\n\n";
		std::cout << "Last Update: December 2, 2025\n";
		std::cout << "---------------------------------------------\n";
		std::cout << "root\\: ";
		std::getline(std::cin, line);

		if (!line.empty())
		{
			if (line == "initialize")
			{
				systemInitialized();
			}
			else if (line == "exit")
			{
				isRunning = false;
			}
		}

	}

	//tester();

	return 0;
}

/*
 // Debugging Functions 
void mmuLRU()
{
	MMU memory(64, 192, 64, 64);

	for (unsigned long long i = 0; i < 6; i++)
	{
		memory.createPages(i);
	}

	// 1
	if (memory.protectedWrite(1, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 2
	if (memory.protectedWrite(2, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 3
	if (memory.protectedWrite(3, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 4
	if (memory.protectedWrite(4, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 5
	if (memory.protectedWrite(1, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 6
	if (memory.protectedWrite(2, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 7
	if (memory.protectedWrite(5, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	memory.remove(5);
	std::cout << "Remove Operation on Process 5" << std::endl;
	memory.printFrames();
	std::cout << "\n";

	// 8
	if (memory.protectedWrite(1, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	// 9
	if (memory.protectedWrite(2, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	if (memory.protectedWrite(3, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	if (memory.protectedWrite(4, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	if (memory.protectedWrite(5, "01", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}
	
	memory.protectedWrite(3, "03", "77");
	auto tst = memory.protectedRead(3, "00", 4);

	if (tst)
	{
		std::cerr << *tst;
	}
}
void mmu()
{
	MMU memory(64, 128, 64, 64);

	for (unsigned long long i = 0; i < 2; i++)
	{
		memory.createPages(i);
	}

	if (memory.protectedWrite(1, "00040", "FF"))
	{
		std::cout << "Write successful\n";
		std::cout << "\n\n\n";
		memory.printFrames();
		std::cout << "\n\n\n";
	}

	for (unsigned long long i = 2; i < 4; i++)
	{
		memory.createPages(i);
		memory.handlePageFault(i, 0);
	}
}
void mmuT()
{
	MMU memory(128, 1024, 64, 64);

	size_t count = 16;

	for (unsigned long long i = 0; i < count; i++)
	{
		memory.createPages(i);
	}

	for (unsigned long long i = 0; i < count; i++)
	{
		memory.handlePageFault(i, 0);
	}

	memory.handlePageFault(0, 1);
	memory.handlePageFault(0, 0);

	memory.remove(5);

	memory.handlePageFault(0, 0);
	//memory.handlePageFault(0, 1);

	memory.printFrames();
}
void cpuDBG()
{
	CPU c;

	std::vector<std::vector<std::string>> t = c.instructionTokenizer(inst);

	for (const auto& instruction : t)        // each inner vector
	{
		for (const auto& token : instruction) // each string inside
		{
			std::cout << "|" << token << "|" << "\n";
		}
		std::cout << "\n\n\n";
	}
}
void tester()
{
	CPU test;
	for (size_t i = 0; i < 100; i++)
	{
		std::string name = "Process_" + std::to_string(i);
		test.generateProcess(name, 64, inst);
	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	test.visualizeMemoryDetailed();
	std::cout << "\n\n\n";
	test.visualizeMemorySummary();
}
 */