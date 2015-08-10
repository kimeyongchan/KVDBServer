#ifndef LRUQUEUE
#define LRUQUEUE

#include<iostream>
#include<list>
#include<string>
using namespace std;

template<typename T>
class LRUQueue
{
private:
	
	list<T> lruQueue;
	

public:
	void arrangement()
	{
		lruQueue.pop_front();
	}

	int queueSize()
	{
		return lruQueue.size();
	}

	void insertQueue(T component)
	{
		lruQueue.push_back(component);
	}

	void displayQueue()
	{
		list<T>::iterator lruQueueIt;
		cout << "----------------------- queue -------------------------- " << endl;
		for (lruQueueIt = lruQueue.begin(); lruQueueIt != lruQueue.end(); ++lruQueueIt)
		{
			cout << *lruQueueIt << endl;
		}
	}

	void update(T component)
	{
		list<T>::iterator lruQueueIt;
		lruQueueIt = findValue(component);
		if (lruQueueIt->compare("root") == 0)
		{
			return;
		}

		T tempStr = *lruQueueIt;
		lruQueue.erase(lruQueueIt);
		lruQueue.push_back(tempStr);
	}

	bool isExist(T component)
	{
		if (find(begin(lruQueue), end(lruQueue), component) != end(lruQueue))
		{
			cout << component << endl;
			return true;
		}
		else
			return false;
	}

	typename list<T>::iterator findValue(T component)
	{
		list<T>::iterator lruQueueIt;
		for (lruQueueIt = lruQueue.begin(); lruQueueIt != lruQueue.end(); ++lruQueueIt)
		{
			if (component)
			{
				return lruQueueIt;
			}

		}
		return lruQueue.begin(); // root는 queue에서 제거하지 않는다.
	}

	void remove(T component)
	{
		list<T>::iterator lruQueueIt;
		lruQueueIt = findValue(component);
		T tempStr = *lruQueueIt;
		lruQueue.erase(lruQueueIt);
	}


	T getFront()
	{
		return this->lruQueue.front();
	}
	
};

#endif