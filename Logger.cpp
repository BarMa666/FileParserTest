#include "Logger.h"

#include <chrono>
#include <ctime> 

static const std::string LOG_FILENAME = "log.log";

Logger::Logger() : m_file(LOG_FILENAME, std::ios::app | std::ios::out)
{
}

Logger::~Logger()
{
	std::lock_guard<std::mutex> lock(m_mu);
	if (m_file.is_open())
	{
		m_file.close();
	}
}

Logger& Logger::instance()
{
	static Logger logger;
	return logger;
}

void Logger::logError(const std::string& _error)
{
	Logger::instance().log(_error, Type::Error);
}

void Logger::logInfo(const std::string& _error)
{
	Logger::instance().log(_error, Type::Info);
}

void Logger::log(const std::string& _error, Type _type)
{
	std::lock_guard<std::mutex> lock(m_mu);
	if (m_file.is_open())
	{
		std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		m_file << std::ctime(&current_time) << ": " << _error << "\n";
	}
}
