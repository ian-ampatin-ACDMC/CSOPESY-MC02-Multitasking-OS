// ----- << External Libraries >> ---- //
#include "DataSection.hpp"
#include <stdexcept>

// ----- << Implementation >> ----- //
DataSection::DataSection() = default;

DataSection::~DataSection() = default;

void DataSection::addInitializedVariable(std::string variableName, uint16_t variableData)
{
	if (!contains(variableName))
	{
		initializedData.emplace(variableName, variableData);
	}
}

void DataSection::addUninitializedVariable(std::string variableName)
{
	if (!contains(variableName))
	{
		uninitializedData.emplace(variableName, 0);
	}
}

bool DataSection::contains(std::string variableName)
{
	auto initialized = initializedData.find(variableName);
	if (initialized != initializedData.end())
	{
		return true;
	}

	auto uninitialized = uninitializedData.find(variableName);
	if (uninitialized != uninitializedData.end())
	{
		return true;
	}

	return false;
}

uint16_t DataSection::getData(std::string variableName)
{
	auto initialized = initializedData.find(variableName);
	if (initialized != initializedData.end())
	{
		return initialized->second;
	}

	auto uninitialized = uninitializedData.find(variableName);
	if (uninitialized != uninitializedData.end())
	{
		return 0;
	}

	throw std::runtime_error("Variable not found in Data Section");
}

void DataSection::updateVariable(std::string variableName, uint16_t variableData)
{
	auto initialized = initializedData.find(variableName);
	if (initialized != initializedData.end())
	{
		initialized->second = variableData;
		return;
	}

	auto uninitialized = uninitializedData.find(variableName);
	if (uninitialized != uninitializedData.end())
	{
		initializedData.emplace(variableName, variableData);
		uninitializedData.erase(uninitialized);
		return;
	}

	throw std::runtime_error("Variable not found in Data Section");
}