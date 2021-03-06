#pragma once

#include <string>
#include <map>
#include <deque>
#include <fstream>

#ifndef OUT
#define OUT
#endif

enum AppRetCode
{
    Ok,
    Error,
	NoSettings
};

using TextTemplateT = std::map<std::string, std::string>;
using FileQueueT = std::deque<std::string>;

static const std::string TEST_POSTFIX = "_before";

class FileHolder : public std::fstream
{
public:
	explicit FileHolder(const std::string& _Str, ios_base::openmode _Mode = ios_base::in | ios_base::out) : std::fstream(_Str, _Mode) {}
	virtual ~FileHolder()
	{
		if (is_open())
		{
			close();
		}
	}
};