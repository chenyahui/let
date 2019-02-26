//
// Created by yahuichen on 2019/1/23.
//

#include "util.h"

namespace let
{
std::string format_time(time_t rawtime, const std::string &format)
{
    char buffer[80];
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    return {buffer};
}

std::string format_now_time(const std::string &format)
{
    time_t rawtime;
    time(&rawtime);

    return format_time(rawtime, format);
}
} // namespace let