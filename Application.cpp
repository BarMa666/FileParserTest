#include "Application.h"

#include "FileManager.h"
#include "ThreadManager.h"
#include "ParserManager.h"

#include <iostream>

// Имя файла конфига настроек
static const std::wstring CONFIG_FILENAME = L"settings.conf";

Application::Application()
	: m_file_manager(std::make_shared<FileManager>())
	, m_thread_manager(std::make_shared<ThreadManager>())
	, m_parser_manager(std::make_shared<ParserManager>())
{
}

Application::RetCode Application::init() noexcept
{
	RetCode ret = RetCode::Ok;
	try
	{
		Settings settings = readSettings();
		m_file_manager->init(std::forward<std::wstring>(settings.root));
		m_thread_manager->init(std::forward<unsigned>(settings.threads_count));
		m_parser_manager->init(std::forward<TextTemplatesT>(settings.text_templates));
	}
	catch (const std::exception& _ex)
	{
		std::cerr << _ex.what();
	}
	return ret;
}

Application::RetCode Application::exec() noexcept
{
	RetCode ret = RetCode::Ok;
	try
	{
		// Считаем общий размер файлов в корневой директории(FileManager)
		// auto files_size = m_file_manager->getAllFileSize();
		// Считаем размер пула потоков(ThreadManager)
		// auto thread_count = m_thread_manager->getThreadsCount();
		// Формируем части для обработки (ParserManager)
			// Частей не больше чем потоков, размер части зависит от кол-ва файлов и их общего размера
			// auto maximum_part_size = m_parser_manager->calculatePartsCount(files_size, thread_count)
		//  
			
		// Для каждой части запускаем свой поток (ThreadManager)
	}
	catch (const std::exception& _ex)
	{
		std::cerr << _ex.what();
	}
	return ret;
}

Settings Application::readSettings()
{
	// read data from config file
}
