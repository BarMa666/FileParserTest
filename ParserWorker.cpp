#include "ParserWorker.h"
#include "Logger.h"

ParserWorker::ParserWorker(FileQueueT&& _file_queue, const TextTemplateT& _text_templates, size_t _threads_count) noexcept
	: IWorker(_text_templates)
	, m_threads(_threads_count)
	, m_alive(true)
	, m_data(std::forward<FileQueueT>(_file_queue))
{
	init(_text_templates);
}

ParserWorker::~ParserWorker() noexcept
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

void ParserWorker::init(const TextTemplateT& _text_templates)
{
	try
	{
		for (auto& thread : m_threads)
		{
			std::thread temp(&ParserWorker::doWorkConditional, this, _text_templates);
			thread.swap(temp);
		}
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
			m_condition.wait(lk, [this] {return !(m_queue.empty() && alive()); });
			// нужно проверить, действительно ли очередь не пуста
			if (!m_queue.empty())
			{
				auto file = std::move(m_queue.front());
				m_queue.pop_front();
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
				auto data = m_data.front();
				m_data.pop_front();
				{
					std::lock_guard<std::mutex> lk(m_mutex);
					m_queue.emplace_back(std::move(data));
				}
				m_condition.notify_one();
			}
		}
		catch (const std::exception& _ex)
		{
			Logger::logError(_ex.what());
		}
	}
}
