#pragma once

#include "Primitives.h"
#include "IWorker.h"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <deque>

class ParserWorker : public IWorker
{
public:
	explicit ParserWorker(const TextTemplateT& _text_templates, size_t _threads_count) noexcept;
	explicit ParserWorker(FileQueueT&& _file_queue, const TextTemplateT& _text_templates, size_t _threads_count) noexcept;

	virtual ~ParserWorker() noexcept;

	ParserWorker(const ParserWorker& _request_worker) = delete;
	ParserWorker operator=(const ParserWorker& _request_worker) = delete;
	ParserWorker(ParserWorker&& _request_worker) = delete;
	ParserWorker operator=(ParserWorker&& _request_worker) = delete;

	void run(FileQueueT&& _file_queue, const TextTemplateT& _text_templates) override;
	bool alive() const;
	void start() noexcept;
private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::vector<std::thread> m_threads;
	std::atomic<bool> m_alive;
	std::deque<std::string> m_data;

	void doWorkConditional(const TextTemplateT& _text_templates) noexcept;
};

