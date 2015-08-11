#include<iostream>

#include "lruQueue.hpp"
#include "radix_tree.hpp"
#include "namedcache.h"

using namespace std;

struct test{
	int data;
};
void returnfunc(int& temp, test* t)
{
	temp = 1;
	t->data = 20;
}

int main()
{
	NamedCache nc;
	NamedData* root;
	NamedData* ch1;
	cout << "root namedData: " << nc.getRoodAddr() << endl;
	
	root= nc.getRoodAddr();
	ch1 = nc.findComponent("ch1", root);

	if (!ch1)
	{
		cout << "not found component" << endl;
		ch1 = new NamedData("ch1");
		//root->insert(ch1->getKey(), ch1);
		nc.insert(root, ch1);


		//nc.getQue()->insertQueue(ch1->getKey());
	}
	else
	{
		cout << "find" << ch1 << endl;
	}
	
	if (ch1)
	{
		cout << nc.findComponent(ch1->getKey(), root)->getKey()<< endl;
	}
	
	NamedData* ch2 = new NamedData("ch2");
	nc.insert(ch1, ch2);
	cout << nc.findComponent(ch2->getKey(), ch1)->getKey() << endl;

//	nc.deleteData(ch1->getKey(), root);
//	delete ch1;

	NamedData* temp = nc.findComponent("ch1", root);

	if (!temp)
	{
		cout << " not found" << endl;
	}
	else
	{
		cout << "found : " << temp->getKey() << endl;
	}


	NamedData* ch12 = new NamedData("ch12");
	nc.insert(ch2, ch12);

	cout << "find: " << nc.findComponent(ch12->getKey(), ch2)->getKey() << endl;

	/*nc.deleteData(ch1->getKey(), root);
	delete ch1;*/
	
	ch1 = nc.findComponent("ch1", root);
	if (ch1)
	{
		cout << "find : " << ch1->getKey() << endl;
	}
	else
	{
		cout << "not found" << endl;
	}

	NamedData* ch11 = new NamedData("ch2");
	nc.insert(root, ch11);
	cout <<"find : " << nc.findComponent(ch11->getKey(), root)->getKey() << endl;

	/*
				root
				  |
				 ch1(delete) - ch11
				  |
				 ch2
				  |
				 ch12
	
	*/

	return 0;
}