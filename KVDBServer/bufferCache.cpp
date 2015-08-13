#include"bufferCache.h"

#define MAXSIZE 10000

using namespace std;

Block* BufferCache::findBlock(uint64_t ba)
{
	map<uint64_t, Block*>::iterator it;
	
	for (it = this->bc.begin(); it != this->bc.end(); ++it)
	{
		if(it->first == ba)
            return it->second;
	}

	return NULL;
}

bool BufferCache::insertBlock2Cache(uint64_t ba, Block* blk)
{
	if (this->bc.size() < MAXSIZE)
	{
	//	this->que.insertQueue(ba);
		this->bc.insert(map<uint64_t, Block*>::value_type(ba, blk));
		return true;
	}
	else
	{
		return false;  //MAXSIZE
	}
}


bool BufferCache::getDeleteBlock(uint64_t& rtba,Block& rtblk)  //arrange : ba, block
{
    uint64_t ba	= this->bufferQueue.front();
	Block* temp = findBlock(ba);

	if (temp->getDirty())
	{
		rtba = ba;
		rtblk = *temp;
		return true; //dirty Block
	}
	else
	{
        this->bufferQueue.remove(ba);
		delete temp;
		this->bc.erase(ba);
		return false; //
	}
}


uint64_t BufferCache::newBlock()
{
	for (int i = 0; i < BLOCKCOUNT; i++)
	{
		if (this->dbt.usingDescriptor[i].blkUsing == 0)
		{
			return i*BLOCKSIZE;
		}
	}

	return NULL;  // do not allocate new Block
}
void BufferCache::deleteDirty(uint64_t ba)
{
	//que remove
	this->bufferQueue.remove(ba);
	delete this->findBlock(ba);
	//cache remove
	this->bc.erase(ba);

}
