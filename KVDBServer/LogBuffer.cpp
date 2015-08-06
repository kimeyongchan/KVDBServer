//
//  LogBuffer.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "LogBuffer.h"

#include "Log.h"

LogBuffer::LogBuffer()
{
    
}

bool LogBuffer::initialize()
{
    logbufferStorage.clear();
    return true;
}

const char* LogBuffer::readLogBuffer()
{
    return logbufferStorage.c_str();
}

bool LogBuffer::saveLog(bool isAllocateBlock, bool isInsert, int64_t IndBlockAddress, uint16_t offset, const Data* data)
{
    return true;
}

const char* LogBuffer::DataToArray(const Data* data)
{
/*    std::string unParsingString = "";
    
    int type = reqInfo->type;
    
    if(type == INSERT_REQUEST)
    {
        unParsingString += "insert(";
        InsertRequestInfo* iri = (InsertRequestInfo*)reqInfo;
        unParsingString += "\"";
        unParsingString += iri->key;
        unParsingString += "\",\"";
        unParsingString += iri->value;
        unParsingString += "\");";
        
    }
    else if(type == INSERT_DIRECTORY_REQUEST)
    {
        unParsingString += "insert(";
        InsertDirectoryRequestInfo* idri = (InsertDirectoryRequestInfo*)reqInfo;
        unParsingString += "\"";
        unParsingString += idri->key;
        unParsingString += "\");";
    }
    else if(type == FIND_REQUEST)
    {
        unParsingString += "find(";
        FindRequestInfo* fri = (FindRequestInfo*)reqInfo;
        unParsingString += "\"";
        unParsingString += fri->key;
        unParsingString += "\");";
        
    }
    else if(type == DELETE_REQUEST)
    {
        unParsingString += "delete(";
        DeleteRequestInfo* dri = (DeleteRequestInfo*)reqInfo;
        unParsingString += "\"";
        unParsingString += dri->key;
        unParsingString += "\");";
    }
    else
    {
        ErrorLog("invalid type - %d", type);
    }
    
    return unParsingString;*/
    return NULL;
}