#pragma once
#include "Primitives.h"
#include "MultiParserWorker.h"

#include <vector>
#include <condition_variable>

class ParserManager
{
public:
	ParserManager(unsigned _needed_workers);
	AppRetCode Parse(FileQueueT&& _files, const TextTemplateT& _text_templates);
	AppRetCode ParseMulti(std::vector<FileQueueT>&& _files, const TextTemplateT& _text_templates);
private:
	const unsigned MAX_WORKERS_COUNT;
	std::condition_variable m_condition;
};

