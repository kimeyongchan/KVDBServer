#include "NamedCache.h"

void NamedCache::insert(NamedData* parent, NamedData* child)
{
    RadixTree* rt = (RadixTree*)parent->getRadixTree();
    bool success = rt->insertData(child->getKey(), child->getBlockAddress());
    if(success == true)
    {
        //search que whether the data is aleady in queue.
        NameQueData nqd;
        nqd.key = child->getKey();
        nqd.hitCount = 1;
        
        this->namedQue.insertQueue(nqd);
    }
}

NamedData* NamedCache::findComponent(string component, NamedData* parent)
{
	
    //NamedData* nd = this->fRdTree->findData(parent->getKey());
    RadixTree* temp = ((RadixTree*)parent->getRadixTree());
    if(temp == NULL) return NULL;
    else
    {
        //queue
        
    }
        
    NamedData* fd = temp->findData(component);
    
    if(fd != NULL)
        return fd;
    
    return NULL;
}

void NamedCache::deleteData(string component, NamedData* parent)
{
    ((RadixTree*)parent->getRadixTree())->deleteData(component);

}



