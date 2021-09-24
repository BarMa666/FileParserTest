#include "Application.h"
#include "ParserManager.h"
#include "SettingsManager.h"
#include "Logger.h"

#ifdef _TEST
#include "Parser.h"
#endif

#include <iostream>
#include <cassert>
#include <filesystem>
#include <chrono>

#define NO_LESS_THAN_ONE(x) (x == 0) ? 1 : x

Application::AppRetCode Application::exec() noexcept
{
	AppRetCode ret = AppRetCode::Ok;
	try
	{
		SettingsManager settings_manager;
		auto settings = settings_manager.readSettings();
		if (settings_manager.hasEmptySettings(settings))
		{
			return AppRetCode::NoSettings;
		}
		{
			ParserManager parser(settings.threads_count, std::move(settings.text_templates));
			parser.start();
#ifdef _DEBUG
			auto start = std::chrono::steady_clock::now();
#endif
			for (const auto& file_name : std::filesystem::recursive_directory_iterator(settings.root))
			{
				parser.put(std::move(file_name.path().string()));
			}
#ifdef _DEBUG
			auto end = std::chrono::steady_clock::now();
			std::cout << (end - start).count() << "\n";
#endif
		}
		std::cout << "All done!\n";
#ifndef _DEBUG
		std::cin.get();
#endif
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
		ret = AppRetCode::Error;																	
	}
	return ret;
}

#ifdef _TEST
Application::AppRetCode Application::test()
{
	SettingsManager settings_manager;
	auto settings = settings_manager.readSettings();
	assert(!settings_manager.hasEmptySettings(settings));
	auto text_templates = settings.text_templates;
	Parser parser(std::move(ParserManager::getMaxTemplateSize(text_templates)));
	parser.testParse();
	return AppRetCode::Ok;
}
#endif
