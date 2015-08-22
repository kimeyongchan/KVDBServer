//
//  ClientServerPacket.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 19..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef KVDBServer_ClientServerPacket_h
#define KVDBServer_ClientServerPacket_h

#include <stdint.h>

#pragma pack(push, 1)

class ClientServerPacket
{
public:
    
    enum RESULT_TYPE
    {
        RESULT_TYPE_INSERT_FILE = 1,
        RESULT_TYPE_INSERT_DIRECTORY,
        RESULT_TYPE_FIND_FILE,
        RESULT_TYPE_FIND_DIRECTORY,
        RESULT_TYPE_DELETE_FILE,
        RESULT_TYPE_DELETE_DIRECTORY,
    };
    
    enum FILE_TYPE
    {
        FILE_TYPE_DIRECTORY = 0,
        FILE_TYPE_FILE,
    };
    
    
    struct KeyInfo
    {
        uint8_t fileType;
        uint8_t keyLen;
        char* key;
    };
    
    struct BasicPacket
    {
        uint8_t resultType;
    };
    
    struct InsertFileRes : public BasicPacket
    {
        InsertFileRes() { resultType = RESULT_TYPE_INSERT_FILE; }
        
        uint8_t resultCode;
    };
    
    struct InsertDirectoryRes : public BasicPacket
    {
        InsertDirectoryRes() { resultType = RESULT_TYPE_INSERT_DIRECTORY; }
        uint8_t resultCode;
    };
    
    struct FindFileRes : public BasicPacket
    {
        FindFileRes() { resultType = RESULT_TYPE_FIND_FILE; }
        
        uint8_t resultCode;
        uint32_t valueLen;
        char* value;
    };
    
    struct FindDirectoryRes : public BasicPacket
    {
        FindDirectoryRes() { resultType = RESULT_TYPE_FIND_DIRECTORY; }
        
        uint8_t resultCode;
        uint8_t keyCount;
        KeyInfo* keyInfo;
    };
    
    struct DeleteFileRes : public BasicPacket
    {
        DeleteFileRes() { resultType = RESULT_TYPE_DELETE_FILE; }
        
        uint8_t resultCode;
    };
    
    struct DeleteDirectoryRes : public BasicPacket
    {
        DeleteDirectoryRes() { resultType = RESULT_TYPE_DELETE_DIRECTORY; }
        
        uint8_t resultCode;
    };
};

#pragma pack(pop)


#endif
