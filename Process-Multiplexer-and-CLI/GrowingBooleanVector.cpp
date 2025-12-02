// ----- << External Library >> ----- //
#include "GrowingBooleanVector.hpp"

// ----- << Private Member Function Implementation >> ----- //
void GrowingBooleanVector::recomputeTrueCount()
{
	trueCount = 0;
	for (bool boolean : resultVector)
	{
		if (boolean)
		{
			++trueCount;
		}
	}
}

// ----- << Public Member Function Implementation >> ----- //
GrowingBooleanVector::GrowingBooleanVector()
	: trueCount(0), maxSize(0)
{

}

GrowingBooleanVector::~GrowingBooleanVector() = default;

void GrowingBooleanVector::addVector(const std::vector<bool>& vector)
{
	std::unique_lock<std::mutex> lock(mutex);

	vectors.push_back(vector);
	size_t index = vectors.size() - 1;

	if (vector.size() > maxSize)
	{
		std::vector<bool> newResult(vector.size(), false);

		size_t offset = vector.size() - resultVector.size();
		for (size_t i = 0; i < resultVector.size(); ++i)
		{
			newResult[i + offset] = resultVector[i];
		}

		resultVector.swap(newResult);
		maxSize = vector.size();

		recomputeTrueCount();
	}

	size_t offset = maxSize - vector.size();
	for (size_t i = 0; i < vector.size(); ++i)
	{
		if (vector[i] && !resultVector[i])
		{
			resultVector[i + offset] = true;
			++trueCount;
		}
	}
}

void GrowingBooleanVector::appendElement(size_t index, bool value)
{
	std::unique_lock<std::mutex> lock(mutex);

	if (index >= vectors.size())
	{
		return;
	}

	vectors[index].push_back(value);
	size_t position = vectors[index].size() - 1;
	size_t size = vectors[index].size();

	if (position >= resultVector.size())
	{
		resultVector.resize(size, false);
		maxSize = resultVector.size();
	}

	if (value && !resultVector[position])
	{
		resultVector[position] = true;
		++trueCount;
	}
}

std::pair<size_t, size_t> GrowingBooleanVector::query() const
{
	return { trueCount, maxSize };
}