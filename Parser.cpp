#include "SettingsManager.h"
#include "Parser.h"
#include "Logger.h"
#include <filesystem>
#include <cassert>
#include <iostream>

Parser::Parser(size_t _max_template_size) noexcept
	:m_max_template_size(_max_template_size)
{
}

Parser::~Parser() noexcept
{
}

// идея теста такова: 
// если после парсинга файла его прогнать парсером в обратном направлении,
// то должен получится изначальный файл
// @REFME выделить часть логики в отдельные функции
void Parser::testParse() const
{
	static const std::string befor_extension = ".before";
	TextTemplateT templates = loadTestTemplates();
	// key is a file before parsing and value is already parsed file
	std::list<std::string> file_names = loadTestFileNames();
	
	// copy files to save state before parsing
	for (const auto& name : file_names)
	{
		std::string before_filename = name.substr(0, name.find_last_of('.')) + befor_extension;
		parseFile(name, before_filename, {});
	}
	
	// parse files
	for (auto&& name : file_names)
	{
		parse(std::move(name), templates);
	}

	// reverse templates
	TextTemplateT reversed_templates;
	for (const auto& [key, value] : templates)
	{
		reversed_templates[value] = key;
	}

	// check parsing
	for (auto&& name : file_names)
	{
		parse(std::move(name), reversed_templates);
	}

	// compare
	assert(compare(file_names, befor_extension));
	std::cout << "Parse test past good\n";
}

void Parser::parse(std::string&& _file_name, const TextTemplateT& _text_templates) const noexcept
{
	try
	{
		std::string temp_filename = _file_name.substr(0, _file_name.find_last_of('.')) + "_temp" + _file_name.substr(_file_name.find_last_of('.'));
		if (parseFile(_file_name, temp_filename, _text_templates))
		{
			if (!std::filesystem::remove(_file_name))
			{
				std::cout << _file_name << " fail to remove\n";
			}
			std::filesystem::rename(temp_filename, _file_name);
		}
		else
		{
			std::filesystem::rename(temp_filename, temp_filename + "[parsing failed]");
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
#ifdef _DEBUG
	Logger::logInfo(_file_name + " done!\n");
#endif
}

bool Parser::parseFile(const std::string& _file_name, const std::string& _temp_name, const TextTemplateT& _text_templates) const noexcept
{
	FileHolder file(_file_name, std::ios_base::in);
	FileHolder file_out(_temp_name, std::ios_base::out);
	if (file.is_open() && file_out.is_open())
	{
		file.seekg(0, std::ios_base::end);
		auto size = file.tellg();
		file.seekg(0, std::ios_base::beg);
		std::string line;
		try
		{
			while (std::getline(file, line, '\0'))
			{
#ifdef _DEBUG
				std::cout << line << "\n";
#endif
				for (const auto& [_template, _substitutor] : _text_templates)
				{
					if (!replaceTemplates(line, _template, _substitutor))
					{
						return false;
					}
				}
				file_out << line;
				file.seekg(file.tellg() - std::streamoff(m_max_template_size));
				file_out.seekp(file_out.tellp() - std::streamoff(m_max_template_size));
			}
#ifdef _DEBUG
			if (file.bad())
			{
				Logger::logError("file is bad");
			}
			if (file.fail())
			{
				Logger::logError("file is fail");
				file.clear();
			}
#endif
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
			return false;
		}
	}
	return true;
}

bool Parser::replaceTemplates(OUT std::string& _line, const std::string& _template, const std::string& _substitutor) const noexcept
{
	size_t idx = 0;
	while (true)
	{
		try
		{
			idx = _line.find(_template, idx);
			if (idx == std::string::npos)
			{
				break;
			}
			_line.replace(idx, _template.size(), _substitutor);
			idx += _substitutor.size();
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
			return false;
		}
	}
	return true;
}

TextTemplateT Parser::loadTestTemplates() const noexcept
{
	TextTemplateT res;
	FileHolder test_settings("parser_test_settings.conf");
	if (test_settings.is_open())
	{
		std::string line;
		std::string templates_string;
		while (std::getline(test_settings, line))
		{
			templates_string += line;
		}
		SettingsManager settings_manager;
		res = settings_manager.readTextTemplates(templates_string);
	}
	return res;
}

std::list<std::string> Parser::loadTestFileNames() const noexcept
{
	FileHolder test_settings("parser_test_filenames.conf");
	if (test_settings.is_open())
	{
		std::string line;
		std::list<std::string> filenames_string;
		while (std::getline(test_settings, line))
		{
			filenames_string.emplace_back(std::move(line));
		}
		return filenames_string;
	}
	return std::list<std::string>();
}

bool Parser::compare(const std::list<std::string>& _file_names, const std::string& _befor_extension) const noexcept
{
	for (const auto& name : _file_names)
	{
		FileHolder file(name);
		std::string before_filename = name.substr(0, name.find_last_of('.')) + _befor_extension;
		FileHolder before_file(before_filename);
		std::string data;
		std::string before_data;
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
		if (before_data != data)
		{
			return false;
		}
	}
	return true;
}
