#ifndef __KEY_VALUE_DATA_H__
#define __KEY_VALUE_DATA_H__

#include "Data.h"

class KeyValueData : public Data
{
protected:
    uint32_t valueLen;  // 42억
    std::string value;
    
    bool setValue(std::string value)
    {
        if(value.length() > UINT32_MAX)
            return false;
        
        valueLen = (uint32_t)value.length();
        value = value;
        
        return true;
    }
    
    std::string getValue()
    {
        return value;
    }
    
    // virtual 함수 구현
    uint16_t getDataSize()
    {
        return (uint16_t)(sizeof(formatFlag) + sizeof(keyLen) + keyLen + sizeof(valueLen) + valueLen);
    }
};

#endif // __KEY_VALUE_DATA_H__