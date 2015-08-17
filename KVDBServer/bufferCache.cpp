#include"bufferCache.h"

using namespace std;

Block* BufferCache::findBlock(uint64_t ba)
{
	map<uint64_t, Block*>::iterator it;
	
	for (it = this->bc.begin(); it != this->bc.end(); ++it)
	{
		if(it->first == ba)
        {
            for(list<uint64_t>::iterator it = bufferQueue.begin(); it != bufferQueue.end(); ++it)
            {
                    if((*it) == ba)
                    {
                        this->bufferQueue.remove((*it));
                        this->bufferQueue.push_back((*it));
                    }
                
            }
            
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
            uint64_t ba =i*spB->getBlockSize();  // index * block size -> addr?
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
