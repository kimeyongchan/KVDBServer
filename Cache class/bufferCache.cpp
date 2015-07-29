#include<list>
#include<hash_map>
#include<iostream>
using namespace std;
#define BLOCKSIZE 8*1024

struct Data
{
	struct dataInfo{
		unsigned char type : 1; // 0 : folder,  1 : file
		unsigned char chain : 1; // 0 : not chaining, 1 : chaining
	}dInfo;  //1
	unsigned char keyLen;  //1byte
	unsigned __int32 valueLen;  //1byte
	unsigned __int64 chaingAddr;  //1byte
};

class Block
{
public:  //private set, get function
	struct BlockInfo{
		unsigned char isDirty : 1;
	}bkInfo;  //1

	unsigned __int64 blockAddr; // physical address //8
	unsigned __int16 remainSize;  //2 
	list<unsigned __int16> idt;   // or realloc? //12
	char arr[BLOCKSIZE]; //??  map

public:
	Block()
	{
		bkInfo.isDirty = 1; // 0 : pure -> not need to write, 1: should write 
		blockAddr = 0;   // memory addr?
		remainSize = BLOCKSIZE;
		memset(arr, 0, BLOCKSIZE);
		this->idt.clear();
	}

	Block(/*int type, int chain, */int isDirty, unsigned __int64 blockAddr, unsigned __int16 remainSize,/* unsigned __int64 chainAddr, */list<unsigned __int16> idt)  //load
	{
		bkInfo.isDirty = isDirty;
		blockAddr = blockAddr;
		remainSize = remainSize;
		this->idt = idt;
	}
};

class BufferCache
{
private:
	hash_map<unsigned __int64, Block> bcMap;
	__int32 count;
	list<Block> dirtyList;
public:
	BufferCache()
	{
		count = 0;
	}

	void insertBlock(unsigned __int64 ba, Block bk)
	{

		// check how many blocks exists
		if (count < 1000)
		{
			bcMap.insert(make_pair(ba, bk));
			count++;
		}
		else;
			//  arrange block
	}

	void insertData(unsigned __int64 ba, string component, char* value)
	{
		Block* blk = findBlock(ba);
		if (blk->remainSize > sizeof(*value))
		{
			//idt -> 키 확인 len 확인 
			//set dirty

		}
		else
		{
			//new block 할당
		}
}

	Block* findBlock(unsigned __int64 ba)
	{
		hash_map<unsigned __int64, Block>::iterator it =bcMap.find(ba);

		if (it == bcMap.end())
			return NULL;
		return &(it->second);

	}

	bool deleteData(unsigned __int64 ba, string key);
	bool deleteData(string Key);
	bool arrangeDirty();


};

int main()
{
	BufferCache bc;
	Block* bk = new Block();
	bc.insertBlock(0x10000000000, *bk);
	Block* bk1 =  bc.findBlock(0x10000000000);
	
	if (bk1 == NULL)
	{
		cout << "here" << endl;
	}
	else 
		cout << bk1->blockAddr<< endl;

	return 0;
}
