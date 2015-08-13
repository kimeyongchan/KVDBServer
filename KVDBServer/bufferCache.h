#include<map>
#include<list>

#include "Block.h"
#include "SuperBlock.h"
#include "Defines.h"
using namespace std;

#define BLOCKCOUNT 10000
class BufferCache
{
private:
	map<uint64_t, Block*> bc;
    list<uint64_t> bufferQueue;

	struct stFlag 	{ unsigned char blkUsing : 1; };
	
	struct DBT {
		struct stFlag usingDescriptor[BLOCKCOUNT];
	}dbt;

public:
	BufferCache(map<uint64_t, Block*> temp)
	{
	}
    
    BufferCache(SuperBlock* spBlock)
    {
        
    }
	
	Block* findBlock(uint64_t ba);
	bool insertBlock2Cache(uint64_t ba, Block* blk); // false: fail because of size -> require to delete.
	uint64_t newBlock();
	int getBlkDescriptor(int idx);
	bool getDeleteBlock(uint64_t& rtba, Block& rtblk);
	void deleteDirty(uint64_t ba);
	bool initFunction(const SuperBlock* spBlock);

};