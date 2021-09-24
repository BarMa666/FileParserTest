#pragma once

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
    AppRetCode exec() noexcept;
#ifdef _TEST
    AppRetCode test();
#endif

};