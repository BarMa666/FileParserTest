#include "IWorker.h"
#include "Logger.h"

#include <filesystem>

IWorker::IWorker(const TextTemplateT& _text_templates) noexcept
	: MAX_TEMPLATE_LEN(getMaxTemplateSize(_text_templates))
{
}

IWorker::IWorker(IWorker&& _other)
	: MAX_TEMPLATE_LEN(_other.MAX_TEMPLATE_LEN)
{
}

void IWorker::ParseFile(const std::string& _file_name, const std::string& _temp_name, const TextTemplateT& _text_templates) const noexcept
{
	FileHolder file(_file_name, std::ios_base::in);
	FileHolder file_out(_temp_name, std::ios_base::out);
	if (file.is_open() && file_out.is_open())
	{
		std::string line;
		try
		{
			while (std::getline(file, line, '\0'))
			{
				for (const auto& [_template, _substitutor] : _text_templates)
				{
					replaceTemplates(line, _template, _substitutor);
				}
				file_out << line;
				file.seekg(file.tellg() - std::streamoff(MAX_TEMPLATE_LEN));
				file_out.seekp(file_out.tellp() - std::streamoff(MAX_TEMPLATE_LEN));
			}
#ifdef _DEBUG
			if (file.bad())
			{
				Logger::logError("file is bad");
			}
			if (file.fail())
			{
				Logger::logError("file failed");
			}
#endif
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
		}
	}
}

void IWorker::replaceTemplates(OUT std::string& _line, const std::string& _template, const std::string& _substitutor) const noexcept
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
		}
	}
}

// найдет максимальную длину шаблона, чтобы откатиться "назад" в файле и не пропустить "на стыке" обрабатываемых данных
size_t IWorker::getMaxTemplateSize(const TextTemplateT& _text_templates) const noexcept
{
	size_t res = 0;
	try
	{
		for (const auto& [key, value] : _text_templates)
		{
			if (key.size() > res)
			{
				res = key.size();
			}
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return res;
}

void IWorker::doWork(std::string&& _file_name, const TextTemplateT& _text_templates) noexcept
{
	try
	{
		std::string temp_filename = _file_name.substr(0, _file_name.find_last_of('.')) + "_temp" + _file_name.substr(_file_name.find_last_of('.'));
		ParseFile(_file_name, temp_filename, _text_templates);
		if (!std::filesystem::remove(_file_name))
		{
			std::cout << _file_name << " fail to remove\n";
		}
		std::filesystem::rename(temp_filename, _file_name);
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
#ifdef _DEBUG
	// Logger::logInfo(_file_name + " done!\n");
#endif
}