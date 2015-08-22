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


bool BufferCache::getDeleteBlock(uint64_t& rtba,Block** rtblk)  //arrange : ba, block
{
    uint64_t ba	= this->bufferQueue.front();
	Block* temp = findBlock(ba);

	if (temp->getDirty())
	{
		rtba = ba;
		rtblk = &temp;
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
    /*
	for (int i = 0; i < this->spB->getBlockCount(); i++)
	{
		if (bitArr[i] == 0)
		{
            uint64_t ba =(i+1)*spB->getBlockSize();  // index * block size -> addr?
            this->bufferQueue.push_back(ba);
            return ba;
		}
	}
     */
    
    int i;
    
    for(i = 0; i < (this->spB->getBlockCount() / 8); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            unsigned char constBit = 0x80;
            
            unsigned char cmpBit = constBit >> j;
            
            if((bitArr[i] & cmpBit) == 0)
            {
                uint64_t ba =this->spB->getRootBlockAddress() + (((i * 8) + j) * spB->getBlockSize());  // index * block size -> addr?
                bitArr[i] |= cmpBit;
                this->bufferQueue.push_back(ba);
                return ba;
            }
        }
    }
    
    int restBitCount = this->spB->getBlockCount() % 8;
    
    if(restBitCount != 0) // rest
    {
        for(int j = 0; j < restBitCount; j++)
        {
            unsigned char constBit = 0x80;
            
            unsigned char cmpBit = constBit >> j;
            
            if((bitArr[i] & cmpBit) == 0)
            {
                uint64_t ba =this->spB->getRootBlockAddress() + (((i * 8) + j) * spB->getBlockSize());  // index * block size -> addr?
                bitArr[i] |= cmpBit;
                this->bufferQueue.push_back(ba);
                return ba;
            }
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

void BufferCache::setBitArrayFlag(uint64_t blockAdr)
{
//    uint64_t idx = ((ba - this->spB->getRootBlockAddress()) / BLOCK_SIZE )- 1;
//    char* bitArr = this->spB->getUsingBlockBitArray();
//    bitArr[idx]? bitArr[idx] = 0 : bitArr[idx] = 1;
    
    char* bitArr = this->spB->getUsingBlockBitArray();
    int i;
    for(i = 0; i < (this->spB->getBlockCount() / 8); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            unsigned char constBit = 0x80;
            unsigned char cmpBit = constBit >> j;
            
            if((bitArr[i] & cmpBit) == 0)
            {
                uint64_t ba =this->spB->getRootBlockAddress() + (((i * 8) + j) * spB->getBlockSize());  // index * block size -> addr?
                
                if(ba == blockAdr)
                {
                    bitArr[i] ^= cmpBit;
                    return;
                }
            }
        }
    }
    
    int restBitCount = this->spB->getBlockCount() % 8;
    
    if(restBitCount != 0) // rest
    {
        for(int j = 0; j < restBitCount; j++)
        {
            unsigned char constBit = 0x80;
            unsigned char cmpBit = constBit >> j;
            
            if((bitArr[i] & cmpBit) == 0)
            {
                uint64_t ba =this->spB->getRootBlockAddress() + (((i * 8) + j) * spB->getBlockSize());  // index * block size -> addr?
                
                if(ba == blockAdr)
                {
                    bitArr[i] ^= cmpBit;
                    return;
                }
            }
        }
    }
    
}




















