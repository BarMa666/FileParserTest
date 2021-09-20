#pragma once

#include <string>

class FileManager
{
public:
    FileManager() = default;
    // void selfTest();

    void init(std::wstring&& _root);
private:
    std::wstring m_root;
};

