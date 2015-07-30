#ifndef __KEY_VALUE_DATA_H__
#define __KEY_VALUE_DATA_H__

#include "Data.h"

class KeyValueData : public Data
{

public:
    virtual ~KeyValueData(){}
    
protected:
    //uint32_t valueLen;  // 42억
    std::string value;
    
    bool setValue(std::string value)
    {
        if(value.length() > UINT32_MAX)
            return false;
        
        //valueLen = (uint32_t)value.length();
        this->value = value;
        
        return true;
    }
    
    std::string getValue()
    {
        return value;
    }
    
    // virtual 함수 구현
    virtual uint16_t getDataSize()
    {
         return (uint16_t)( sizeof(formatType) + key.size() + value.size() );
    }
};

#endif // __KEY_VALUE_DATA_H__