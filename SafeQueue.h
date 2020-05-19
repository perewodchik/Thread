#pragma once
#include <queue>
#include <mutex>

template<typename T>
class SafeQueue
{
public:
	SafeQueue() = default;

	std::shared_ptr<T> pop()
	{
		std::shared_ptr<T> ptr = nullptr;
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_queue.empty())
		{
			ptr = m_queue.top();
			m_queue.pop();
		}
		return ptr;
	}
	void push(std::shared_ptr<T> ptr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(ptr);
	}
private:
	std::queue<std::shared_ptr<T> > m_queue;
	std::mutex m_mutex;
};