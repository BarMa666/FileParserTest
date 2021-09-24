#pragma once

#include "Primitives.h"

#include <list>

class Parser
{
public:

	Parser(size_t _max_template_size) noexcept;
	virtual ~Parser() noexcept;

	Parser(const Parser& _worker) = delete;
	Parser operator=(const Parser& _worker) = delete;
	Parser(Parser&& _worker) = delete;
	Parser operator=(Parser&& _worker) = delete;

	void parse(std::string&& _file_name, const TextTemplateT& _text_templates) const noexcept;

private:
	size_t m_max_template_size;

	bool parseFile(const std::string& _file_name, const std::string& _temp_name, const TextTemplateT& _text_templates) const noexcept;
	bool replaceTemplates(OUT std::string& _line, const std::string& _template, const std::string& _substitutor) const noexcept;
};

