#include "FileManager.h"
#include "Logger.h"

#include <filesystem>
#include <iostream>
#include <vector>

FileManager::FileManager(const std::string& _root) noexcept
    : m_root(_root)
{
}

FileQueueT FileManager::getFiles() const noexcept
{
    FileQueueT result;
    try
    {
        for (const auto& file_name : std::filesystem::recursive_directory_iterator(m_root))
        {
            result.push_back(file_name.path().string());
#ifdef _DEBUG
            std::cout << file_name.path() << "\n";
#endif
        }
    }
    catch (const std::exception& _ex)
    {
        Logger::logError(_ex.what());
    }
    return result;
}

std::vector<FileQueueT> FileManager::getFileQueues(unsigned _threads_count) const noexcept
{
    std::vector<FileQueueT> result(_threads_count);
    try
    {
        uintmax_t hole_size = getFilesSize();
        unsigned num = 0;
        uintmax_t limit = hole_size / _threads_count;
        uintmax_t current_size = 0;
        for (const auto& file_name : std::filesystem::recursive_directory_iterator(m_root))
        {
            current_size += file_name.file_size();
            result[num].push_back(file_name.path().string());
            if (current_size > limit)
            {
                ++num;
                current_size = 0;
            }
        }
    }
    catch (const std::exception& _ex)
    {
        Logger::logError(_ex.what());
    }
    return result;
}

uintmax_t FileManager::getFilesSize() const noexcept
{
    uintmax_t hole_size = 0;
    try
    {
        for (const auto& file_name : std::filesystem::recursive_directory_iterator(m_root))
        {
            hole_size += file_name.file_size();
        }
    }
    catch (const std::exception& _ex)
    {
        Logger::logError(_ex.what());
    }
    return hole_size;
}
