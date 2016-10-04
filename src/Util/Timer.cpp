#include "Timer.hpp"
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#endif

/* -------------------------------------------------- */
/* Helpers                                            */
/* -------------------------------------------------- */
#if defined(_WIN32) || defined(_WIN64)
static long long win_get_time_now();
#else
static long long unix_get_time_now();
#endif

static long long get_time_now();

/* -------------------------------------------------- */
/* Header implementations                             */
/* -------------------------------------------------- */
struct timer_data
{
    long long start_time = 0;
    int started = 0;
};

struct timer_data* timer_create()
{
    return (struct timer_data*) malloc(sizeof(struct timer_data));
}

void timer_start(struct timer_data* data)
{
    data->start_time = get_time_now();
    data->started = 1;
}

long long timer_now(struct timer_data* data)
{
    return (data->started) ? (long long) (get_time_now() - data->start_time) : 0;
}

long long timer_stop(struct timer_data* data)
{
    long long value = timer_now(data);
    data->started = 0;
    return value;
}

void timer_destroy(struct timer_data* data)
{
    free(data);
}

/* -------------------------------------------------- */
/* Helpers                                            */
/* -------------------------------------------------- */
#if defined(_WIN32) || defined(_WIN64)
static long long win_get_time_now()
{
    LARGE_INTEGER time, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&time);

    long long usec = time.QuadPart * 1000000;
    usec /= frequency.QuadPart;
    return usec;
}
#else
static long long unix_get_time_now()
{
    /* TODO: Implement unix function to retrieve high resolution time */
    return 0;
}
#endif

static long long get_time_now()
{
#if defined(_WIN32) || defined(_WIN64)
    return win_get_time_now();
#else
    return unix_get_time_now();
#endif
}
