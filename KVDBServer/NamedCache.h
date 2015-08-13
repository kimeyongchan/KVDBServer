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

using namespace std;

struct NameQueData
{
    uint32_t hitCount;
    string key;
};

class NamedCache
{
private:
    
    NamedData* root;
   // RadixTree* fRdTree;  //root block contents input
    LruQueue<NameQueData> namedQue;
    
public:
	NamedCache(const SuperBlock* spBlock)
	{
		//allocate radix tree.
        //fRdTree = new RadixTree;
	}

	void insert(NamedData* parent, NamedData* child);
    NamedData* findComponent(string component, NamedData* parent);
	void deleteData(string component, NamedData* parent);
	bool initFunction(const SuperBlock* spBlock);

	
};

#endif
