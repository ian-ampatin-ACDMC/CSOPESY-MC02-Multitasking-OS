// ----- << External Libraries >> ---- //
#include "Heap.hpp"
#include <stdexcept>

// ----- << Implementation >> ----- //
Heap::Heap(size_t size) 
	: memory(size)
{
	blocks.push_back({ 0, size, true });
}

Heap::~Heap() = default;

template<typename T>
size_t Heap::allocate()
{
	return allocateArray<T>(1);
}

template<typename T>
size_t Heap::allocateArray(size_t size)
{
	size_t totalSize = size * sizeof(T);

	for (auto& block : blocks)
	{
		if (block.free && block.size >= totalSize)
		{
			size_t handle = nextHandle++;
			block.free = false;

			// Split block if larger
			if (block.size > totalSize)
			{
				blocks.push_back({ block.start + totalSize, block.size - totalSize, true });
				block.size = totalSize;
			}

			allocations[handle] = { &block, sizeof(T), size };
			return handle;
		}
	}

	throw std::runtime_error("Heap is out of memory");
}

void Heap::free(size_t handle)
{
	auto allocation = allocations.find(handle);

	if (allocation == allocations.end())
	{
		throw std::runtime_error("Invalid Handle");
	}

	allocation->second.block->free = true;

	allocations.erase(allocation); // Coalesce adjacent free blocks
}

template<typename T>
T Heap::get(size_t handle, size_t index)
{
	auto allocation = allocations.find(handle);
	if (allocation == allocations.end())
	{
		throw std::runtime_error("Invalid handle");
	}

	if (index >= allocation->second.count)
	{
		throw std::runtime_error("Index is out of bounds");
	}

	T value;
	size_t address = allocation->second.block->start + index * allocation->second.elementSize;
	std::memcpy(&value, &memory[address], sizeof(T));
	return value;
}

template<typename T>
size_t Heap::reallocate(size_t handle, size_t newSize)
{
	auto allocation = allocations.find(handle);
	if (allocation = allocations.end())
	{
		throw std::runtime_error("Invalid handle");
	}

	ALLOCATION alloc = allocation->second;
	size_t newAllocationSize = newSize * sizeof(T);
	size_t oldAllocationSize = alloc.count * alloc.elementSize;
	HEAP_BLOCK* block = alloc.block;

	// Shrinking the allocation
	if (newAllocationSize <= oldAllocationSize)
	{
		alloc.count = newAllocationSize;

		if (oldAllocationSize > newAllocationSize)
		{
			blocks.push_back({ block->start + newAllocationSize, oldAllocationSize - newAllocationSize, true });
			block->size = newAllocationSize;
		}

		allocations[handle] = alloc;
		return handle;
	}

	// Grow the allocation in place if the next block is free and large enough
	for (auto& b : blocks)
	{
		if (b.start == block->start + block->size && b.free && b.size >= (newAllocationSize - oldAllocationSize))
		{
			b.start += (newAllocationSize - oldAllocationSize);
			b.ssize -= (newAllocationSize - oldAllocationSize);
			block->size = newSize;
			alloc.count = newSize;
			allocations[handle] = alloc;
			return handle;
		}
	}

	// Allocate a new block and copy
	size_t newHandle = allocateArray<T>(newSize);
	for (size_t i = 0; i < alloc.count; i++)
	{
		T value = get<T>(handle, i);
		set<T>(newHandle, i, value);
	}
	free(handle);
	return newHandle;
}

template<typename T>
void Heap::set(size_t handle, size_t index, const T& value)
{
	auto allocation = allocations.find(handle);
	if (allocation = allocations.end())
	{
		throw std::runtime_error("Invalid handle");
	}

	if (index >= allocation->second.count)
	{
		throw std::runtime_error("Index is out of bounds");
	}

	size_t address = allocation->second.block->start + index * allocation->second.elementSize;
	std::memcpy(&memory[address], &value, sizeof(T));
}