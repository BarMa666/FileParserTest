#pragma once

#include <string>
#include <memory>
#include <map>

#include "Primitives.h"

class FileManager;
class ParserManager;
class ThreadManager;

struct Settings
{
    unsigned threads_count;
    std::wstring root;
    TextTemplatesT text_templates;
};

class Application
{
public:
    enum class RetCode
    {
        Ok,
        Error
    };

    Application();
    RetCode init() noexcept;
    RetCode exec() noexcept;
    // void selfTest();

private:
    Settings readSettings();
    std::shared_ptr<FileManager> m_file_manager;
    std::shared_ptr<ParserManager> m_parser_manager;
    std::shared_ptr<ThreadManager> m_thread_manager;
};