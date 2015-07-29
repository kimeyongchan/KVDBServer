#ifndef __DIRECTORY_DATA_H__
#define __DIRECTORY_DATA_H__

#include "Data.h"

class DirectoryData : public Data
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
        return (uint16_t)(sizeof(m_formatFlag) + sizeof(m_keyLen) + m_keyLen + sizeof(m_indBlockAddress));
    }
};

#endif // __DIRECTORY_DATA_H__
