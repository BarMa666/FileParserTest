#pragma once
#include "Primitives.h"

#include <string>

class FileManager
{
public:
	FileManager(const std::string& _root) noexcept;
	FileQueueT getFiles() const noexcept;
	std::vector<FileQueueT> getFileQueues(unsigned _threads_count) const noexcept;
	uintmax_t getFilesSize() const noexcept;

private:
	std::string m_root;
};

