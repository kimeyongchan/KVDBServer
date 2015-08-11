#ifndef __DIRECTORY_DATA_H__
#define __DIRECTORY_DATA_H__

#include "Data.h"

class DirectoryData : public Data
{
protected:
	int64_t indBlockAddress;

public:

	virtual ~DirectoryData(){}

	void setIndBlockAddress(int64_t indBlockAddress)
	{
		this->indBlockAddress = indBlockAddress;
	}

	int64_t  getIndBlockAddress()
	{
		return indBlockAddress;
	}

	// virtual �Լ� ����
	uint16_t getDataSize()
	{
		return (uint16_t)(sizeof(formatType)+sizeof(int8_t)/*Ű������*/ +key.size() + sizeof(indBlockAddress));
	}
};

#endif // __DIRECTORY_DATA_H__
