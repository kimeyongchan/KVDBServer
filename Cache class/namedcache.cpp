#include<iostream>
#include "radix_tree.hpp"
#include <iterator>
#include<string>
#include <cassert>
#include<list>
#include<algorithm>
using namespace std;

/*

	Todo: 
		2. Search bufferCache and update namedCache
		3. root directory cache
*/


class LRUQueue
{
private:
	list<string> lruQueue;
	list<string>::iterator lruQueueIt;
public:
	void arrangement()
	{
		lruQueue.pop_front();
	}

	int queueSize()
	{
		return lruQueue.size();
	}

	void insertQueue(string component)
	{
		lruQueue.push_back(component);
	}

	void displayQueue()
	{
		cout << "----------------------- queue -------------------------- " << endl;
		for (lruQueueIt = lruQueue.begin(); lruQueueIt != lruQueue.end(); ++lruQueueIt)
		{
			cout << *lruQueueIt << endl;
		}
	}

	void update(string component)
	{
		lruQueueIt = findValue(component);
		if ((*lruQueueIt).compare("root") == 0)
		{
			return;
		}

		string tempStr = *lruQueueIt;
		lruQueue.erase(lruQueueIt);


		lruQueue.push_back(tempStr);
	}

	bool isExist(string component)
	{
		if (find(begin(lruQueue), end(lruQueue), component) != end(lruQueue))
		{
			cout << component << endl;
			return true;
		}
		else
			return false;
	}

	list<string>::iterator findValue(string component)
	{
		for (lruQueueIt = lruQueue.begin(); lruQueueIt != lruQueue.end(); ++lruQueueIt)
		{
			if ((*lruQueueIt).compare(component) == 0)
			{
				return lruQueueIt;
			}

		}
		return lruQueue.begin(); // root는 queue에서 제거하지 않는다.
	}

	void remove(string component)
	{
		lruQueueIt = findValue(component);
		string tempStr = *lruQueueIt;
		lruQueue.erase(lruQueueIt);
	}

};

struct NamedData
{
	__int64 idtAddress;
	bool isChaining;
	string key;
	//__int64    parentMemoryAddr;
	//namedData* prev;
	//namedData* next;
};

class NamedCache
{

private:
	radix_tree<string, NamedData> nc;
	class LRUQueue que;
public:

	NamedCache()
	{

	}

	NamedCache(string root, NamedData* ba)
	{
		// root directory cache
		insert(root, ba);
	}

	

	void deleteData(string component)
	{
		deleteQueue(component);
		nc.erase(component);
	}
	

	bool insert(string component, NamedData* nd)
	{
		if (!this->findIBA(component))
		{

			//buffercache search
			nc[component] = *nd;
			que.insertQueue(component);
		}
		else
			return false;
	}

	__int64 findIBA(string component)
	{
		radix_tree<string, NamedData>::iterator it;	
		it = nc.find(component);

		if (it != nc.end())
		{
			return it->second.idtAddress;
		}
		else
			return NULL;

	}

	NamedData* findValue(string component)
	{
		radix_tree<string, NamedData>::iterator it;
		it = nc.find(component);
		
		if (it != nc.end())
		{
			cout << it->first << endl;  // key
			cout << it->second.idtAddress << endl;  //value
			return &it->second;
		}
		else
			return NULL;
	}

	void displayTree()
	{
		radix_tree<string, NamedData>::iterator it;
		
		cout << "--------------------- KeyValue ------------------------ " << endl;
		for (it = nc.begin(); it != nc.end(); ++it) {
			cout <<it->first << ", " << hex << "0x"<<it->second.idtAddress << endl;
		}
	}

	void displayQueue()
	{
		que.displayQueue();
	}

	void updateQueue(string component)
	{
		que.update(component);
	}

	bool isValuaInQueue(string component)
	{
		return que.isExist(component);
	}

	list<string>::iterator findValueInQueue(string component)
	{
		return que.findValue(component);
	}

	void deleteQueue(string component) 
	{
		que.remove(component);

	}

};

void input(NamedCache& ncH)
{
	NamedData* n1 = new NamedData;
	n1->idtAddress = 0x11111111;
	n1->isChaining = false;

	NamedData* n2 = new NamedData;
	n2->idtAddress = 0x22222222;
	n2->isChaining = false;

	NamedData* n3 = new NamedData;
	n3->idtAddress = 0x33333333;
	n3->isChaining = false;

	NamedData* n4 = new NamedData;
	n4->idtAddress = 0x444444444444;
	n4->isChaining = true;

	NamedData* n5 = new NamedData;
	n5->idtAddress = 0x555555555555;
	n5->isChaining = true;

	ncH.insert("root", n1);   //full path???
	cout << "insert result: " << ncH.insert("root", n1) << endl; // if insert exist key, return 'false'
	ncH.insert("rootch1", n2);
	ncH.insert("rootch1ch2", n3);
	ncH.insert("rootch2", n4);
	ncH.insert("rootch1sib", n5);

}
int main()
{
	NamedCache ncHeader;
	input(ncHeader);

	__int64 tnd = ncHeader.findIBA("aaaabbccde");  // need alignment
	
	if (tnd != NULL)
		cout <<"find key : "<< hex<<"0x"<< tnd << endl;  // find key
	else
		cout << "not find key : NULL" << endl;				// not find key.
	
	
	ncHeader.displayQueue();
	ncHeader.deleteData("rootch1");
	//ncHeader.deleteQueue("rootch1");
	ncHeader.displayTree();
	//ncHeader.isValuaInQueue("rootch1sib");
	
	ncHeader.updateQueue("rootch1ch2");
	ncHeader.displayQueue();
	return 0;
}