//
//  lruQueue.cpp
//  radixTreeCache
//
//  Created by SonHojun on 8/13/15.
//  Copyright (c) 2015 SonHojun. All rights reserved.
//

#include "lruQueue.h"

template<typename T>
T LruQueue<T>::findData(T data)
{
    
    for(typename list<T>::iterator it = this->que.begin(); it != this->que.end() ; ++it)
    {
        if((*it) == data)
        {
            return (*it);
        }
    }
    
    
    return NULL;
    
    
}

template<typename T>
void LruQueue<T>::insertQueue(T data)
{
    this->que.push_back(data);
}


template<typename T>
void LruQueue<T>::deleteQueue(T data)
{
    T dData = findData(data);
    this->que.erase(dData);
}

template<typename T>
void LruQueue<T>::updateQueue(T data)
{
    T upData = findData(data);
    this->que.erase(upData);
    this->que.push_back(data);
}
