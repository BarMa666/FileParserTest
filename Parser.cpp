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

void Parser::parse(std::string&& _file_name, const TextTemplateT& _text_templates) const noexcept
{
	try
	{
#ifdef _TEST
		if (_file_name.find(TEST_POSTFIX) != std::string::npos)
		{	
			return;
		}
		else
		{
#endif
			std::string temp_filename = _file_name.substr(0, _file_name.find_last_of('.')) + "_temp" + _file_name.substr(_file_name.find_last_of('.'));
			if (parseFile(_file_name, temp_filename, _text_templates))
			{
#ifdef _TEST
				std::string before_filename = _file_name.substr(0, _file_name.find_last_of('.')) + TEST_POSTFIX + _file_name.substr(_file_name.find_last_of('.'));
				if (std::filesystem::exists(before_filename))
				{
#endif
					if (!std::filesystem::remove(_file_name))
					{
						std::cout << _file_name << " fail to remove\n";
					}
					std::filesystem::rename(temp_filename, _file_name);
#ifdef _TEST
				}
				else
				{
					std::filesystem::rename(_file_name, before_filename);
					std::filesystem::rename(temp_filename, _file_name);
				}
#endif
			}
			else
			{
				std::filesystem::rename(temp_filename, temp_filename + "[parsing failed]");
			}
#ifdef _TEST
		}
#endif
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