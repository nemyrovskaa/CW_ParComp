#include "ThreadPool.h"

ThreadPool::ThreadPool()
{

}

ThreadPool::~ThreadPool()
{
	break_safe();
}

void ThreadPool::init(function<void(void)> callback, int thread_num)
{
	unique_lock<mutex> u_lock(m_rw_mutex);
	if (m_initialised)
		return;

	this->callback = callback;

	m_thread_num = thread_num < MAX_THREAD_NUM ? thread_num : MAX_THREAD_NUM;
	m_break_execution = m_is_paused = false;
	m_initialised = true;

	m_threads.reserve(thread_num);
	for (int i = 0; i < thread_num; i++)
		m_threads.push_back(thread(&ThreadPool::routine, this));

	m_queue.set_max_size(MAX_Q_SIZE);
}

void ThreadPool::add_task(Task* task)
{
	if (!m_queue.push(task))
	{
		unique_lock<mutex> u_lock(m_rw_mutex);
		task->set_status(Status::IGNORED);
	}
	m_wait_for_task.notify_all();
}

void ThreadPool::routine()
{
	while (true)
	{
		Task* task = nullptr;
		{
			unique_lock<mutex> u_lock(m_rw_mutex);

			if (m_queue.empty())
				callback();

			m_wait_for_task.wait(u_lock, [this] {return (!m_queue.empty() || m_break_execution) && !m_is_paused; });

			if (m_break_execution)
				return;

			try
			{
				task = m_queue.pop();
			}
			catch (out_of_range& e)
			{
				cout << e.what() << endl;
			}
		}
		task->do_work();
	}
}

void ThreadPool::break_safe()
{
	{
		unique_lock<mutex> u_lock(m_rw_mutex);
		m_break_execution = true;
	}

	m_wait_for_task.notify_all();
	for (thread& i : m_threads)
		if (i.joinable())
			i.join();

	m_threads.clear();
	m_queue.clear();
	m_initialised = false;
}

void ThreadPool::break_momentary()
{
	{
		unique_lock<mutex> u_lock(m_rw_mutex);
		m_break_execution = true;
	}

	m_wait_for_task.notify_all();
	for (thread& i : m_threads)
		if (i.joinable())
			i.detach();

	m_threads.clear();
	m_queue.clear();
	m_initialised = false;
}

void ThreadPool::pause()
{
	unique_lock<mutex> u_lock(m_pause_mutex);
	m_is_paused = true;
}

void ThreadPool::resume()
{
	{
		unique_lock<mutex> u_lock(m_pause_mutex);
		m_is_paused = false;
	}
	m_wait_for_task.notify_all();
}