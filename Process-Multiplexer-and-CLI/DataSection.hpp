#pragma once

// ----- << External Library >> ----- //
#include <unordered_map>
#include <string>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates the data section of a process 
 *			which are mainly initialized and unitialized variables
 */
class DataSection
{
private:
	// User-defined Data Members
	std::unordered_map<std::string, uint16_t> initializedData;
	std::unordered_map<std::string, uint16_t> uninitializedData;

	// Primitive Data Members
public:
	/**
	 * @brief	Class constructor
	 */
	DataSection();

	/**
	 * @brief	Class destructor
	 */
	~DataSection();

	/**
	 * @brief	Adds an initialized data to the data section
	 * 
	 * @param	variableName --
	 *			Name of the variable that is going to be added
	 * 
	 * @param	variableData --
	 *			Data stored associated with the variableName
	 */
	void addInitializedVariable(
		std::string variableName,
		uint16_t variableData
	);

	/**
	 * @brief	Adds an uninitialized data to the data section
	 * 
	 * @param	variableName --
	 *			Name of the varaible that is going to be added
	 */
	void addUninitializedVariable(std::string variableName);

	/**
	 * @brief	Checks if the data section contains a variable
	 * 
	 * @param	variableName --
	 *			Name of the variable that needs to be checked
	 */
	bool contains(std::string variableName);

	/**
	 * @brief	Get the data associated with a specific variable
	 *			name
	 * 
	 * @param	variableName --
	 *			Name of the variable whose data is going to be
	 *			retrieved
	 * 
	 * @return	Data associated with the variableName
	 */
	uint16_t getData(std::string variableName);

	/**
	 * @brief	Updates the data associated with a variabled
	 * 
	 * @param	variableName --
	 *			Name of the variable whose data is being 
	 *			updated
	 * 
	 * @param	variableData --
	 *			Data that is going to replace the value associated
	 *			with the variable
	 */
	void updateVariable(
		std::string variableName,
		uint16_t variableData
	);
};