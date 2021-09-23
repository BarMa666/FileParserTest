#include "ParserManager.h"
#include "MultiParserWorker.h"
#include "ParserWorker.h"
#include "Logger.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <cassert>
#include <list>

// нет смысла создавать потоков больше чем ядер процессора
ParserManager::ParserManager(unsigned _needed_workers)
	: MAX_WORKERS_COUNT(std::min(std::thread::hardware_concurrency(), _needed_workers) - 1) // оставим один текущий поток
{
}

AppRetCode ParserManager::Parse(FileQueueT&& _files, const TextTemplateT& _text_templates)
{
	AppRetCode ret = AppRetCode::Ok;
	assert(MAX_WORKERS_COUNT < std::thread::hardware_concurrency());
	try
	{
		ParserWorker parser(std::move(_files), _text_templates, MAX_WORKERS_COUNT);
		parser.start();
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
		ret = AppRetCode::Error;
	}
	return ret;
}

AppRetCode ParserManager::ParseMulti(std::vector<FileQueueT>&& _fileQueues, const TextTemplateT& _text_templates)
{
	AppRetCode ret = AppRetCode::Ok;
	assert(MAX_WORKERS_COUNT < std::thread::hardware_concurrency());
	try
	{
		std::list<MultiParserWorker> waiters;
		for (size_t i = 0; i < _fileQueues.size(); ++i)
		{
			MultiParserWorker worker(_text_templates);
			waiters.emplace_back(std::move(worker));
			waiters.back().run(std::move(_fileQueues[i]), _text_templates);
		}
		while(waiters.size() > 0)
		{
			for (auto worker_iter = waiters.cbegin(); worker_iter != waiters.cend();)
			{
				if (worker_iter->isDone())
				{
					worker_iter = waiters.erase(worker_iter);
				}
				else
				{
					++worker_iter;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
		ret = AppRetCode::Error;
	}
	return ret;
}
