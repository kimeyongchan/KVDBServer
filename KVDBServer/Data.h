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
    int8_t formatFlag;
    uint8_t keyLen;
    std::string key;
    
public:
    void setFormatFlag(int8_t flag)
    {
        formatFlag = flag;
    }
    
    char getFormatFlag()
    {
        return formatFlag;
    }
    
    uint8_t getKeyLength()
    {
        return keyLen;
    }
    
    bool setKey(std::string key)
    {
        if(key.length() > UINT8_MAX)
            return false;
        
        keyLen = key.length();
        key = key;
        return true;
    }
    
    virtual uint16_t getDataSize() = 0;
};

#endif // __DATA_H__