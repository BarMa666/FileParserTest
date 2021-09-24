#include "ParserManager.h"
#include "Logger.h"
#include"Parser.h"

#include <chrono>
#include <iostream>
#include <cassert>
#include <filesystem>

#define NO_LESS_THAN_TWO(x) (x == 0) ? 1 : x

ParserManager::ParserManager(unsigned _needed_workers, TextTemplateT&& _text_templates) noexcept
	: m_threads(correctWorkersCount(_needed_workers))
	, m_alive(true)
	, m_text_templates(std::move(_text_templates))
	, m_max_template_size(getMaxTemplateSize(m_text_templates))
{
	
}

ParserManager::~ParserManager() noexcept
{
	try
	{
		while (!m_queue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		m_alive = false;
		m_condition.notify_all();

		for (auto& thread : m_threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
}

void ParserManager::start() noexcept
{
	try
	{
		for (auto& thread : m_threads)
		{
			std::thread temp(&ParserManager::doWork, this);
			thread.swap(temp);
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
}

void ParserManager::put(std::string&& _file_name) noexcept
{
	try
	{
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_queue.emplace(std::move(_file_name));
		}
		m_condition.notify_one();
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
}

bool ParserManager::alive() const noexcept
{
	return m_alive.load();
}

unsigned ParserManager::correctWorkersCount(unsigned _needed_workers) noexcept
{
	unsigned result = 1;
	if (std::thread::hardware_concurrency() > 1)
	{
		result = std::min(std::thread::hardware_concurrency(), _needed_workers) - 1;
	}
	return result;
}

void ParserManager::doWork() noexcept
{
	Parser parser(m_max_template_size);
	while (alive())
	{
		try
		{
			std::unique_lock<std::mutex> lk(m_mutex);
			// wait new data or "death" signal(!alive())
			m_condition.wait(lk, [this] {return !(m_queue.empty() && alive()); });
			// check again if queue really empty
			if (!m_queue.empty())
			{
				auto _file_name = std::move(m_queue.front());
				m_queue.pop();
				lk.unlock();
				parser.parse(std::move(_file_name), m_text_templates);
			}
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
		}
	}
}

size_t ParserManager::getMaxTemplateSize(const TextTemplateT& _text_templates) noexcept
{
	size_t res = 0;
	try
	{
		for (const auto& [key, value] : _text_templates)
		{
			if (key.size() > res)
			{
				res = key.size();
			}
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
	return res;
}