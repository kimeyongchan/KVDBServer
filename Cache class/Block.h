
#include <map>

#define BLOCK_FIRST_FREE_SPACE 8192 - (sizeof(indirectionCnt)+ sizeof(freeSpace) + sizeof(chainingAddress))
#define BLOCKSIZE 8*1024*1024
class Data;

class IndirectionData
{
public:
	IndirectionData(unsigned __int16 offset, Data* data)
	{
		this->offset = offset;
		this->data = data;
	}
	unsigned __int16 offset;
	Data* data;
};

class Block
{

private:

	unsigned __int16  indirectionCnt;   // 0~65535개 , 인다이렉션 개수
	unsigned __int16  freeSpace;       //  블럭 잔여량  (8k = 8192byte)
	unsigned __int64   chainingAddress;  // 블럭 체이닝 주소
	bool isDirty;

	std::map<unsigned __int16, IndirectionData*> indirectionDataMap; //  <인디렉션넘버, indirectionData>



	void increaseIndirectionCnt()
	{
		++indirectionCnt;
	}

	void decreaseIndirctionCnt()
	{
		--indirectionCnt;
	}

public:
	Block()
	{
		indirectionCnt = 0;
		freeSpace = BLOCK_FIRST_FREE_SPACE;
		chainingAddress = NULL;
		indirectionDataMap.clear();
	}

	void setIndirectionCnt(unsigned __int16 indirectionCnt)
	{
		this->indirectionCnt = indirectionCnt;
	}
	void setFreeSpace(unsigned __int16 freeSpace)
	{
		this->freeSpace = freeSpace;
	}

	unsigned __int16 getIndirectionCnt() const
	{
		return indirectionCnt;
	}

	void calculateFreeSpace(unsigned __int16 dataSize, unsigned __int16 offsetSize, bool isIncrease)
	{
		if (isIncrease == true)
			freeSpace += (dataSize + offsetSize);
		else
			freeSpace -= (dataSize + offsetSize);
	}

	unsigned __int64 getFreeSpace() const
	{
		return freeSpace;
	}


	void setChainingAddress(unsigned __int64 address)
	{
		chainingAddress = address;
	}

	unsigned __int64 getChaingAddress() const
	{
		return chainingAddress;
	}

	bool checkOffset(unsigned __int64 offset)
	{
		for (auto iter : indirectionDataMap)
		if (iter.second->offset == offset)
			return true;

		return false;
	}

	bool insertData(unsigned __int64 idx, unsigned __int64 offset, Data* data);
	bool deleteData(unsigned __int64 offset);
	Data* getData(unsigned __int64 offset);

	const std::map<unsigned __int16, IndirectionData*>* getIndirectionDataMap() const
	{
		return &indirectionDataMap;
	}

	bool getDirty()
	{
		return this->isDirty;
	}

};
