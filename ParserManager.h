#pragma once
#include "Primitives.h"

#include <vector>
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>

class ParserManager
{
public:
	ParserManager(unsigned _needed_workers, TextTemplateT&& _text_templates) noexcept;
	~ParserManager() noexcept;
	void start() noexcept;
	void put(std::string&& _file_name) noexcept;
	bool alive() const noexcept;
	static size_t getMaxTemplateSize(const TextTemplateT& _text_templates) noexcept;

private:
	std::condition_variable m_condition;
	std::mutex m_mutex;
	std::vector<std::thread> m_threads;
	
	std::queue<std::string> m_queue;
	std::atomic<bool> m_alive;

	const TextTemplateT m_text_templates;
	const size_t m_max_template_size;

	static unsigned correctWorkersCount(unsigned _needed_workers) noexcept;
	void doWork() noexcept;
};

