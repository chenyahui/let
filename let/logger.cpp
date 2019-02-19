//
// Created by yahuichen on 2019/1/23.
//

#include <iostream>
#include <boost/format.hpp>
#include <cstdlib>

#include "logger.h"
#include "util.h"

namespace let
{
LogLevel Logger::g_log_level = DEBUG;

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

    auto format_msg =
        boost::format("%s [%s:%s:%s] [%s] %s") % now_time_str() % file_name_ % func_name_ % line_ % level_str % msg;

    if (level_ > LogLevel::INFO)
    {
        std::cerr << format_msg << std::endl;
    }
    else
    {
        std::cout << format_msg << std::endl;
    }

    if (level_ == FATAL)
    {
        exit(-1);
    }
}
} // namespace let