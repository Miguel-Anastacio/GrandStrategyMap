#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
namespace ALogger
{
    // Define the LogLevel enum to specify the severity of the log messages.
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    // Logger base class with virtual methods for logging messages at different severity levels.
    class Logger
    {
    protected:
        static std::string logLevelToString(LogLevel level);
        bool logOn = true;
        LogLevel minLogLevel = LogLevel::INFO;

    public:
        virtual ~Logger() {}
        virtual void Log(const std::string &message, LogLevel level, const char *file, int line) = 0;
        virtual void Log(const std::string &message, LogLevel level) = 0;
        void SetLogState(bool state);
        void SetMinLogLevel(LogLevel level);
#define LOG(logger, level, message) (logger).Log(message, level, __FILE__, __LINE__)
#define LOG_SIMPLE(logger, level, message) (logger).Log(message, level)
#define LOG_DEBUG(logger, message) LOG_SIMPLE(logger, ALogger::LogLevel::DEBUG, message)
#define LOG_INFO(logger, message) LOG_SIMPLE(logger, ALogger::LogLevel::INFO, message)
#define LOG_WARNING(logger, message) LOG_SIMPLE(logger, ALogger::LogLevel::WARNING, message)
#define LOG_ERROR(logger, message) LOG_SIMPLE(logger, ALogger::LogLevel::ERROR, message)
#define LOG_CRITICAL(logger, message) LOG_SIMPLE(logger, ALogger::LogLevel::CRITICAL, message)
    };

} // namespace ALogger