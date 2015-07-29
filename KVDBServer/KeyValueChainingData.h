#ifndef __KEY_VALUE_CHAINING_DATA_H__
#define __KEY_VALUE_CHAINING_DATA_H__

#include "KeyValueData.h"

class KeyValueChainingData : public KeyValueData
{
protected:
    int64_t m_indBlockAddress;
    
public:
    void setIndBlockAddress(int64_t indBlockAddress)
    {
        m_indBlockAddress = indBlockAddress;
    }
    
    long  getIndBlockAddress()
    {
        return m_indBlockAddress;
    }
    
    // virtual 함수 구현
    uint16_t getDataSize()
    {
        return (uint16_t)(sizeof(m_formatFlag) + sizeof(m_keyLen) + m_keyLen + sizeof(m_valueLen) + m_valueLen+ sizeof(m_indBlockAddress));
    }
};

#endif // __KEY_VALUE_CHAINING_DATA_H__