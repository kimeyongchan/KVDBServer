#include<hash_map>
#include "lruQueue.hpp"
#include "Block.h"
using namespace std;

#define BLOCKCOUNT 10000
class BufferCache
{
private:
	hash_map<unsigned __int64, Block*> bc;
	LRUQueue<unsigned __int64> que;
	
	struct stFlag 	{ unsigned char blkUsing : 1; };
	
	struct DBT {
		struct stFlag usingDescriptor[BLOCKCOUNT];
	}dbt;

	void setBlkDescriptor(int idx);
public:
	BufferCache(hash_map<unsigned __int64, Block*> temp)
	{
	}
	
	Block* findBlock(unsigned __int64 ba);
	bool insertBlock2Cache(unsigned __int64 ba, Block* blk); // false: fail because of size -> require to delete.
	unsigned __int64 newBlock();
	int getBlkDescriptor(int idx);
	bool BufferCache::getDeleteBlock(unsigned __int64& rtba, Block& rtblk);
	void deleteDirty(unsigned __int64 ba);
	bool initFunction(/*const super block* */);

};