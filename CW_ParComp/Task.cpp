#include "Task.h"

unsigned int Task::s_ID = 0;

Task::Task()
{
	m_taskID = ++s_ID;
	m_status = Status::kIsStoring;
	m_duration_ms = MIN_TASK_TIME + rand() % (MAX_TASK_TIME - MIN_TASK_TIME + 1);
	m_result_time = chrono::milliseconds(0);
}

Task::Task(const Task& other)
{
	m_taskID = other.m_taskID;
	m_status = other.m_status;
	m_duration_ms = other.m_duration_ms;
	m_result_time = other.m_result_time;
	m_start_time = other.m_start_time;
	m_end_time = other.m_end_time;
}

Task::~Task()
{

}

void Task::DoWork()
{
	setStatus(Status::kIsProcessing);
	m_start_time = chrono::high_resolution_clock::now();

	m_end_time = chrono::high_resolution_clock::now();
	m_result_time = chrono::duration_cast<chrono::milliseconds>(m_end_time - m_start_time);
	setStatus(Status::kExecuted);
}

unsigned int Task::getID()
{
	return m_taskID;
}

string Task::getString()
{
	string ret_str;
	ret_str = string("Task [") + to_string(m_taskID) + string("] - Duration(") +
		to_string(m_result_time.count()) + string(" ms) ") + getStatusStr() + "\n";
	return ret_str;
}

Status Task::getStatus()
{
	return m_status;
}

string Task::getStatusStr()
{
	switch (m_status)
	{
	case Status::kIsStoring:
		return "IS STORING";
	case Status::kIgnored:
		return "WAS IGNORED";
	case Status::kIsProcessing:
		return "IS PROCESSING...";
	case Status::kExecuted:
		return "WAS EXECUTED";
	default:
		return "STATUS ERROR";
	}
}

void Task::setStatus(Status status)
{
	m_status = status;
}