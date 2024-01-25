#pragma once

#include <shared_mutex>
#include <climits>

using namespace std;

template <class T>
class ConcurrencyQueue
{
private:
	struct Node
	{
		Node* next;
		Node* previous;
		T data;

		Node(Node* next, Node* previous, T data)
		{
			this->next = next;
			this->previous = previous;
			this->data = data;
		}
	};

	unsigned int m_size;
	unsigned int m_max_size;
	Node* m_head;
	Node* m_tail;
	shared_mutex m_rw_mutex;

public:
	ConcurrencyQueue();
	~ConcurrencyQueue();
	bool push(T data);
	T pop();
	bool empty();
	void clear();
	unsigned int get_size();
	void set_max_size(unsigned int);
};

template <typename T>
ConcurrencyQueue<T>::ConcurrencyQueue()
{
	this->m_head = nullptr;
	this->m_tail = nullptr;
	this->m_size = 0;
	this->m_max_size = UINT_MAX;
}

template <typename T>
ConcurrencyQueue<T>::~ConcurrencyQueue()
{
	clear();
}

template <typename T>
bool ConcurrencyQueue<T>::push(T data)
{
	{
		shared_lock<shared_mutex> s_lock(m_rw_mutex);
		if (m_size == m_max_size)
			return false;
	}

	unique_lock<shared_mutex> u_lock(m_rw_mutex);
	Node* new_node = new Node(nullptr, m_tail, data);
	if (m_tail == nullptr)
	{
		m_tail = new_node;
		m_head = m_tail;
	}
	else
	{
		m_tail->next = new_node;
		m_tail = new_node;
	}
	m_size++;
	return true;
}

template <typename T>
T ConcurrencyQueue<T>::pop()
{
	unique_lock<shared_mutex> u_lock(m_rw_mutex);
	if (m_tail == nullptr)
		throw out_of_range("Queue is empty.");

	T ret_data = m_head->data;
	if (m_head == m_tail)
	{
		delete m_head;
		m_head = m_tail = nullptr;
	}
	else
	{
		m_head = m_head->next;
		delete m_head->previous;
		m_head->previous = nullptr;
	}
	m_size--;
	return ret_data;
}

template <typename T>
bool ConcurrencyQueue<T>::empty()
{
	shared_lock<shared_mutex> s_lock(m_rw_mutex);
	return m_size == 0 ? true : false;
}

template <typename T>
void ConcurrencyQueue<T>::clear()
{
	unique_lock<shared_mutex> u_lock(m_rw_mutex);
	if (m_tail == nullptr)
		return;
	Node* temp = m_tail;
	while (temp->previous != nullptr)
	{
		temp = temp->previous;
		delete temp->next;
		temp->next = nullptr;
	}
	delete temp;
	m_head = m_tail = nullptr;
	m_size = 0;
}

template <typename T>
unsigned int ConcurrencyQueue<T>::get_size()
{
	shared_lock<shared_mutex> s_lock(m_rw_mutex);
	return m_size;
}

template <typename T>
void ConcurrencyQueue<T>::set_max_size(unsigned int max_size)
{
	unique_lock<shared_mutex> u_lock(m_rw_mutex);
	m_max_size = max_size;
}