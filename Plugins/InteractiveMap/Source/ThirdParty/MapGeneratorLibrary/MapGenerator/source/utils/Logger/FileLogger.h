#pragma once
#include "Logger.h"
#include <filesystem>

namespace ALogger
{
	class FileLogger : public Logger
	{
	private:
		std::ofstream logFile;

	public:
		FileLogger();
		FileLogger(const std::filesystem::path& filePath);
		~FileLogger();
		virtual void Log(const std::string& message, ALogger::LogLevel level, const char *file, int line) override;
		virtual void Log(const std::string& message, ALogger::LogLevel level) override;
	};
}