#ifndef __DATA_H__
#define __DATA_H__

#include <cstdint>
#include <limits.h>
#include <string>


#define FLAG_DIRECTORY_DATA             0// 0: 폴더형 데이터 포맷,
#define FLAG_KEY_VALUE_DATA             1// 1: key/value 데이터포맷이면서 chaining 안함
#define FLAG_KEY_VALUE_CHAINING_DATA    2 // 2: key/value 데이터포맷이면서 chaining 함


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
    
    int8_t getFormatType() const
    {
        return formatType;
    }
    
    uint8_t getKeyLength() const
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
    
    virtual uint16_t getDataSize() const = 0;
};

#endif // __DATA_H__