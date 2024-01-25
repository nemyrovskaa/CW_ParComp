#pragma once

#include "ConcurrencyQueue.h"
#include "Task.h"
#include <vector>
#include <thread>
#include <condition_variable>

#include <functional>

#define MAX_Q_SIZE 1000
#define MAX_THREAD_NUM 20

using namespace std::chrono;

class ThreadPool
{
private:
	ConcurrencyQueue<Task*> m_queue;
	vector<thread> m_threads;

	mutex m_rw_mutex, m_pause_mutex;//, m_clock_mutex;
	condition_variable m_wait_for_task;
	condition_variable m_pause;

	bool m_break_execution;
	bool m_initialised;
	bool m_is_paused;
	unsigned int m_thread_num;

	high_resolution_clock::time_point m_start_time;
	high_resolution_clock::time_point m_end_time;

	function<void(void)> callback;

public:
	unsigned long long max_time, min_time;

	ThreadPool();
	~ThreadPool();
	void init_test(function<void(void)> callback, int);
	void init(function<void(void)> callback, int);
	void init(vector<Task*>, function<void(void)>, int);
	void add_task(Task*);
	void routine();
	void break_safe();
	void break_momentary();
	void pause();
	void resume();
};