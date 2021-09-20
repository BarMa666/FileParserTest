#include "FileManager.h"

void FileManager::init(std::wstring&& _root)
{
	m_root = std::move(_root);
}
