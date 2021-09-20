#pragma once

#include "Primitives.h"

class ParserManager
{
public:
    ParserManager() = default;
    // void selfTest();

    void init(TextTemplatesT&& _templates);
private:
    TextTemplatesT m_templates;
};

