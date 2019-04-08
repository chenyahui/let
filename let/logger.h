//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_LOGGER_H
#define LET_LOGGER_H

#include <sstream>

namespace let
{
enum LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

class Logger
{
  public:
    Logger(const std::string &file_name, const std::string &func_name, int line, LogLevel level);

    ~Logger();

    template <class T>
    Logger &operator<<(const T &val)
    {
        body_ << val;
        return *this;
    }

    static void setLogLevel(LogLevel level);

  private:
    std::stringstream body_;
    LogLevel level_;

    std::string file_name_;
    std::string func_name_;
    int line_;

  private:
    static LogLevel g_log_level;
};
} // namespace let

#define LOG(level) (::let::Logger(__FILE__, __FUNCTION__, __LINE__, level))
#define LOG_DEBUG LOG(::let::LogLevel::DEBUG)
#define LOG_INFO LOG(::let::LogLevel::INFO)
#define LOG_WARN LOG(::let::LogLevel::WARN)
#define LOG_ERROR LOG(::let::LogLevel::ERROR)
#define LOG_FATAL LOG(::let::LogLevel::FATAL)
#endif //LET_LOGGER_H
