// ----- << External Libraries >> ------ //
#include "commandInterpreter.hpp"
#include "CPU.hpp"
#include "Instruction_ADD.hpp"
#include "Instruction_DECLARE.hpp"
#include "Instruction_FOR.hpp"
#include "Instruction_PRINT.hpp"
#include "Instruction_SLEEP.hpp"
#include "Instruction_SUBTRACT.hpp"
#include <sstream>

// ----- << User-defined Variables >> ----- //
std::string userCommand;
std::vector<std::string> commandTokens;

// ----- << User-defined Variables >> ----- //
bool isRunning = true;
bool isInitialized = false;
bool isProcess = false;

// ----- << Helper Functions >> ----- //
std::vector<std::string> getTokens(std::string command, char delimiter)
{
	// Variables
	std::vector<std::string> tokens;
	std::string collector;
	std::stringstream stream(command);

	// Tokenization Loop
	while (getline(stream, collector, ' '))
	{
		tokens.push_back(collector);
	}

	return tokens;
}

void clearConsole()
{
	std::cout << "\033\[2J\033[1;H";
}

// ----- << Implementation >> ----- //
void processMultiplexerCommands()
{
	while (isRunning)
	{
		{
			std::cout << "root\\: ";
			std::getline(std::cin, userCommand);
		}

		if (!userCommand.empty())
		{
			if (userCommand == "initialize")
			{
				// Message
				std::cout << "\nNOTICE: Successfully Initialized.\n\n";

				// Put the initialized loop here
				isInitialized = true;

				// Variables needed for initialized command logic
				std::string firstToken;

				// Create a CPU object
				CPU CPU01;

				while (isInitialized)
				{
					{
						std::cout << "root\\: ";
						std::getline(std::cin, userCommand);
					}

					if (!userCommand.empty())
					{
						commandTokens = getTokens(userCommand, ' ');
						firstToken = commandTokens.at(0);

						if (firstToken == "exit")
						{
							if (commandTokens.size() > 1)
							{
								std::cout << "Error: Command unrecognized.\n";
							}
							else
							{
								isInitialized = false;
								isRunning = false;
							}
						}
						else if (firstToken == "report-util")
						{
							if (commandTokens.size() > 1)
							{
								std::cout << "Error: Command unrecognized.\n";
							}
							else
							{
								CPU01.writeReport();
							}
						}
						else if (firstToken == "scheduler-start")
						{
							if (commandTokens.size() > 1)
							{
								std::cout << "Error: Command unrecognized.\n";
							}
							else
							{
								CPU01.startGenerator();
							}
						}
						else if (firstToken == "scheduler-stop")
						{
							if (commandTokens.size() > 1)
							{
								std::cout << "Error: Command unrecognized.\n";
							}
							else
							{
								CPU01.stopGenerator();
							}
						}
						else if (firstToken == "screen")
						{
							if (commandTokens.size() > 3 || commandTokens.size() < 2)
							{
								std::cout << "Error: Command unrecognized.\n";
							}
							else if (commandTokens.size() == 2) // 2 tokens
							{
								if (commandTokens.at(1) == "-ls")
								{
									CPU01.printProcessStatus();
								}
								else
								{
									std::cout << "Error: Command unrecognized.\n";
								}
							}
							else // 3 tokens
							{
								std::string secondToken = commandTokens.at(1);
								std::string processName = commandTokens.at(2);

								if (secondToken == "-r")
								{
									if (CPU01.isProcessContained(processName))
									{
										if (CPU01.isProcessRunning(processName))
										{
											clearConsole();

											// start isProcess loop
											isProcess = true;

											// Logic
											while (isProcess)
											{
												{
													std::cout << processName + "\\: ";
													std::getline(std::cin, userCommand);
												}

												if (!userCommand.empty())
												{
													if (userCommand == "process-log")
													{
														CPU01.printProcessLog(processName);
													}
													else if (userCommand == "exit")
													{
														isProcess = false;

														std::cout << "\n\nProcess Screen exited.\n\n";
														clearConsole();
													}
													else
													{
														std::cout << "Error: Command unrecognized.\n";
													}
												}
											}
										}
										else
										{
											std::cout << "\n\nProcess [" + processName + "] has been terminated\n\n";
										}
									}
									else
									{
										std::cout << "\n\nProcess [" + processName + "] does not exist.\n\n";
									}

								}
								else if (secondToken == "-s")
								{
									CPU01.generateProcess(processName, 64);
									clearConsole();

									// start isProcess loop
									isProcess = true;

									// Logic
									while (isProcess)
									{
										{
											std::cout << processName + "\\: ";
											std::getline(std::cin, userCommand);
										}

										if (!userCommand.empty())
										{
											if (userCommand == "process-log")
											{
												CPU01.printProcessLog(processName);
											}
											else if (userCommand == "exit")
											{
												isProcess = false;

												std::cout << "\n\nProcess Screen exited.\n\n";
												clearConsole();
											}
											else
											{
												std::cout << "Error: Command unrecognized.\n";
											}
										}
									}
								}
								else
								{
									std::cout << "Error: Command unrecognized.\n";
								}
							}
						}
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			}
			else if (userCommand == "exit")
			{
				isRunning = false;
			}
		}

		// Make the thread sleep so it doesn't consume too much 
		// resources
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}