//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_UTILITY_H
#define LET_UTILITY_H

#include <string>
#include <sys/time.h>
#include <cstdint>
#include <utility>

namespace let
{
struct timeval milliseconds_to_timeval(long interval);

int64_t get_monotonic_milliseconds();

time_t get_now_time_t();

std::string format_time(time_t, const std::string &format);

void sleep_ms(int64_t milliseconds);
} // namespace let
#endif //LET_UTILITY_H
