#include"bufferCache.h"

using namespace std;

Block* BufferCache::findBlock(uint64_t ba)
{
	map<uint64_t, Block*>::iterator it;
	
	for (it = this->bc.begin(); it != this->bc.end(); ++it)
	{
		if(it->first == ba)
        {
            list<uint64_t>::iterator itList;
            for(itList= bufferQueue.begin(); itList != bufferQueue.end(); ++itList)
            {
                    if((*itList) == ba)
                    {
                        break;
                    }
                
            }
            
            this->bufferQueue.remove((*itList));
            this->bufferQueue.push_back((*itList));
            
            return it->second;
        }
    }

	return NULL;
}

bool BufferCache::insertBlock2Cache(uint64_t ba, Block* blk)
{
	if (this->bc.size() < this->spB->getBlockCount())
	{
        this->bufferQueue.push_back(ba);
		this->bc.insert(map<uint64_t, Block*>::value_type(ba, blk));
		return true;
	}
	else
	{
		return false;  //MAXSIZE -> deleteBlock.
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
    
    if (this->bc.size() >= this->spB->getBlockCount())
    {
        return NULL; //need arrange
    }
    
    char* bitArr = this->spB->getUsingBlockBitArray();
	for (int i = 0; i < this->spB->getBlockCount(); i++)
	{
		if (bitArr[i] == 0)
		{
            uint64_t ba =(i+1)*spB->getBlockSize();  // index * block size -> addr?
            this->bufferQueue.push_back(ba);
            return ba;
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

void BufferCache::setBitArrayFlag(uint64_t ba)
{
    uint64_t idx = (ba / BLOCK_SIZE )- 1;
    char* bitArr = this->spB->getUsingBlockBitArray();
    bitArr[idx]? bitArr[idx] = 0 : bitArr[idx] = 1;
    
}
