#include "Task.h"

unsigned int Task::s_ID = 0;

Task::Task()
{
	m_taskID = ++s_ID;
	m_status = Status::IS_STORING;
	m_duration_ms = MIN_TASK_TIME + rand() % (MAX_TASK_TIME - MIN_TASK_TIME + 1);
	m_result_time = chrono::milliseconds(0);

	//val = 0;
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

void Task::do_work()
{
	set_status(Status::IS_PROCESSING);
	m_start_time = chrono::high_resolution_clock::now();

	//Sleep(m_duration_ms);

	//for (int i = 0; i < 10; i++)
		//val++;

	m_end_time = chrono::high_resolution_clock::now();
	m_result_time = chrono::duration_cast<chrono::milliseconds>(m_end_time - m_start_time);
	set_status(Status::EXECUTED);
}

unsigned int Task::get_ID()
{
	return m_taskID;
}

string Task::get_string()
{
	string ret_str;
	ret_str = string("Task [") + to_string(m_taskID) + string("] - Duration(") +
		to_string(m_result_time.count()) + string(" ms) ") + get_status_str() + "\n";
	return ret_str;
}

Status Task::get_status()
{
	//shared_lock<shared_mutex> s_lock(m_rw_mutex);
	return m_status;
}

string Task::get_status_str()
{
	switch (m_status)
	{
	case Status::IS_STORING:
		return "IS STORING";
	case Status::IGNORED:
		return "WAS IGNORED";
	case Status::IS_PROCESSING:
		return "IS PROCESSING...";
	case Status::EXECUTED:
		return "WAS EXECUTED";
	default:
		return "STATUS ERROR";
	}
}

void Task::set_status(Status status)
{
	//unique_lock<shared_mutex> u_lock(m_rw_mutex);
	m_status = status;
}