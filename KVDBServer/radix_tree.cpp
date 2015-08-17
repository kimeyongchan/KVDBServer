//
//  radixtree.cpp
//  cache
//
//  Created by SonHojun on 8/11/15.
//  Copyright (c) 2015 SonHojun. All rights reserved.
//

#include "radix_tree.h"

int RadixTree::bits(uint32_t hash, int st)
{
    return (hash & (0x1 << st)) >> st;
}

Node* RadixTree::allocateEachNode(int idx[DEPTH])
{
    Node* temp = this->headNode;
    for(int i = 0; i < DEPTH; i++)
    {
        try {
            if(temp->slot[idx[i]] == NULL)
            {
                temp->slot[idx[i]] = new Node;
            }
            
            temp = temp->slot[idx[i]];
            
        } catch (bad_alloc& err) {
            cout<<"allocateEachNode : " << err.what() << endl;
        }
    }
    
    return temp;
    
}

void RadixTree::createIndex(string key, int (&arr)[DEPTH])
{
    /*
       make hash for key.
     */
    hash<string> hash_fn;
    uint32_t str_hash = (uint32_t)hash_fn(key);
    
    /*
        calculate key hash index.
     */
    for( int i = 1; i < DEPTH+1; i++)
    {
        for(int j = MASKBIT*i - 1; j >= MASKBIT * (i-1); j--)
            arr[i-1] += ((bits(str_hash, j)) << (j-(MASKBIT*(i-1))));
    }
    
}

bool RadixTree::insertData(string key, uint64_t ba)
{
    
    int idx[DEPTH] = { 0, };
    
    createIndex(key, idx);
    
    cout<< "check index "<< endl;
    for(int i=0; i<DEPTH; i++)
    {
        cout << i << " : " << idx[i] <<" | ";
    }
    
    cout<<endl;
    
    /* allocate memory */
    Node* value = allocateEachNode(idx);
    
    if(value == NULL)
        cout<< "error in insertData"<<endl;
    else
    {
        if(findData(key) == NULL)
        {
            NamedData* nd ;
            void * rt;
            try{
            
            
                nd = new NamedData(key,ba);
                rt = nd->getRadixTree();
                rt = new RadixTree;
                cout << "rt" <<rt << endl;
            }catch(bad_alloc& e)
            {
                cout<<e.what()<<endl;
            }
            
            nd->insertVoidRadix(rt);
            value->listValue.push_back(nd);
            this->dataSize++;
            return true;
        }
    }
    
    return false;
}

Node* RadixTree::getLastNode(string key)
{
    int idx[DEPTH] = { 0, };
    
    createIndex(key, idx);
    
    Node* temp = this->headNode;
    
    for(int i=0;i<DEPTH;i++)
    {
        temp = temp->slot[idx[i]];
    }
    
    return temp;
}

void RadixTree::deleteData(string key)
{
    Node* temp = getLastNode(key);
    
    list<NamedData*>::iterator it;
    for(it = temp->listValue.begin(); it != temp->listValue.end() ; ++it)
    {
        NamedData* delData = (*it);
        
        if (delData->getKey().compare(key) == 0) {
            // FIND KEY
            cout<<"Before delete value list size:  " << temp->listValue.size() << endl;
            cout<< "delete key: " << delData->getKey()<<endl;  //need for statement
            temp->listValue.erase(it);
            delete (*it);
            break;
        }
    }
    
    cout<<"After delete value list size:  " << temp->listValue.size() << endl;
    
    int idx[DEPTH] = {0 ,};
    createIndex(key, idx);
    
    if(temp->listValue.size() == 0 ) // no value in Radix tree.
    {
        Node* upper = this->headNode;
        Node* lower;
        for(int i = 0; i < DEPTH; i++)
        {
            lower = upper->slot[idx[i]];
            delete lower;
            upper = lower;
        }
    
    }
   

    this->dataSize--;
   
}

string RadixTree::findKey(string key)
{
     Node* temp = getLastNode(key);
    
    list<NamedData*>::iterator it;
    for(it = temp->listValue.begin(); it != temp->listValue.end() ; ++it)
    {
        NamedData* findData= (*it);
        
        if (findData->getKey().compare(key) == 0) {
            return findData->getKey();
        }
    }
    
    return NULL;
}

NamedData* RadixTree::findData(string key)
{
    

    int idx[DEPTH] = { 0, };
    
    createIndex(key, idx);
    
    Node* temp = this->headNode;
    
    //need error handler.
    try{
        for(int i=0;i<DEPTH;i++)
        {
            if(temp == NULL)
                return NULL;
            
            temp = temp->slot[idx[i]];
        }
        
        //for statement for searching collision value
        if(temp->listValue.front() == NULL)
            return NULL;
        
        cout<< "find key: " << temp->listValue.front()->getKey()<<endl;
        return temp->listValue.front();
        
    }catch(bad_exception& err)
    {
        cout<<err.what()<<endl;
    }
    
    return NULL;
}
