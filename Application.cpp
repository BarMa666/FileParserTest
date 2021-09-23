#include "Application.h"
#include "FileManager.h"
#include "ParserManager.h"
#include "Logger.h"

#include <iostream>
#include <cassert>
#include <filesystem>
#include <chrono>

#define NO_LESS_THAN_ONE(x) (x == 0) ? 1 : x

static SettingsManager g_settings_manager;

AppRetCode Application::exec() noexcept
{
	AppRetCode ret = AppRetCode::Ok;
	try
	{
		m_settings = g_settings_manager.readSettings();
		if (g_settings_manager.hasEmptySettings(m_settings))
		{
			return AppRetCode::NoSettings;
		}
		FileManager file_manager(m_settings.root);
		[[maybe_unused]] auto files_size = file_manager.getFilesSize();
#ifdef _TEST_ONE
		oneQueueAlgorithm(std::move(file_manager.getFiles()));
#else
		static const unsigned MAX_WORKERS_COUNT(NO_LESS_THAN_ONE(std::min(std::thread::hardware_concurrency(), m_settings.threads_count)) - 1);
		multiQueueAlgorithm(std::move(file_manager.getFileQueues(MAX_WORKERS_COUNT)));
#endif
		std::cout << "All done!\n";
		std::cin.get();
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
		ret = AppRetCode::Error;																	
	}
	return ret;
}

AppRetCode Application::oneQueueAlgorithm(FileQueueT&& _file_queues)
{
	AppRetCode ret = AppRetCode::Ok;
	try
	{
#ifdef _DEBUG
		auto start = std::chrono::steady_clock::now();
#endif
		ParserManager parser(m_settings.threads_count);
		ret = parser.Parse(std::move(_file_queues), m_settings.text_templates);
#ifdef _DEBUG
		auto end = std::chrono::steady_clock::now();
		std::cout << (end - start).count() << "\n";
#endif
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return ret;
}

AppRetCode Application::multiQueueAlgorithm(std::vector<FileQueueT>&& _file_queues)
{
	AppRetCode ret = AppRetCode::Ok;
	try
	{
#ifdef _DEBUG
		auto start = std::chrono::steady_clock::now();
#endif
		ParserManager parser(m_settings.threads_count);
		ret = parser.ParseMulti(std::move(_file_queues), m_settings.text_templates);
#ifdef _DEBUG
		auto end = std::chrono::steady_clock::now();
		std::cout << (end - start).count() << "\n";
#endif
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return ret;
}
