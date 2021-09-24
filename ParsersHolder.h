#pragma once

#include "Parser.h"

#include <vector>

class ParsersHolder
{
public:

private:
	std::vector<std::thread> m_threads;
};

