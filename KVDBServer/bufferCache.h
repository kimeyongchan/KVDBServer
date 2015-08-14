#include<map>
#include<list>

#include "Block.h"
#include "SuperBlock.h"
#include "Defines.h"
using namespace std;

class BufferCache
{
private:
	map<uint64_t, Block*> bc;
    list<uint64_t> bufferQueue;
    SuperBlock* spB;
public:
    BufferCache(SuperBlock* spBlock)
    {
        this->spB = spBlock;
        
    }
	
	Block* findBlock(uint64_t ba);
	bool insertBlock2Cache(uint64_t ba, Block* blk); // false: fail because of size -> require to delete.
	uint64_t newBlock();
	
	bool getDeleteBlock(uint64_t& rtba, Block& rtblk);
	void deleteDirty(uint64_t ba);
    
    void initialize(SuperBlock* spBlock)
    {
        this->spB = spBlock;
    }
};