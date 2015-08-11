#include"bufferCache.h"

#define MAXSIZE 10000

using namespace std;

Block* BufferCache::findBlock(unsigned __int64 ba) 
{
	hash_map<unsigned __int64, Block*>::iterator it;
	
	for (it = this->bc.begin(); it != this->bc.end(); ++it)
	{
		return it->second;
	}

	return NULL;
}

bool BufferCache::insertBlock2Cache(unsigned __int64 ba, Block* blk)
{
	if (this->bc.size() < MAXSIZE)
	{
		this->que.insertQueue(ba);
		this->bc.insert(hash_map<unsigned __int64, Block*>::value_type(ba, blk));
		return true;
	}
	else
	{
		return false;  //MAXSIZE
	}
}


bool BufferCache::getDeleteBlock(unsigned __int64& rtba,Block& rtblk)  //arrange : ba, block
{
	unsigned __int64 ba	= this->que.getFront();
	Block* temp = findBlock(ba);

	if (temp->getDirty())
	{
		rtba = ba;
		rtblk = *temp;
		return true; //dirty Block
	}
	else
	{
		this->que.remove(ba);
		delete temp;
		this->bc.erase(ba);
		return false; //dirty�� �ƴѰ� �׳� ����
	}
}


unsigned __int64 BufferCache::newBlock()
{
	for (int i = 0; i < BLOCKCOUNT; i++)
	{
		if (this->dbt.usingDescriptor[i].blkUsing == 0)
		{
			return i*BLOCKSIZE;
		}
	}

	return NULL;  //�Ҵ��� �� ����
}
void BufferCache::deleteDirty(unsigned __int64 ba)
{

	//que���� ����
	this->que.remove(ba);
	delete this->findBlock(ba);
	//cache���� ����
	this->bc.erase(ba);

}
