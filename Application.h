#pragma once
#include "Primitives.h"
#include "SettingsManager.h"

#include <string>
#include <map>

class Application
{
public:
    Application() = default;
    AppRetCode exec() noexcept;
    // void selfTest();

private:
    AppRetCode oneQueueAlgorithm(FileQueueT&& _file_queues);
    AppRetCode multiQueueAlgorithm(std::vector<FileQueueT>&& _file_queues);

    Settings m_settings;
};