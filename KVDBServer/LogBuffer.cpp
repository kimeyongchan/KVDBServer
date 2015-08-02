//
//  LogBuffer.cpp
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#include "LogBuffer.h"
#include "RequestInfo.h"
#include "Log.h"

LogBuffer::LogBuffer()
{
    
}

bool LogBuffer::initialize()
{
    logbufferStorage.clear();
    return true;
}

const char* LogBuffer::sendLogToLogFile()
{
    return logbufferStorage.c_str();
}

bool LogBuffer::saveRequestInfo(const RequestInfo* reqInfo)
{
    std::string reqInfoString = unParsing(reqInfo);
    
    if(reqInfoString.empty())
        return false;

    logbufferStorage = logbufferStorage.append(reqInfoString);
    
    return true;
}

std::string LogBuffer::unParsing(const RequestInfo* reqInfo)
{
    std::string unParsingString = "";
    
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
    
    return unParsingString;
}