#ifndef __KEY_VALUE_DATA_H__
#define __KEY_VALUE_DATA_H__

#include "Data.h"

class KeyValueData : public Data
{
protected:
    uint32_t m_valueLen;  // 42억
    std::string m_value;
    
    bool setValue(std::string value)
    {
        if(value.length() > UINT32_MAX)
            return false;
        
        m_valueLen = (uint32_t)value.length();
        m_value = value;
        
        return true;
    }
    
    std::string getValue()
    {
        return m_value;
    }
    
    // virtual 함수 구현
    uint16_t getDataSize()
    {
        return (uint16_t)(sizeof(m_formatFlag) + sizeof(m_keyLen) + m_keyLen + sizeof(m_valueLen) + m_valueLen);
    }
};

#endif // __KEY_VALUE_DATA_H__