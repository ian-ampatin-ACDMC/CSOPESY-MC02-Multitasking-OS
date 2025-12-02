// ----- << External Libraray >> ----- //
#include "LUM.hpp"

// ----- << Implementation >> ----- //
template <typename A, typename B>
LUM<A, B>::LUM(size_t size) : size(size)
{

}

template <typename A, typename B>
LUM<A, B>::~LUM() = default;

template <typename A, typename B>
std::optional<B> LUM<A, B>::find(const A& key)
{
	auto pair = map.find(key);
	if (pair != map.end())
	{
		usageList.splice(usageList.begin(), usageList, pair->second.second);

		return pair->second.first;
	}

	return std::nullopt;
}

template <typename A, typename B>
void LUM<A, B>::insert(const A& key, const B& value)
{
	auto pair = map.find(key);

	if (pair != map.end())
	{
		pair->second.first = value;

		usageList.splice(usageList.begin(), usageList, pair->second.second);
		return;
	}

	if (map.size() >= size)
	{
		A evictKey = usageList.back();
		usageList.pop_back();
		map.erase(evictKey);
	}

	usageList.push_front(key);
	map[key] = { value, usageList.begin() };
}