#include "Block.h"
#include "Data.h"
#include "DirectoryData.h"
#include "KeyValueData.h"
#include "KeyValueChainingData.h"

bool Block::insertData(uint64_t idx, uint64_t offset, Data* data)
{
	if (data == NULL)
		return false;

	uint16_t dataSize = data->getDataSize();


	if (freeSpace < dataSize)
		return false;

	if (checkOffset(offset))  // offset ������ ����
		return false;

	IndirectionData* iData = new IndirectionData(offset, data);


	if (indirectionDataMap.rbegin()->first == (indirectionDataMap.size() - 1))  // �������
	{
		indirectionDataMap.insert(std::pair<uint16_t, IndirectionData*>(indirectionDataMap.size(), iData));
		calculateFreeSpace(dataSize, 2, false);

	}
	else  // �߰��� ����ִ� ���
	{
		for (uint16_t i = 0; true; ++i)
		{
			if (indirectionDataMap.find(i) == indirectionDataMap.end())
			{
				indirectionDataMap.insert(std::pair<uint16_t, IndirectionData*>(i, iData));
				break;
			}
		}

		calculateFreeSpace(dataSize, 0, false);
	}

	increaseIndirectionCnt();

	return true;
}

bool Block::deleteData(uint64_t idx)
{
	auto iter = indirectionDataMap.find(idx);

	if (iter == indirectionDataMap.end())
		return false; //ã�� Ű ����.


	Data* data = iter->second->data;
	uint16_t dataSize = data->getDataSize();
	uint16_t freeSpaceSize = freeSpace + dataSize;

	if (BLOCK_FIRST_FREE_SPACE < freeSpaceSize)
		return false; // ������ ���� �ϸ� ����������� ������...


	if (iter->second->data != NULL)
		delete iter->second->data;

	if (iter->second != NULL)
		delete iter->second;


	indirectionDataMap.erase(iter);
	calculateFreeSpace(dataSize, 0, true);
	decreaseIndirctionCnt();

	return true;
}

Data* Block::getData(uint64_t idx)
{
	auto iter = indirectionDataMap.find(idx);

	if (iter == indirectionDataMap.end())
		return NULL; //ã�� ������


	return iter->second->data;
}



