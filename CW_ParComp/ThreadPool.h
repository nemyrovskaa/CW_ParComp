#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>

#include "ConcurrencyQueue.h"
#include "Task.h"

#define MAX_Q_SIZE 500
#define MAX_THREAD_NUM 20

using namespace std;

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	void Init(function<void(void)> callback, int);
	void AddTask(Task*);
	void Routine();
	void BreakSafe();
	void BreakMomentary();
	void Pause();
	void Resume();

private:
	ConcurrencyQueue<Task*> m_queue;
	vector<thread> m_threads;

	mutex m_rw_mutex, m_Pause_mutex;
	condition_variable m_wait_for_task;
	condition_variable m_Pause;

	bool m_break_execution;
	bool m_Initialised;
	bool m_is_Paused;
	unsigned int m_thread_num;

	function<void(void)> callback;
};