#include "NamedCache.h"

void NamedCache::insert(NamedData* parent, NamedData* child)
{
    RadixTree* rt = (RadixTree*)parent->getRadixTree();
    bool success = rt->insertData(child->getKey(), child->getBlockAddress());
    if(success == true)
    {
        //search que whether the data is aleady in queue.
        NameQueData* nqd = new NameQueData(child->getKey());
        this->namedQue.push_back(nqd);
        
    }
}

NamedData* NamedCache::findComponent(string component, NamedData* parent)
{
    
    RadixTree* temp = ((RadixTree*)parent->getRadixTree());
    if(temp == NULL) return NULL;
    else
    {
        
    }
        
    NamedData* fd = temp->findData(component);
    
    if(fd != NULL)
    {
        list<NameQueData*>::iterator it;
        for(it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
        {
                NameQueData* temp =  *it;
                if(temp->getKey().compare(fd->getKey()) == 0)
                {
                    int hitCount = temp->getHitCount();
                    hitCount++;
                    string key = temp->getKey();
                    this->namedQue.erase(it);
                    NameQueData* newData = new NameQueData(key,hitCount);
                    this->namedQue.push_back(newData);
                }
        }
        return fd;
    }
    
    return NULL;
}

void NamedCache::deleteData(string component, NamedData* parent) //arrange
{
    // warrant data which will be deleted should be in the Radix Tree.
    ((RadixTree*)parent->getRadixTree())->deleteData(component);
    
    list<NameQueData*>::iterator it;
    for(it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
    {
        NameQueData* temp =  *it;
        if(temp->getKey().compare(component) == 0)
        {
            int hitCount = temp->getHitCount();
            hitCount++;
            string key = temp->getKey();
            this->namedQue.erase(it);
            NameQueData* newData = new NameQueData(key,hitCount);
            this->namedQue.push_back(newData);
        }
    }
}



