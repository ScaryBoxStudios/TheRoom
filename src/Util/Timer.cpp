#include "Timer.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#endif

// --------------------------------------------------
// Helpers
// --------------------------------------------------
// Retrieves current time with high resolution
static long long GetTimeNow();

// OS specific implementations of GetTimeNow()
#if defined(_WIN32) || defined(_WIN64)
static long long WinGetTimeNow();
#else
static long long UnixGetTimeNow();
#endif

// --------------------------------------------------
// Header implementations
// --------------------------------------------------
Timer::Timer() : mStartTime(0), mStarted(false)
{
}

void Timer::Start()
{
    mStartTime = GetTimeNow();
    mStarted = true;
}

long long Timer::Now()
{
    return (mStarted) ? static_cast<long long>(GetTimeNow() - mStartTime) : 0;
}

long long Timer::Stop()
{
    // Get timer value right now
    long long timeNow = Now();

    // Reset state
    mStarted = false;
    mStartTime = 0;

    // Return time right now
    return timeNow;
}

// --------------------------------------------------
// Helpers
// --------------------------------------------------
static long long GetTimeNow()
{
#if defined(_WIN32) || defined(_WIN64)
    return WinGetTimeNow();
#else
    return UnixGetTimeNow();
#endif
}

#if defined(_WIN32) || defined(_WIN64)
static long long WinGetTimeNow()
{
    LARGE_INTEGER time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&time);

    long long usec = time.QuadPart * 1000000;
    usec /= frequency.QuadPart;
    return usec;
}
#else
static long long UnixGetTimeNow()
{
    // TODO: Implement unix function to retrieve high resolution time
    return 0;
}
#endif
