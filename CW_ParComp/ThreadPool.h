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
private:
	ConcurrencyQueue<Task*> m_queue;
	vector<thread> m_threads;

	mutex m_rw_mutex, m_pause_mutex;
	condition_variable m_wait_for_task;
	condition_variable m_pause;

	bool m_break_execution;
	bool m_initialised;
	bool m_is_paused;
	unsigned int m_thread_num;

	function<void(void)> callback;

public:
	ThreadPool();
	~ThreadPool();

	void init(function<void(void)> callback, int);
	void add_task(Task*);
	void routine();
	void break_safe();
	void break_momentary();
	void pause();
	void resume();
};