#pragma once

#include "Primitives.h"

class IWorker
{
public:
	explicit IWorker(const TextTemplateT& _text_templates) noexcept;
	virtual ~IWorker() noexcept = default;

	IWorker(const IWorker&) = delete;
	IWorker& operator=(const IWorker&) = delete;
	IWorker(IWorker&&);
	IWorker& operator=(IWorker&&) = delete;

	virtual void run(FileQueueT&& _file_queue, const TextTemplateT& _text_templates) = 0;

protected:
	const size_t MAX_TEMPLATE_LEN;
	void ParseFile(const std::string& _file_name, const std::string& _temp_name, const TextTemplateT& _text_templates) const noexcept;
	void doWork(std::string&& _file_name, const TextTemplateT& _text_templates) noexcept;

private:
	void replaceTemplates(OUT std::string& _line, const std::string& _template, const std::string& _substitutor) const noexcept;
	size_t getMaxTemplateSize(const TextTemplateT& _text_templates) const noexcept;
	
	
};

