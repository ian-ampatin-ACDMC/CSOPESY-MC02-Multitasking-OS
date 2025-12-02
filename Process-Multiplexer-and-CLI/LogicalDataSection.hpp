#pragma once

// ----- << External Library >> ----- //
#include <map>
#include <optional>
#include <string>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates the data section of a process 
 *			which are mainly initialized and unitialized variables
 */
class LogicalDataSection
{
private:
	// User-defined Data Members
	size_t maximumVariables;
	std::map<std::string, std::optional
		<std::pair<std::string, uint16_t>>
	> data;
			// Address - variable identifier

	/**
	 * @brief	Conversts a decimal number to a hexadecimal string
	 *
	 * @param	dec --
	 *			The number that is being converted
	 *
	 * @return	An equivalent hexadecimal string representation of
	 *			the decimal number
	 */
	std::string convertDecToHex(size_t dec);

	/**
	 * @brief	Converts a string of hexadecmial numbers to a
	 *			decimal number
	 *
	 * @param	hex --
	 *			The string of hexadecimal characters is being
	 *			converted
	 *
	 * @return	Decimal equivalent of the hexadecimal string
	 */
	size_t convertHexToDecimal(std::string hex);

public:
	/**
	 * @brief	Class constructor
	 */
	LogicalDataSection(size_t numberOfVariables);

	/**
	 * @brief	Class destructor
	 */
	~LogicalDataSection();

	/**
	 * @brief	Copy constructor (deleted)
	 */
	LogicalDataSection(const LogicalDataSection&) = delete;

	/**
	 * @brief	Copy assignment operator (deleted)
	 */
	LogicalDataSection& 
		operator=(const LogicalDataSection&) = delete;

	/**
	 * @brief	Move constructor
	 */
	LogicalDataSection(LogicalDataSection&& source) noexcept;

	/**
	 * @brief	Move assignment operator
	 */
	LogicalDataSection& 
		operator=(LogicalDataSection&& source) noexcept;

	/**
	 * @brief	Get the value assigned to a variable given
	 *			its identifier
	 * 
	 * @param	identifier --
	 *			The identifier of the variable whose value is
	 *			being requested
	 * 
	 * @return	The data that the variable stores
	 */
	std::optional<uint16_t> getData(std::string identifier);

	/**
	 * @brief	Get the address for a variable
	 * 
	 * @param	identifier --
	 *			The identifier of the variable whose address is
	 *			being requested
	 * 
	 * @return	The address of the variable being requested if
	 *			a variable with the idenfier exists in the data 
	 *			section. Otherwise, null.
	 */
	std::optional<std::string> getVariableAddress(
		std::string identifier
	);

	/**
	 * @brief	Inserts a variable into the data section
	 * 
	 * @param	identifier --
	 *			The identifier being assigned to a variabled
	 * 
	 * @return	true if variable is successfully inserted into
	 *			the data section. Otherwise, false.
	 */
	bool insertVariable(std::string identifier);

	/**
	 * @brief	Checks if the data section is full
	 *
	 * @return	true if the data section is full. Otherwise, false.
	 */
	bool isFull();

	/**
	 * @brief	Prints the contents of the logical data section
	 */
	void print();

	/**
	 * @brief	Sets a value for a variable
	 * 
	 * @param	identifier --
	 *			Identifer of the variable being assigned a value
	 * 
	 * @param	value --
	 *			The value being assigned to the variable
	 * 
	 * @return	true if the operation is successful.
	 *			Otherwise, false.
	 */
	bool setValue(std::string identifier, uint16_t value);
};