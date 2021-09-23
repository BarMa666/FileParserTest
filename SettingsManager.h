#pragma once
#include "Primitives.h"

#include <string>

struct Settings
{
    unsigned threads_count;
    std::string root;
    TextTemplateT text_templates;
};

class SettingsManager
{
public:

    Settings readSettings();
    unsigned readThreadCount(const std::string& setting_value);
    TextTemplateT readTextTemplates(std::string setting_value);
    bool hasEmptySettings(const Settings& _settings);

private:
    void removeSpaces(OUT std::string& _string);
    void printSetting(const std::string& setting_name, const std::string& setting_value);
};

