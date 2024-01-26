#pragma once

#include <iostream>
#include <chrono>
#include <shared_mutex>

#define MIN_TASK_TIME 5000
#define MAX_TASK_TIME 10000

using namespace std;

enum class Status
{
	kIsStoring, kIgnored, kIsProcessing, kExecuted
};

class Task
{
public:
	Task();
	Task(const Task&);
	~Task();
	virtual void DoWork();

	Task& operator= (const Task&) { return *this; };
	Task& operator= (const Task&&) { return *this; };

	unsigned int getID();
	string getString();
	Status getStatus();
	string getStatusStr();
	void setStatus(Status);

private:
	static unsigned int s_ID;
	unsigned int m_taskID;

	Status m_status;
	unsigned int m_duration_ms;
	chrono::high_resolution_clock::time_point m_start_time;
	chrono::high_resolution_clock::time_point m_end_time;
	chrono::milliseconds m_result_time;
};