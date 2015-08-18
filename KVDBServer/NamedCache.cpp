#include "NamedCache.h"

void NamedCache::insert(NamedData* parent, NamedData* child)
{
    
    if(namedQue.size() > MAXNUMDATA)
    {
        for(auto it = namedQue.begin(); it != namedQue.end(); ++it)
        {
            if((*it)->getChildCount() > 0)
                continue;
            else
            {
                deleteData(child->getKey(), parent);
                this->namedQue.remove(*it);
                break;
            }
        }
        
        //child 도 다 있음.
    }
    
    RadixTree* rt = (RadixTree*)parent->getRadixTree();
    if(rt == NULL)
    {
        cout<< "parent Radix tree is NULL" <<endl;
        return;
    }
    
    if(child->getRadixTree() == NULL)
        child->insertVoidRadix(new RadixTree);
    
    bool success = rt->insertData(child->getKey(), child->getBlockAddress());
    if(success == true)
    {
        //search que whether the data is aleady in queue.
        for(auto it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
        {
                if((*it)->getKey().compare(parent->getKey()) == 0)
                {
                    (*it)->incChild();
                    NamedQueData* newData = new NamedQueData(parent->getKey(), (*it)->getChildCount());
                    this->namedQue.remove((*it));
                    this->namedQue.push_back(newData);
                    break;
                }
        }
        
        NamedQueData* nqd = new NamedQueData(child->getKey());
        this->namedQue.push_back(nqd);
        
    }
}

NamedData* NamedCache::findComponent(string component, NamedData* parent)
{
    
    RadixTree* temp = ((RadixTree*)parent->getRadixTree());
    if(temp == NULL) return NULL;

        
    NamedData* fd = temp->findData(component);
    
    if(fd != NULL)
    {
        list<NamedQueData*>::iterator it;
        for(auto it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
        {
            if((*it)->getKey().compare(parent->getKey()) == 0)
            {
                NamedQueData* newData = new NamedQueData(parent->getKey(), (*it)->getChildCount());
                delete (*it);
                this->namedQue.push_back(newData);  //parent update.
                break;
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
    
    for(auto it = this->namedQue.begin() ; it != this->namedQue.end(); ++it)
    {
        if((*it)->getKey().compare(component) == 0)
        {
            //delete (*it);
            this->namedQue.remove(*it);
            break;
        }
    }
}



