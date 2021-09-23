#include "SettingsManager.h"
#include "Logger.h"

#include <cassert>
#include <filesystem>

// Имя файла конфига настроек
static const std::string CONFIG_FILENAME = "settings.conf";

static const std::string THREADS_COUNT_SETTING_NAME = "ParserThreadCount";
static const std::string ROOT_DIRECTORY_SETTING_NAME = "RootDirectory";
static const std::string TEXT_TEMPLATE_SETTING_NAME = "TextTemplates";

static const unsigned DEFAULT_THREADS_COUNT = 1;

static const char SETITNGS_DELIMITER = L':';
static const char TEXT_TEMPLATES_DELIMITER = L',';

void SettingsManager::printSetting(const std::string& setting_name, const std::string& setting_value)
{
	std::cout << setting_name << ": " << setting_value << "\n";
}

void SettingsManager::removeSpaces(OUT std::string& _string)
{
	_string.erase(std::remove_if(_string.begin(), _string.end(), [](const auto& _ch) {return std::isspace(_ch); }), _string.end());
}

bool SettingsManager::hasEmptySettings(const Settings& _settings)
{
	return _settings.text_templates.empty() || _settings.root.empty() || (_settings.threads_count == 0);
}

unsigned SettingsManager::readThreadCount(const std::string& setting_value)
{
	try
	{
		return static_cast<unsigned>(std::stoul(setting_value));
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return DEFAULT_THREADS_COUNT;
}

TextTemplateT SettingsManager::readTextTemplates(std::string setting_value)
{
	size_t pos = 0;
	TextTemplateT result;
	std::string text_template_setting;
	std::string text_template;
	std::string text_substitute;
	while (!setting_value.empty()) 
	{
		pos = setting_value.find(TEXT_TEMPLATES_DELIMITER);
		text_template_setting = setting_value.substr(0, pos);
		assert(!text_template_setting.empty());
		text_template = text_template_setting.substr(0, text_template_setting.find(SETITNGS_DELIMITER));
		assert(!text_template.empty());
		text_substitute = text_template_setting.substr(text_template_setting.find(SETITNGS_DELIMITER) + 1);
		assert(!text_substitute.empty());

#ifdef _DEBUG
		printSetting(text_template, text_substitute);
#endif

		result[text_template] = text_substitute;
		if (pos == std::string::npos)
		{
			pos = setting_value.size();
		}
		setting_value.erase(0, pos + 1);
	}

	return result;
}

Settings SettingsManager::readSettings()
{
	std::filesystem::path path = std::filesystem::current_path();
#ifdef _DEBUG
	FileHolder settings_file(path.string() + "/" + CONFIG_FILENAME, std::ios_base::in);
#else
	FileHolder settings_file(path.string() + "/" + CONFIG_FILENAME, std::ios_base::in);
#endif
	std::string setting_line;
	Settings settings = { DEFAULT_THREADS_COUNT };
	if (settings_file.is_open())
	{
		while (std::getline(settings_file, setting_line))
		{
			std::string setting_name = setting_line.substr(0, setting_line.find(SETITNGS_DELIMITER));
			removeSpaces(setting_name);
			assert(!setting_name.empty());
			std::string setting_value = setting_line.substr(setting_line.find(SETITNGS_DELIMITER) + 1);
			removeSpaces(setting_value);
			assert(!setting_value.empty());
			if (setting_name == THREADS_COUNT_SETTING_NAME)
			{
				settings.threads_count = readThreadCount(setting_value);
			}
			else if (setting_name == ROOT_DIRECTORY_SETTING_NAME)
			{
				settings.root = setting_value;
			}
			else if (setting_name == TEXT_TEMPLATE_SETTING_NAME)
			{
				settings.text_templates = readTextTemplates(setting_value);
			}
			else
			{
				Logger::logError("WARNING: Unexpected setting\n");
			}
#ifdef _DEBUG
			printSetting(setting_name, setting_value);
#endif
		}
		settings_file.close();
	}

	return settings;
}