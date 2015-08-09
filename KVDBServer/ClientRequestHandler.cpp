#include "ClientRequestHandler.h"

#include <cstring>
#include <string>

#include "RequestInfo.h"
#include "WorkerThread.h"
#include "KVDBServer.h"
#include "Log.h"


#include <unistd.h>

ClientRequestHandler::ClientRequestHandler()
{
	memset(m_recvBuffer, 0, BUF_SIZE);
}


ClientRequestHandler::~ClientRequestHandler()
{

}


bool ClientRequestHandler::Initialize()
{
    m_cutOffQueryMap.clear();
	return true;
}


void ClientRequestHandler::Parse(const ConnectInfo* connectInfo, const char* query, int queryLength)
{
/*    sleep(10);
	RequestInfo* ri = NULL;
    int trimIndex = 0;
    int i = 0;
    const char* pointingQuery = query;
    
    for(i = 0; i < queryLength; i++) // divide by semicolon
    {
        if(query[i] == ';')
        {
            itr = m_cutOffQueryMap.find(connectInfo->fd);
            
            if(itr != m_cutOffQueryMap.end()) // if rest query exist
            {
                int savedQueryLen = (int)itr->second.length();
                std::string completeQuery(itr->second.c_str());
                completeQuery += std::string(pointingQuery, i - trimIndex + 1);
                
                m_cutOffQueryMap.erase(itr);
                
                if(ParsingQueryToRequestInfo(completeQuery.c_str(), savedQueryLen + i - trimIndex + 1, &ri) == false)
                {
                    DebugLog("errorformat : %s", completeQuery.c_str());
                }
                else
                {
                    KVDBServer::getInstance()->SendWorkToWorkerThread(ri);
                }
            }
            else // rest query not exist
            {
                if(ParsingQueryToRequestInfo(pointingQuery, i - trimIndex + 1, &ri) == false)
                {
                    DebugLog("errorformat : %s", pointingQuery);
                }
                else
                {
                    KVDBServer::getInstance()->SendWorkToWorkerThread(ri);
                }
            }
            
            ri = NULL;
            trimIndex = i + 1;
            pointingQuery = query + trimIndex;
            
        }
    }
    
    if( i != 0 && query[i-1] != ';' ) //not receive complete query
    {
        std::string tempString(pointingQuery, i - trimIndex);
        
        itr = m_cutOffQueryMap.find(connectInfo->fd);
        if(itr != m_cutOffQueryMap.end()) // if rest query exist, concatenation
        {
            std::string tempString2(itr->second.c_str());
            tempString = tempString2.append(tempString);
            m_cutOffQueryMap.erase(itr);
        }

        if(m_cutOffQueryMap.insert(std::pair<int, std::string>(connectInfo->fd, tempString)).second == false)
        {
            ErrorLog("insertError");
        }
    }*/
}

void ClientRequestHandler::IsConnected(const ConnectInfo* connectInfo)
{
    
}

void ClientRequestHandler::IsDisconnected(const ConnectInfo* connectInfo)
{
    
}


bool ClientRequestHandler::ParsingQueryToRequestInfo(const char* query, const int queryLength, RequestInfo** pri)
{
    std::string queryString(query, queryLength);
    
    std::size_t findTypeLength = queryString.find_first_of('(');
    
    if(findTypeLength == std::string::npos)
    {
        return false;
    }
    
    std::size_t findDataLength = queryString.find_first_of(')');
    
    
    if(queryString.substr(findDataLength+ 1).compare(";") != 0)
    {
        return false;
    }
    
    if(findDataLength == std::string::npos)
    {
        return false;
    }
    
    if(findTypeLength + 2 >= findDataLength)
    {
        return false;
    }
    std::string ee = queryString.substr(findTypeLength + 1, 1);
    if(queryString.substr(findTypeLength + 1, 1).compare("\"") != 0
       || queryString.substr(findDataLength + -1, 1).compare("\"") != 0)
    {
        ErrorLog("\" error ");
        return false;
    }
    
    std::string typeString = queryString.substr(0, findTypeLength);
    std::string dataString = queryString.substr(findTypeLength + 1, findDataLength - findTypeLength - 1);
    
    if(typeString.compare("insert") == 0) // insert
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.substr(findDQM + 1, 2).compare(",\"") == 0) // file insert
        {
            std::size_t findDQM2 = dataString.find_first_of('"', findDQM + 3);
            
            if(findDQM2 == std::string::npos)
            {
                return false;
            }
            
            std::string secondString = dataString.substr(findDQM + 3, findDQM2 - findDQM - 3);
            
            if(secondString.empty())
            {
                return false;
            }
            
            if (dataString.length() != findDQM2 + 1)
            {
                return false;
            }
            
            InsertRequestInfo* iri = new InsertRequestInfo();
            iri->key = firstString.substr();
            iri->value = secondString.substr();
            
            *pri = iri;
            
            return true;
        
        }
        else if(dataString.length() == findDQM + 1) //directory insert
        {
            InsertDirectoryRequestInfo* idri = new InsertDirectoryRequestInfo();
            idri->key = firstString.substr();
            
            *pri = idri;
            
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(typeString.compare("find") == 0) // find
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.length() != findDQM + 1)
        {
            return false;
        }
        
        FindRequestInfo* fri = new FindRequestInfo();
        fri->key = firstString.substr();
        
        *pri = fri;
        
        return true;
    }
    else if(typeString.compare("delete") == 0) // delete
    {
        std::size_t findDQM = dataString.find_first_of('"', 1);
        
        std::string firstString = dataString.substr(1, findDQM - 1);
        
        if(firstString.empty())
        {
            return false;
        }
        
        if(dataString.length() != findDQM + 1)
        {
            return false;
        }
        
        DeleteRequestInfo* dri = new DeleteRequestInfo();
        dri->key = firstString.substr();
        
        *pri = dri;
        
        return true;
    }
    else
    {
        return false;
    }
    
    return true;
}