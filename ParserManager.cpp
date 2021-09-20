#include "ParserManager.h"

void ParserManager::init(TextTemplatesT&& _templates)
{
	m_templates = std::move(_templates);
}