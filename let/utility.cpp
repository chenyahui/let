#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "utility.h"

namespace let
{
std::string format_time(time_t rawtime, const std::string &format)
{
    char buffer[80];
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    return {buffer};
}

struct timeval milliseconds_to_timeval(long interval)
{
    struct timeval result {};
    result.tv_sec = interval / 100;
    result.tv_usec = (interval % 1000) * 1000;
    return result;
}

int64_t get_monotonic_milliseconds()
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) == 0)
    {
        return (int64_t)t.tv_sec * 1000 + (int64_t)t.tv_nsec / (1000 * 1000);
    }
    else
    {
        struct timeval t;
        memset(&t, 0, sizeof(t));
        gettimeofday(&t, NULL);
        return (int64_t)t.tv_sec * 1000 + (int64_t)t.tv_usec / 1000;
    }

    return -1;
}

time_t get_now_time_t()
{
    time_t raw_time;
    time(&raw_time);
    return raw_time;
}

void sleep_ms(int64_t milliseconds)
{
    std::chrono::milliseconds timespan(milliseconds); // or whatever

    std::this_thread::sleep_for(timespan);
}
} // namespace let