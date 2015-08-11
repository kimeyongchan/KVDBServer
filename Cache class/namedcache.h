#include<iostream>
#include <iterator>
#include<string>
#include <cassert>

#include<list>
#include<algorithm>

#include "lruQueue.hpp"
#include "radix_tree.hpp"

using namespace std;

class NamedData
{
private:
	unsigned __int64 idtAddress;  //pointer??
	NamedData* parentAddr;
	string key;
	radix_tree<string, NamedData> childRadix;  // °¢ node ´ç radix tree.

	friend class NamedCache;
public:
	NamedData(string key/*, unsigned __int64 ba*/)
	{
		this->key = key;
		this->idtAddress = 0x100000000;
		this->parentAddr = NULL;
		this->childRadix.clear();
	}
	NamedData()
	{
		this->idtAddress = 0x100000000;
		this->parentAddr = NULL;
		this->childRadix.clear();

	}
	void insert(string key, NamedData* child)
	{
		this->childRadix[key] = *child;
	}

	string getKey()
	{
		return this->key;
	}

	radix_tree<string, NamedData>* getRadixTree()
	{
		return &this->childRadix;
	}
	unsigned __int64 getIdtAddr()
	{
		return this->idtAddress;
	}
	NamedData*  getParentAddr()
	{
		return this->parentAddr;
	}
	
	__int64 getSize()
	{
		return sizeof(this->childRadix.size());
	}

	void insertParentAddress(NamedData* parent)
	{
		this->parentAddr = parent;
	}

};

class NamedCache
{
private:
	NamedData* rootHeader;
    LRUQueue<string>* que;
	
public:
	NamedCache()
	{
		rootHeader = new NamedData("root");
	}

	void insert(NamedData* parent, NamedData* child);
	NamedData* findComponent(string component, NamedData* parent);
	NamedData* getRoodAddr();
	void deleteData(string component, NamedData* parent);
	void commitLruQueue();
	bool initFunction(/*const super block* */);
	__int64 getNamedCacheSize()
	{
		return this->rootHeader->getSize();
	}

	
};

