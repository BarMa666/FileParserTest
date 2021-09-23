#pragma once
#include "Primitives.h"
#include "IWorker.h"

#include <string>
#include <thread>
#include <atomic>
#include <fstream>

class MultiParserWorker : public IWorker
{
public:
	explicit MultiParserWorker(const TextTemplateT& _text_templates) noexcept;
	explicit MultiParserWorker(std::string&& _file_name, const TextTemplateT& _text_templates) noexcept;
	virtual ~MultiParserWorker() noexcept;

	MultiParserWorker(const MultiParserWorker&) = delete;
	MultiParserWorker& operator=(const MultiParserWorker&) = delete;
	MultiParserWorker(MultiParserWorker&&);
	MultiParserWorker& operator=(MultiParserWorker&&) = delete;

	void run(FileQueueT&& _file_queue, const TextTemplateT& _text_templates) override;
	bool isDone() const;

	static unsigned workersCount();

private:
	static unsigned g_workers_count;
	std::thread m_thread;

	void doWorkMulti(FileQueueT&& _file_queue, const TextTemplateT& _text_templates) noexcept;
};

