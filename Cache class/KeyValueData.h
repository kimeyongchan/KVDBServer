#ifndef __KEY_VALUE_DATA_H__
#define __KEY_VALUE_DATA_H__

#include "Data.h"

class KeyValueData : public Data
{

public:
	virtual ~KeyValueData(){}

protected:
	//uint32_t valueLen;  // 42��
	std::string value;

public:
	bool setValue(std::string value)
	{
		if (value.length() > UINT32_MAX)
			return false;

		//valueLen = (uint32_t)value.length();
		this->value = value;

		return true;
	}

	std::string getValue() const
	{
		return value;
	}

	// virtual �Լ� ����
	virtual uint16_t getDataSize()
	{
		return (uint16_t)(sizeof(formatType)+sizeof(int8_t)+key.size() + sizeof(int32_t)+value.size());
	}
};

#endif // __KEY_VALUE_DATA_H__