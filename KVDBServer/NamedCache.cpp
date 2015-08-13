#include "NamedCache.h"

void NamedCache::insert(NamedData* parent, NamedData* child)
{
    
    if(namedQue.size() > MAXNUMDATA)
    {
            //arrange Queue 
    }
    
    RadixTree* rt = (RadixTree*)parent->getRadixTree();
    bool success = rt->insertData(child->getKey(), child->getBlockAddress());
    if(success == true)
    {
        //search que whether the data is aleady in queue.
        NamedQueData* nqd = new NamedQueData(child->getKey());
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
        list<NamedQueData*>::iterator it;
        for(it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
        {
                NamedQueData* temp =  *it;
                if(temp->getKey().compare(fd->getKey()) == 0)
                {
                    int hitCount = temp->getChildCount();
                    hitCount++;
                    string key = temp->getKey();
                    this->namedQue.erase(it);
                    NamedQueData* newData = new NamedQueData(key,hitCount);
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
    
    list<NamedQueData*>::iterator it;
    for(it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
    {
        NamedQueData* temp =  *it;
        if(temp->getKey().compare(component) == 0)
        {
            int hitCount = temp->getChildCount();
            hitCount++;
            string key = temp->getKey();
            this->namedQue.erase(it);
            NamedQueData* newData = new NamedQueData(key,hitCount);
            this->namedQue.push_back(newData);
        }
    }
}



