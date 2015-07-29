#ifndef __KEY_VALUE_CHAINING_DATA_H__
#define __KEY_VALUE_CHAINING_DATA_H__

#include "KeyValueData.h"

class KeyValueChainingData : public KeyValueData
{
protected:
    int64_t indBlockAddress;
    
public:
    void setIndBlockAddress(int64_t indBlockAddress)
    {
        indBlockAddress = indBlockAddress;
    }
    
    long  getIndBlockAddress()
    {
        return indBlockAddress;
    }
    
    // virtual 함수 구현
    uint16_t getDataSize()
    {
        return (uint16_t)(sizeof(formatFlag) + sizeof(keyLen) + keyLen + sizeof(valueLen) + valueLen+ sizeof(indBlockAddress));
    }
};

#endif // __KEY_VALUE_CHAINING_DATA_H__