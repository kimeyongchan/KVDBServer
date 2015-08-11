#ifndef __KEY_VALUE_CHAINING_DATA_H__
#define __KEY_VALUE_CHAINING_DATA_H__

#include "KeyValueData.h"

class KeyValueChainingData : public KeyValueData
{
protected:
	int64_t indBlockAddress;

public:

	virtual ~KeyValueChainingData(){}

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
		return (uint16_t)(sizeof(formatType)+sizeof(int8_t)/*Ű������*/ +key.size()
			+ sizeof(int32_t)/*���������*/ +value.size() + sizeof(indBlockAddress));
	}
};

#endif // __KEY_VALUE_CHAINING_DATA_H__