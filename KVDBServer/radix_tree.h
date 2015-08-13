//
//  radix_tree.h
//  radixTreeCache
//
//  Created by SonHojun on 8/11/15.
//  Copyright (c) 2015 SonHojun. All rights reserved.
//

#ifndef __radixTreeCache__radix_tree__
#define __radixTreeCache__radix_tree__

#include <iostream>
#include <cstring>
#include <map>
#include <list>

using namespace std;

#define SLOTSIZE 256
#define MASKBIT 8
#define DEPTH 4

struct value
{
    string key;
    uint64_t ba;
};

class NamedData
{
private:
    string key;
    uint64_t ba;
    void* tree;
public:
    NamedData(string key, uint64_t ba)
    {
        this->key = key;
        this->ba = ba;
        this->tree = NULL;
    }
    string getKey() {  return this->key; }
    uint64_t getBlockAddress() {  return this->ba; }
    void* getRadixTree() { return this->tree; }
    void insertVoidRadix(void* addr) { this->tree = addr; }
};

struct Node
{
    list<NamedData*> listValue;
    Node* slot[SLOTSIZE]; //256*4
};


class RadixTree
{
private:
    Node* headeNnode;
    uint32_t dataSize;
    
private:
    int bits(uint32_t hash, int st);
    Node* allocateEachNode(int idx[DEPTH]);
    void createIndex(string key,int (&arr)[DEPTH]);
    Node* getLastNode(string key);
    string findKey(string key);
public:
    RadixTree()
    {
        this->dataSize = 0;
        try{
            this->headeNnode = new Node();
            
            
        }catch(bad_alloc& e)
        {
            cout<<e.what()<<endl;
        }
    
    }
    ~RadixTree()
    {
        //delete all;
    }
    
    RadixTree(NamedData* data)
    {
        this->dataSize = 0;
        try{
            this->headeNnode = new Node();
            insertData(data->getKey(), data->getBlockAddress());
            
        }catch(bad_alloc& e)
        {
            cout<<e.what()<<endl;
        }

    }
    
    bool insertData(string key, uint64_t ba);
    void deleteData(string key);
    NamedData* findData(string key);
    uint32_t getSize() const
    {
        return this->dataSize;
    }
};

#endif /* defined(__radixTreeCache__radix_tree__) */
