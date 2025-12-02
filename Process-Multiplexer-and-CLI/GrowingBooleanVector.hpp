#pragma once

// ----- << External Library >> ----- //
#include <mutex>
#include <vector>

// ----- << Class >> ----- //

/**
 * @brief	A class that only allows the storage of boolean
 *			vectors that automatically performs an OR operation
 *			when a vector is added or if an element is added to
 *			each individual vector
 */
class GrowingBooleanVector
{
private:
	// User-defined Data Members
	size_t trueCount;
	size_t maxSize;

	std::mutex mutex;

	std::vector<bool> resultVector;
	std::vector<std::vector<bool>> vectors;

	/**
	 * @brief	Recomputes the number of true values
	 *			there are
	 */
	void recomputeTrueCount();

public:
	/**
	 * @brief	Class constructor
	 */
	GrowingBooleanVector();

	/**
	 * @brief	Class destructor
	 */
	~GrowingBooleanVector();
	/**
	 * @brief	Adds a vector to the class
	 * 
	 * @param	vector --
	 *			The vector being added to the class
	 */
	void addVector(const std::vector<bool>& vector);

	/**
	 * @brief	Appends an element to one of the vectors in 
	 *			the class
	 * 
	 * @param	index --
	 *			The index of the vector which will have an element 
	 *			added to it
	 * 
	 * @param	value --
	 *			The element being added to the vector
	 */
	void appendElement(size_t index, bool value);

	/**
	 * @brief	Retrieve the number of true values there are in the 
	 *			result and the ovearll size of the vector
	 */
	std::pair<size_t, size_t> query() const;

};