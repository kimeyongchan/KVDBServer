#ifndef __BUFFERCACHE__
#define __BUFFERCACHE__

#include<map>
#include<list>

#include "Block.h"
#include "SuperBlock.h"
#include "Defines.h"
using namespace std;

class BufferCache
{
private:
	map<uint64_t, Block*> bc;  //concurrent map??
    list<uint64_t> bufferQueue;
    SuperBlock* spB;
public:
    BufferCache(SuperBlock* spBlock)
    {
        this->spB = spBlock;
        insertBlock2Cache(this->spB->getRootBlockAddress(), this->spB->getRootBlock());
        
    }
	
	Block* findBlock(uint64_t ba);
	bool insertBlock2Cache(uint64_t ba, Block* blk); // false: fail because of size -> require to delete.
	uint64_t newBlock();
	
	bool getDeleteBlock(uint64_t& rtba, Block** rtblk);
	void deleteDirty(uint64_t ba);
    void setBitArrayFlag(uint64_t ba);
  
};

#endif