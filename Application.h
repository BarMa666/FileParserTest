#pragma once
#include "SettingsManager.h"

class Application
{
public:
    enum AppRetCode
    {
        Ok,
        Error,
        NoSettings
    };

    Application() = default;
    AppRetCode exec(bool _reversed_settings = false) noexcept;
#ifdef _TEST
    AppRetCode test();
#endif
private:
    AppRetCode readSettigns(Settings& _settings, bool _reversed = false) const noexcept;
    void compare() const;
    void compare(const std::string& _lhs, const std::string& _rhs) const;
};