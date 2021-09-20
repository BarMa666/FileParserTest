#pragma once

#include <vector>
#include <thread>

class ThreadManager
{
public:
	void init(unsigned _thread_count);

private:
	std::vector<std::thread> m_threads;
};

