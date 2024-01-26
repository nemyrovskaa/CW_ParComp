#include "ThreadPool.h"

ThreadPool::ThreadPool()
{

}

ThreadPool::~ThreadPool()
{
	BreakSafe();
}

void ThreadPool::Init(function<void(void)> callback, int thread_num)
{
	unique_lock<mutex> u_lock(m_rw_mutex);
	if (m_Initialised)
		return;

	this->callback = callback;

	m_thread_num = thread_num < MAX_THREAD_NUM ? thread_num : MAX_THREAD_NUM;
	m_break_execution = m_is_Paused = false;
	m_Initialised = true;

	m_threads.reserve(thread_num);
	for (int i = 0; i < thread_num; i++)
		m_threads.push_back(thread(&ThreadPool::Routine, this));

	m_queue.set_max_size(MAX_Q_SIZE);
}

void ThreadPool::AddTask(Task* task)
{
	if (!m_queue.push(task))
	{
		unique_lock<mutex> u_lock(m_rw_mutex);
		task->setStatus(Status::kIgnored);
	}
	m_wait_for_task.notify_all();
}

void ThreadPool::Routine()
{
	while (true)
	{
		Task* task = nullptr;
		{
			unique_lock<mutex> u_lock(m_rw_mutex);

			if (m_queue.empty())
				callback();

			m_wait_for_task.wait(u_lock, [this] {return (!m_queue.empty() || m_break_execution) && !m_is_Paused; });

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
		task->DoWork();
	}
}

void ThreadPool::BreakSafe()
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
	m_Initialised = false;
}

void ThreadPool::BreakMomentary()
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
	m_Initialised = false;
}

void ThreadPool::Pause()
{
	unique_lock<mutex> u_lock(m_Pause_mutex);
	m_is_Paused = true;
}

void ThreadPool::Resume()
{
	{
		unique_lock<mutex> u_lock(m_Pause_mutex);
		m_is_Paused = false;
	}
	m_wait_for_task.notify_all();
}