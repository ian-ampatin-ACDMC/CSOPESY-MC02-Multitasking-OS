// ----- << External Librarary >> ----- //
#include <list>
#include <optional>
#include <unordered_map>

// ----- << Class >> ----- //

/**
 * @brief	A class that will emulate an unoredered map that only
 *			stores a limited number of entries
 * 
 *			When full, entries are deleted using the 
 *			Least Recently Used algorithm
 */

template <typename A, typename B>
class LUM
{
private:
	// User-defined Data Members
	size_t size;

	std::list<A> usageList;
	std::unordered_map<A, 
		std::pair<B, typename std::list<A>::iterator>> map;

public:
	/**
	 * @brief	Class constructor
	 */
	LUM(size_t size);

	/**
	 * @brief	Class destructor
	 */
	~LUM();

	/**
	 * @brief	Find an element in the map
	 * 
	 * @param	key --
	 *			The key of the pair in the map
	 * 
	 * @return	The second element of the pair
	 */
	std::optional<B> find(const A& key);

	/**
	 * @brief	Insert an element into the map
	 * 
	 * @param	key --
	 *			Unique identifier for a value pair
	 * 
	 * @param	value --
	 *			Value associated with a key
	 */
	void insert(const A& key, const B& value);

	/**
	 * @brief	Update the value of assigned to a key
	 * 
	 * @param	key --
	 *			Unique identifier for a value pair
	 * 
	 * @param	value --
	 *			The data that will replace the current value
	 *			associated with the key
	 */
	bool update(const A& key, const B& value);
};