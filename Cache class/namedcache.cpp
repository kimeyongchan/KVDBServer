
#include "namedcache.h"
/*
	Todo : 
	  1. memory leak?
	  2. optimize radix tree
	  3. consider lock issue
*/

void NamedCache::insert(NamedData* parent, NamedData* child)
{
	parent->insert(child->getKey(), child);
	child->insertParentAddress(parent);
	cout << "parent addr: " <<hex<< child->getParentAddr()<< endl;
	//que->insertQueue(child->getKey());
}

NamedData* NamedCache::findComponent(string component, NamedData* parent)
{
	radix_tree<string, NamedData>::iterator it;
	radix_tree<string, NamedData>* temp;
	temp = parent->getRadixTree();
	it = temp->find(component);

	if (it != temp->end())
	{
		//que->update(component);
		return &it->second;  //return 
	}
	else
	{
		return NULL;
	}
}

void NamedCache::deleteData(string component, NamedData* parent)
{
	radix_tree<string, NamedData>::iterator it;
	radix_tree<string, NamedData>* temp;
	//temp = parent->getRadixTree()->;  // deep copy??
	//it = temp->find(component);
	it = parent->getRadixTree()->find(component);
	
	cout <<  it->second.getParentAddr()->getKey() << endl;

	//NamedData* parentTrie = it->second.parentAddr;
	//if (it->second.getKey() == component)
	//{
	//	cout << "has child: " << parentTrie->getRadixTree()->size() << endl;
	//	//que->remove(it->second.getKey());  //que에서 삭제. (child 여부)
	//	parent->getRadixTree()->erase(component);
	//}
}

NamedData* NamedCache::getRoodAddr()
{
	return this->rootHeader;
}

void NamedCache::commitLruQueue()
{

}


