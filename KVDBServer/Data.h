#ifndef __DATA_H__
#define __DATA_H__

#include <cstdint>
#include <limits.h>
#include <string>

// 0: 폴더형 데이터 포맷,
// 1: key/value 데이터포맷이면서 chaining 안함
// 2: key/value 데이터포맷이면서 chaining 함

class Data
{
protected:
    int8_t m_formatFlag;
    uint8_t m_keyLen;
    std::string m_key;
    
public:
    void setFormatFlag(int8_t flag)
    {
        m_formatFlag = flag;
    }
    
    char getFormatFlag()
    {
        return m_formatFlag;
    }
    
    uint8_t getKeyLength()
    {
        return m_keyLen;
    }
    
    bool setKey(std::string key)
    {
        if(key.length() > UINT8_MAX)
            return false;
        
        m_keyLen = key.length();
        m_key = key;
        return true;
    }
    
    virtual uint16_t getDataSize() = 0;
};

#endif // __DATA_H__