// ----- << External Library >> ----- //
#include "LogicalDataSection.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

// ---- << Private Member Function Implementation >> ----- //
std::string LogicalDataSection::convertDecToHex(size_t dec)
{
	std::ostringstream stream;

	stream << std::hex << std::uppercase;
	stream << std::setw(4) << std::setfill('0') << dec;

	return stream.str();
}

size_t LogicalDataSection::convertHexToDecimal(std::string hex)
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

// ----- << Public Member Function Implementation >> ----- //
LogicalDataSection::LogicalDataSection(size_t numberOfVariables)
	: maximumVariables(numberOfVariables)
{
	for (size_t i = 0; i < numberOfVariables; i++)
	{
		std::ostringstream stream;
		stream << std::hex << std::uppercase
			<< std::setw(5) << std::setfill('0') << (i * 2);
		std::string address = stream.str();

		data[address] = std::nullopt;
	}
}

LogicalDataSection::~LogicalDataSection() = default;

LogicalDataSection::LogicalDataSection(LogicalDataSection&& source) noexcept
	: data(std::move(source.data)), maximumVariables(source.maximumVariables)
{

}

LogicalDataSection& LogicalDataSection::operator=(LogicalDataSection&& source) noexcept
{
	if (this != &source)
	{
		data = std::move(source.data);
	}

	return *this;
}

std::optional<uint16_t> LogicalDataSection::getData(std::string identifier)
{
	for (const auto& [key, value] : data)
	{
		if (value)
		{
			if (value->first == identifier)
			{
				return value->second;
			}
		}
	}

	return std::nullopt;
}

std::optional<std::string> LogicalDataSection::getVariableAddress(std::string identifier)
{
	for (const auto& [key, value] : data)
	{
		if (value)
		{
			if (value->first == identifier)
			{
				return key;
			}
		}
	}

	return std::nullopt;
}

bool LogicalDataSection::insertVariable(std::string identifier)
{
	if (!isFull())
	{
		for (auto& [key, value] : data)
		{
			if (!value) // if null
			{
				value = { identifier , 0 };
				return true;
			}
		}
	}

	return false;
}

bool LogicalDataSection::isFull()
{
	for (const auto& [key, value] : data)
	{
		if (!value) // if null
		{
			return false;
		}
	}

	return true;
}

void LogicalDataSection::print()
{
	for (const auto& [key, value] : data)
	{
		if (value)
		{
			std::cout << key << " | " << value->first << " -> " << value->second << "\n";
		}
		else
		{
			std::cout << key << " | " << "free\n";
		}
	}
}

bool LogicalDataSection::setValue(std::string identifier, uint16_t varibaleValue)
{
	for (auto& [key, value] : data)
	{
		if (value)
		{
			if (value->first == identifier)
			{
				value->second = varibaleValue;
				return true;
			}
		}
	}

	return false;
}