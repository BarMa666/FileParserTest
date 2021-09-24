#include "Application.h"
#include "ParserManager.h"
#include "Logger.h"

#include <iostream>
#include <cassert>
#include <filesystem>
#include <chrono>

#define NO_LESS_THAN_ONE(x) (x == 0) ? 1 : x

Application::AppRetCode Application::exec(bool _reversed_settings) noexcept
{
	AppRetCode ret = AppRetCode::Ok;
	try
	{
		{
			Settings settings;
			ret = readSettigns(settings, _reversed_settings);
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
	AppRetCode ret = AppRetCode::Ok;
	ret = exec();
	if (ret != AppRetCode::Ok)
	{
		return ret;
	}
	ret = exec(true);
	if (ret != AppRetCode::Ok)
	{
		return ret;
	}
	compare();
	return ret;
}
#endif

void Application::compare() const
{
	Settings settings;
	readSettigns(settings);
	std::map<std::string, std::string> filenames;
	for (const auto& file_name : std::filesystem::recursive_directory_iterator(settings.root))
	{
		const auto& filename = file_name.path().string();
		if (filename.find(TEST_POSTFIX) == std::string::npos)
		{
			filenames[filename] = filename.substr(0, filename.find_last_of('.')) + TEST_POSTFIX + filename.substr(filename.find_last_of('.'));
		}
	}
	for (const auto& [after, before] : filenames)
	{
		compare(before, after);
	}
}

void Application::compare(const std::string& _lhs, const std::string& _rhs) const
{
	FileHolder before_file(_lhs);
	FileHolder file(_rhs);

	std::string before_data;
	std::string data;
	std::string line;

	while (std::getline(file, line, '\0'))
	{
		data += line + "\n";
	}
	while (std::getline(before_file, line, '\0'))
	{
		before_data += line + "\n";
	}
	assert(!data.empty());
	assert(before_data == data);
}

Application::AppRetCode Application::readSettigns(Settings& _settings, bool _reversed) const noexcept
{
	try
	{
		SettingsManager settings_manager;
		_settings = settings_manager.readSettings();
		if (_reversed)
		{
			// reverse templates
			TextTemplateT reversed_templates;
			for (const auto& [key, value] : _settings.text_templates)
			{
				reversed_templates[value] = key;
			}
			_settings.text_templates = reversed_templates;
		}
		if (settings_manager.hasEmptySettings(_settings))
		{
			return AppRetCode::NoSettings;
		}
		return AppRetCode::Ok;
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return AppRetCode::Error;
}