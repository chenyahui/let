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

#define LOG(level) (Logger(__FILE__, __FUNCTION__, __LINE__, level))
#define LOG_DEBUG LOG(DEBUG)
#define LOG_INFO LOG(INFO)
#define LOG_WARN LOG(WARN)
#define LOG_ERROR LOG(ERROR)
#define LOG_FATAL LOG(FATAL)

} // namespace let

#endif //LET_LOGGER_H
