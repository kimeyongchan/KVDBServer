#ifndef __DIRECTORY_DATA_H__
#define __DIRECTORY_DATA_H__

#include "Data.h"

class DirectoryData : public Data
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
        return (uint16_t)(sizeof(formatFlag) + sizeof(keyLen) + keyLen + sizeof(indBlockAddress));
    }
};

#endif // __DIRECTORY_DATA_H__
