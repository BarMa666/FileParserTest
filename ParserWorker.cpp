#include "ParserWorker.h"
#include "Logger.h"

ParserWorker::ParserWorker(const TextTemplateT& _text_templates, size_t _threads_count) noexcept
	: ParserWorker(std::deque<std::string>(), _text_templates, _threads_count)
{
}

ParserWorker::ParserWorker(FileQueueT&& _file_queue, const TextTemplateT& _text_templates, size_t _threads_count) noexcept
	: IWorker(_text_templates)
	, m_threads(_threads_count)
	, m_alive(true)
	, m_data(_file_queue)
{
	run(std::move(_file_queue), _text_templates);
}

ParserWorker::~ParserWorker() noexcept
{
	try
	{
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

void ParserWorker::run(FileQueueT&& _file_queue, const TextTemplateT& _text_templates)
{
	try
	{
		for (auto& thread : m_threads)
		{
			std::thread temp(&ParserWorker::doWorkConditional, this, _text_templates);
			thread.swap(temp);
		}
		m_data = _file_queue;
	}
	catch (const std::exception& _ex)
	{
	}
}

bool ParserWorker::alive() const
{
	return m_alive.load();
}

void ParserWorker::doWorkConditional(const TextTemplateT& _text_templates) noexcept
{
	while (alive())
	{
		try
		{
			std::unique_lock<std::mutex> lk(m_mutex);
			// ожидание прекратится если во время уведомления очередь не пуста и обработчик "жив"
			m_condition.wait(lk, [this] {return !(m_data.empty() && alive()); });
			// нужно проверить, действительно ли очередь не пуста
			if (!m_data.empty())
			{
				auto file = std::move(m_data.front());
				m_data.pop_front();
				// освободим мьютекс, чтобы позволить другому потоку работать с очередью
				lk.unlock();
				doWork(std::move(file), _text_templates);
			}
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
		}
	}
}

void ParserWorker::start() noexcept
{
	if (alive())
	{
		try
		{
			while (!m_data.empty())
			{
				m_condition.notify_one();
			}
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
		}
	}
}
