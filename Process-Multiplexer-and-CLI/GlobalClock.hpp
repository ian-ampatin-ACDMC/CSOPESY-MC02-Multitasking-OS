#pragma once

// ----- << External Library >> ----- //
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

// ----- << Class >> ----- //
class GlobalClock
{
private:
	// User-defined Data Members
	std::atomic<size_t> ticks;
	std::condition_variable clockCV;

	std::mutex clockMutex;

	std::thread clockThread;

	// Primitive Data Members
	bool clockRunning;

	/**
	 * @brief	Clock logic
	 */
	void clockFunction();

public:
	/** 
	 * @brief	Class Constructor
	 */
	GlobalClock();

	/**
	 * @brief	Class destructor
	 */
	~GlobalClock();

	/**
	 * @brief	
	 */
	/**
	 * @brief	Get the clock
	 * 
	 * @return	The same clock that was generated when the object
	 *			was created
	 */
	static GlobalClock& getInstance();

	/**
	 * @brief	Get the number of ticks
	 * 
	 * @return	The current number of ticks
	 */
	size_t getTicks() const { return ticks; }

	/**
	 * @brief	Start the clock
	 */
	void startClock() { clockRunning = true; };

	/**
	 * @brief	Waits until a tick has passed
	 */
	void waitForTick();

	/**
	 * @brief	Wait for an N number of ticks
	 * 
	 * @param	numTicks --
	 *			Is the number of ticks that must pass before 
	 *			execution can return to the original thread
	 *			the function was called on
	 */
	void waitForTick(size_t numTicks);
};