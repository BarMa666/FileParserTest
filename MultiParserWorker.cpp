#include "MultiParserWorker.h"
#include "ParserManager.h"
#include "Logger.h"

// количество работающих потоков
unsigned MultiParserWorker::g_workers_count = 0;

MultiParserWorker::MultiParserWorker(const TextTemplateT& _text_templates) noexcept
	: IWorker(_text_templates)
	, m_thread()
	
{
	++g_workers_count;
}

MultiParserWorker::MultiParserWorker(std::string&& _file_name, const TextTemplateT& _text_templates) noexcept
	: IWorker(_text_templates)
	, m_thread(&MultiParserWorker::doWork, this, std::forward<std::string>(_file_name), std::cref(_text_templates))
{
	++g_workers_count;
}

bool compareTeplateSize(const std::string& _left, const std::string& _right)
{
	return _left.size() < _right.size();
}

MultiParserWorker::~MultiParserWorker() noexcept
{
	try
	{
		// если нужно, то ждем завершение выполенения потока
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		// после уменьшаем число рабочих потоков на единицу, 
		// давая возможность создать новый обработчик менеджеру
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}

	--g_workers_count;
}

MultiParserWorker::MultiParserWorker(MultiParserWorker&& _other)
	: IWorker(std::move(_other))
	, m_thread(std::move(_other.m_thread))
{
}

void MultiParserWorker::run(FileQueueT&& _file_queue, const TextTemplateT& _text_templates)
{
	try
	{
		std::thread temp = std::thread(&MultiParserWorker::doWorkMulti, this, std::forward<FileQueueT>(_file_queue), std::cref(_text_templates));
		m_thread.swap(temp);
	}
	catch (std::exception& _ex)
	{
		std::cerr << _ex.what() << "\n";
	}
}

bool MultiParserWorker::isDone() const
{
	return !m_thread.joinable();
}

unsigned MultiParserWorker::workersCount()
{
	return g_workers_count;
}

void MultiParserWorker::doWorkMulti(FileQueueT&& _file_queue, const TextTemplateT& _text_templates) noexcept
{
	try
	{
		while (!_file_queue.empty())
		{
			auto file = _file_queue.front();
			_file_queue.pop_front();
			doWork(std::move(file), _text_templates);
		}
	}
	catch (const std::exception& _ex)
	{
		Logger::logError(_ex.what());
	}
}