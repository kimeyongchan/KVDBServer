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
    
    // virtual 함수 구현
    uint16_t getDataSize()
    {
        //return (uint16_t)(sizeof(formatFlag) + sizeof(keyLen) + keyLen + sizeof(valueLen) + valueLen+ sizeof(indBlockAddress));
        return (uint16_t)sizeof(KeyValueChainingData);
    }
};

#endif // __KEY_VALUE_CHAINING_DATA_H__