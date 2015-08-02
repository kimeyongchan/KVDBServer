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
    int8_t      formatType;
    //uint8_t     keyLen;
    std::string key;
    
public:
    
    virtual ~Data(){}
    
    bool setFormatType(int8_t type)
    {
        if(type > 2)
            return false;
        
        formatType = type;
        
        return true;
    }
    
    int8_t getFormatType()
    {
        return formatType;
    }
    
    uint8_t getKeyLength()
    {
        return key.length();
    }
    
    bool setKey(std::string key)
    {
        if(key.length() > UINT8_MAX)
            return false;
        
        //keyLen = key.length();
        this->key = key;
        return true;
    }
    
    std::string getKey() const
    {
        return key;
    }
    
    virtual uint16_t getDataSize() = 0;
};

#endif // __DATA_H__