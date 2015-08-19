#ifndef __NAMED_CACHE__
#define __NAMED_CACHE__

#include<iostream>
#include <iterator>
#include<string>
#include <cassert>
#include<list>
#include<algorithm>
#include<map>

#include "radix_tree.h"
#include "Block.h"
#include "SuperBlock.h"
#include "Data.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"

#define MAXNUMDATA 1000
using namespace std;

class NamedQueData
{
private:
    uint32_t childCount;
    string key;
public:
    NamedQueData(string key)
    {
        childCount = 0;
        this->key = key;
    }
    
    NamedQueData(string key, uint32_t count)
    {
        this->childCount = count;
        this->key = key;
    }
    void incChild()
    {
        childCount++;
    }
    string getKey() const
    {
        return this->key;
    }
    uint32_t getChildCount()
    {
        return childCount;
    }
};

class NamedCache
{
private:
    
    NamedData* root;
    list<NamedQueData*> namedQue;
    
public:
	NamedCache(SuperBlock* spBlock)
	{
        this->root = new NamedData("/", spBlock->getRootBlockAddress(),new RadixTree);
        Block* rB = spBlock->getRootBlock();
        const map<uint16_t,IndirectionData*>* dataMap = rB->getIndirectionDataMap();
        
        for(auto it = dataMap->begin(); it != dataMap->end(); ++it)
        {
            uint64_t rootBlockAdr  = spBlock->getRootBlockAddress();
            uint16_t offsetLocation =  it->first;
            uint64_t indBlockAdr = rootBlockAdr + BLOCK_HEADER_SIZE + (offsetLocation *2);
            
            /*   make cache data in root block.  */
            Data* d = it->second->data;
            switch(d->getFormatType())
            {
                case FLAG_DIRECTORY_DATA:
                {   //uint64_t ba = ((DirectoryData*)d)->getIndBlockAddress();
                    string key = ((DirectoryData*)d)->getKey();
                    //((RadixTree*)this->root->getRadixTree())->insertData(key, ba);
                    ((RadixTree*)this->root->getRadixTree())->insertData(key, indBlockAdr);
                    this->namedQue.push_back(new NamedQueData(key));
                    break;
                }
                case FLAG_KEY_VALUE_DATA:
                {
                    //uint64_t ba = this->root->getBlockAddress();
                    string key = ((KeyValueData*)d)->getKey();
                    //((RadixTree*)this->root->getRadixTree())->insertData(key, ba);
                    ((RadixTree*)this->root->getRadixTree())->insertData(key, indBlockAdr);
                    this->namedQue.push_back(new NamedQueData(key));
                    break;
                }
                case FLAG_KEY_VALUE_CHAINING_DATA:
                {
                    //uint64_t ba = ((KeyValueChainingData*)d)->getIndBlockAddress();
                    string key = ((KeyValueChainingData*)d)->getKey();
                    //((RadixTree*)this->root->getRadixTree())->insertData(key, ba);
                    ((RadixTree*)this->root->getRadixTree())->insertData(key, indBlockAdr);
                    this->namedQue.push_back(new NamedQueData(key));
                    break;
                }
            }
        }
        
    }
    
	void insert(NamedData* parent, NamedData* child);
    NamedData* findComponent(string component, NamedData* parent);
	void deleteData(string component, NamedData* parent);
	bool initFunction(const SuperBlock* spBlock);
    NamedData* getRootAddr() { return this->root;}
};

#endif
