//--------------------------------------------------------------------------------------
//	CTimer.cpp
//
//	Timer class - works like a stopwatch
//--------------------------------------------------------------------------------------

#include "Timer.h"

//////////////////////////////
// Constructor

Timer::Timer()
{
	// Try to initialise performance timer, will use low-resolution timer on failure
	m_HighRes = (QueryPerformanceFrequency( &m_HighResFreq ) != 0);

	// Reset and start the timer
	reset();
	m_Running = true;
}


//////////////////////////////
// Timer control

// Start the timer running
void Timer::start()
{
	if (!m_Running)
	{
		m_Running = true;

		// Get restart time - add time passed since stop time to the start and lap times
		// Select high or low-resolution timer
		if (m_HighRes)
		{
			LARGE_INTEGER newHighResTime;
			QueryPerformanceCounter( &newHighResTime );
			m_HighResStart.QuadPart += (newHighResTime.QuadPart - m_HighResStop.QuadPart);
			m_HighResLap.QuadPart += (newHighResTime.QuadPart - m_HighResStop.QuadPart);
		}
		else
		{
			DWORD newLowResTime;
			newLowResTime = timeGetTime();
			m_LowResStart += (newLowResTime - m_LowResStop);
			m_LowResLap += (newLowResTime - m_LowResStop);
		}
	}
}

// Stop the timer running
void Timer::stop()
{
	m_Running = false;

	// Get stop time
	// Select high or low-resolution timer
	if (m_HighRes)
	{
		QueryPerformanceCounter( &m_HighResStop );
	}
	else
	{
		m_LowResStop = timeGetTime();
	}
}

// Reset the timer to zero
void Timer::reset()
{
	// Reset start, lap and stop times to current time
	// Select high or low-resolution timer
	if (m_HighRes)
	{
		QueryPerformanceCounter( &m_HighResStart );
		m_HighResLap = m_HighResStart;
		m_HighResStop = m_HighResStart;
	}
	else
	{
		m_LowResStart = timeGetTime();
		m_LowResLap = m_LowResStart;
		m_LowResStop = m_LowResStart;
	}
}


//////////////////////////////
// Timing

// Get frequency of the timer being used (in counts per second)
float Timer::getFrequency()
{
	// Select high or low-resolution timer
	if (m_HighRes)
	{
		return static_cast<float>(m_HighResFreq.QuadPart);
	}
	else
	{
		return 1000.0f;
	}
}

// Get time passed (seconds) since since timer was started or last reset
float Timer::getTime()
{
	float fTime;
	if (m_HighRes)
	{

		LARGE_INTEGER newHighResTime;
		if (m_Running)
		{
			QueryPerformanceCounter( &newHighResTime );
		}
		else
		{
			newHighResTime = m_HighResStop;
		}
		double dTime = static_cast<double>(newHighResTime.QuadPart - m_HighResStart.QuadPart) /
			           static_cast<double>(m_HighResFreq.QuadPart);
		fTime = static_cast<float>(dTime);
	}
	else
	{
		DWORD newLowResTime;
		if (m_Running)
		{
			newLowResTime = timeGetTime();
		}
		else
		{
			newLowResTime = m_LowResStop;
		}
		fTime = static_cast<float>(newLowResTime - m_LowResStart) / 1000.0f;
	}

	return fTime;
}

// Get time passed (seconds) since last call to this function. If this is the first call, then
// the time since timer was started or the last reset is returned
float Timer::getLapTime()
{
	float fTime;

	if (m_HighRes)
	{
		LARGE_INTEGER newHighResTime;
		if (m_Running)
		{
			QueryPerformanceCounter( &newHighResTime );
		}
		else
		{
			newHighResTime = m_HighResStop;
		}
		double dTime = static_cast<double>(newHighResTime.QuadPart - m_HighResLap.QuadPart) /
			           static_cast<double>(m_HighResFreq.QuadPart);
		fTime = static_cast<float>(dTime);
		m_HighResLap = newHighResTime;
	}
	else
	{
		DWORD newLowResTime;
		if (m_Running)
		{
			newLowResTime = timeGetTime();
		}
		else
		{
			newLowResTime = m_LowResStop;
		}
		fTime = static_cast<float>(newLowResTime - m_LowResLap) / 1000.0f;
		m_LowResLap = newLowResTime;
	}
	return fTime;
}
