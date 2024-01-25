#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <shared_mutex>

#define MIN_TASK_TIME 5000
#define MAX_TASK_TIME 10000

using namespace std;

enum class Status
{
	IS_STORING, IGNORED, IS_PROCESSING, EXECUTED
};

class Task
{
private:
	static unsigned int s_ID;
	unsigned int m_taskID;

	Status m_status;
	unsigned int m_duration_ms;
	chrono::high_resolution_clock::time_point m_start_time;
	chrono::high_resolution_clock::time_point m_end_time;
	chrono::milliseconds m_result_time;
	//shared_mutex m_rw_mutex;

public:
	//unsigned int val;

	Task();
	Task(const Task&);
	~Task();
	virtual void do_work();

	unsigned int get_ID();
	string get_string();
	Status get_status();
	string get_status_str();
	void set_status(Status);

	Task& operator= (const Task&) { return *this; };
	Task& operator= (const Task&&) { return *this; };
};