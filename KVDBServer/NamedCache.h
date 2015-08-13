#ifndef __NAMED_CACHE__
#define __NAMED_CACHE__

#include<iostream>
#include <iterator>
#include<string>
#include <cassert>
#include<list>
#include<algorithm>

#include "radix_tree.h"
#include "Block.h"
#include "SuperBlock.h"
#include "lruQueue.h"

#define MAX 1000
using namespace std;

class NameQueData
{
private:
    uint32_t hitCount;
    string key;
public:
    NameQueData(string key)
    {
        hitCount = 1;
        this->key = key;
    }
    
    NameQueData(string key, uint32_t hitCount)
    {
        this->hitCount = hitCount;
        this->key = key;
    }
    void inc()
    {
        hitCount++;
    }
    string getKey() const
    {
        return this->key;
    }
    uint32_t getHitCount()
    {
        return hitCount;
    }
};

class NamedCache
{
private:
    
    NamedData* root;
    list<NameQueData*> namedQue;
    
public:
	NamedCache(const SuperBlock* spBlock)
	{
       // this->root = new NameData("/",spBlock->getRootAddress());
    }

	void insert(NamedData* parent, NamedData* child);
    NamedData* findComponent(string component, NamedData* parent);
	void deleteData(string component, NamedData* parent);
	bool initFunction(const SuperBlock* spBlock);

	
};

#endif
