#pragma once

// ----- << External Library >> ----- //
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// ----- << Class >> ----- //

/**
 * @brief	A class that emulates a heap
 */
class Heap
{
private:
	/**
	 * @brief	Meta data for describing a contiguous region of the
	 *			heaps raw memory
	 */
	struct HEAP_BLOCK
	{
		size_t start;
		size_t size;

		bool free;
	};

	/**
	 * @brief	Gives semantic meaning to heap slices
	 */
	struct ALLOCATION
	{
		HEAP_BLOCK* block;

		size_t elementSize;
		size_t count;
	};

	// User-defined Data Members
	std::unordered_map<size_t, ALLOCATION> allocations;
	std::vector<HEAP_BLOCK> blocks;
	std::vector<uint16_t> memory;

	size_t nextHandle = 1;

public:
	/**
	 * @brief	Class constructor
	 */
	Heap(size_t size);

	/**
	 * @brief	Class destructor
	 */
	~Heap();

	/**
	 * @brief	Allocate primitive data into the heap
	 */
	template<typename T>
	size_t allocate();

	/**
	 * @brief	Allocate an array to the heap
	 * 
	 * @param	size --
	 *			How large do you need the allocated space
	 */
	template<typename T>
	size_t allocateArray(size_t size);

	/**
	 * @brief	Frees a portion of the heap
	 * 
	 * @param	handle --
	 *			The handle of the allocation you want to free up
	 */
	void free(size_t handle);

	/**
	 * @brief	Get an element from the heap
	 * 
	 * @param	handle --
	 *			The handle of the allocation whose element you want
	 *			to get
	 * 
	 * @param	index --
	 *			Position of the element in the allocation
	 */
	template<typename T>
	T get(size_t handle, size_t index);

	/**
	 * @brief	Resizes an allocation
	 * 
	 * @brief	handle --
	 *			The allocation that you want to resize
	 * 
	 * @brief	newSize --
	 *			The new size of the allocation
	 */
	template<typename T>
	size_t reallocate(size_t handle, size_t newSize);

	/**
	 * @brief	Set the value of an memory space
	 * 
	 * @param	handle --
	 *			The handle of the allocation whose element you want
	 *			to set
	 * 
	 * @param	index --
	 *			Position of the element in the allocation
	 * 
	 * @param	value --
	 *			The value you want to set the memory space to
	 */
	template<typename T>
	void set(size_t handle, size_t index, const T& value);
};