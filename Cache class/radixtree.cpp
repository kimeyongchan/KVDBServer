#include<iostream>
#include<string>
#include<list>
#include<hash_map>
using namespace std;

#define SLOTSIZE 64
#define FIRSTMASK 6
#define SECONDMASK 12

int bits(unsigned int hash, int st)
{
	cout << ((hash & (0x1 << st)) >> st);
	return (hash & (0x1 << st)) >> st;
}

struct Data
{
	string key;
	//Node* slot[SLOTSIZE];
	//Data* parentData;
};

struct Node
{
	//Data d; 
	//Node* slot[SLOTSIZE]; //64*4
	string key;
	hash_map <const int, Node*> slot;
};

int main()
{
	string key = "aaaaaqwe";
	hash<string> hash_fn;
	unsigned int str_hash = hash_fn(key);
	cout << str_hash % 64 << endl;
	Node* root = new Node;
	int total1 = 0, total2 = 0, total3 = 0, total4 = 0, total5 = 0;
	
	//for (int i = 0; i < SLOTSIZE; i++)
	//{
	//	root->slot[i] = new Node;
	//	for (int j = 0; j < SLOTSIZE; j++)
	//	{
	//		root->slot[i]->slot[j] = new Node;
	//		for (int k = 0; k < SLOTSIZE; k++)
	//		{
	//			root->slot[i]->slot[j]->slot[k] = new Node;
	//		}
	//	}
	//}

	cout << str_hash  << '\n';
	
	/*for (int i = 18; i >= 0; i--)
		cout << bits(str_hash, i);
	cout << endl;*/
	
	cout << "first key : ";
	/*for (int i = 5; i >= 0; i--)
		cout << bits(str_hash, i) << endl;
	
	cout << endl;*/
	
	for (int i = 5; i >= 0; i--)
		total1 +=((bits(str_hash, i)) << i);

	cout << endl;
	cout << total1 << endl;

	root->slot.insert(hash_map<int, Node*>::value_type(total1, new Node));

	cout << "second key : ";
	/*for (int i = 11; i >= 6; i--)
		cout << bits(str_hash, i) << endl;

	cout << endl;*/

	for (int i = 11; i >= 6; i--)
		total2 += ((bits(str_hash, i)) << (i-6));
	cout << endl;
	cout << total2 << endl;

	root->slot[total1]->slot.insert(hash_map<int, Node*>::value_type(total2, new Node));
	/*for (int i = 18; i >= 12; i--)
		cout << bits(str_hash, i) << endl;

	cout << endl;*/

	cout << "third key: ";
	
	for (int i = 17; i >= 12; i--)
		total3 += ((bits(str_hash, i)) << (i - 12));
	cout << endl;
	cout << total3 << endl;
	root->slot[total1]->slot[total2]->slot.insert(hash_map<int, Node*>::value_type(total3, new Node));
	/*root->slot[total] = new Node;
	root->slot[total]->slot[total1] = new Node;
	root->slot[total]->slot[total1]->slot[total2] = new Node;
	cout << total2;
	Node* temp = root->slot[total]->slot[total1]->slot[total2];
	temp->d.key = key;
	*/
	cout << "forth key: ";
	for (int i = 23; i >= 18; i--)
		total4 += ((bits(str_hash, i)) << (i - 18));
	
	cout << endl;
	cout << total4 << endl;
	root->slot[total1]->slot[total2]->slot[total3]->slot.insert(hash_map<int, Node*>::value_type(total4, new Node));

	cout << "fifth key: ";
	for (int i = 29; i >= 24; i--)
		total5 += ((bits(str_hash, i)) << (i - 29));
	
	cout << endl;
	cout << total5 << endl;
	Node* temp = new Node();
	temp->key = "key";

	root->slot[total1]->slot[total2]->slot[total3]->slot[total4]->slot.insert(hash_map<int, Node*>::value_type(total5, temp));

	cout<<"find key: " <<root->slot[total1]->slot[total2]->slot[total3]->slot[total4]->slot[total5]->key << endl;
	cout << sizeof(root->slot);
	return 0;
}