// ----- << External Library >> ----- //
#include "GlobalClock.hpp"
#include <chrono>

// ----- << Private Member Function Implementation >> ------ //
void GlobalClock::clockFunction()
{
	while (clockRunning)
	{
		// Introduce a delay so the clock won't consume a large 
		// amoung of resources
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		{
			std::lock_guard<std::mutex> clockLock(clockMutex);
			++ticks;
		}

		clockCV.notify_all();
	}
}
// ----- << Public Member Function Implementation >> ----- //
GlobalClock::GlobalClock() : ticks(0), clockRunning(false)
{
	clockThread = std::thread(&GlobalClock::clockFunction, this);
}

GlobalClock::~GlobalClock()
{
	// Turn the clock off
	clockRunning = false;

	// Join the clockThread
	if (clockThread.joinable())
	{
		clockThread.join();
	}
}

GlobalClock& GlobalClock::getInstance()
{
	static GlobalClock globalClock;
	return globalClock;
}

void GlobalClock::waitForTick()
{
	std::unique_lock<std::mutex> clockLock(clockMutex);
	size_t current = ticks;
	clockCV.wait(clockLock, [this, current]
		{
			return ticks != current;
		});
}

void GlobalClock::waitForTick(size_t numTicks)
{
	std::unique_lock<std::mutex> clockLock(clockMutex);
	size_t current;

	for (size_t i = 0; i < numTicks; i++)
	{
		current = ticks;
		clockCV.wait(clockLock, [this, current]
			{
				return ticks != current;
			});
	}
}