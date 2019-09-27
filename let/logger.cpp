//
// Created by yahuichen on 2019/1/23.
//

#include <iostream>
#include <cstdlib>

#include "logger.h"
#include "utility.h"

namespace let
{
LogLevel Logger::g_log_level = VERBOSE;

Logger::Logger(const std::string &file_name, const std::string &func_name, int line, LogLevel level)
    : file_name_(file_name),
      line_(line),
      func_name_(func_name),
      level_(level)
{
}

void Logger::setLogLevel(let::LogLevel level)
{
    g_log_level = level;
}

Logger::~Logger()
{
    if (level_ < g_log_level)
    {
        return;
    }

    std::string msg = body_.str();
    std::string level_str = "INFO";

    switch (level_)
    {
    case VERBOSE:
        level_str = "VERBOSE";
        break;
    case DEBUG:
        level_str = "DEBUG";
        break;
    case INFO:
        level_str = "INFO";
        break;
    case WARN:
        level_str = "WARN";
        break;
    case ERROR:
        level_str = "ERROR";
        break;
    case FATAL:
    {
        level_str = "FATAL";
        break;
    }
    }

    auto now_time = get_now_time_t();
    std::string now_time_str = format_time(now_time, "%Y-%m-%d %H:%M:%S");

    char *format_msg = new char[now_time_str.size() + file_name_.size() + func_name_.size() + level_str.size() + msg.size() + 50];

    sprintf(format_msg, "%s [%s:%s:%d] [%s] %s",
            now_time_str.c_str(),
            file_name_.c_str(),
            func_name_.c_str(),
            line_,
            level_str.c_str(),
            msg.c_str());

    if (level_ > LogLevel::INFO)
    {
        std::cerr << format_msg << std::endl;
    }
    else
    {
        std::cout << format_msg << std::endl;
    }

    delete[] format_msg;

    if (level_ == FATAL)
    {
        exit(-1);
    }
}
} // namespace let