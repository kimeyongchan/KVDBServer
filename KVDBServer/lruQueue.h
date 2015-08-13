//
//  lruQueue.h
//  radixTreeCache
//
//  Created by SonHojun on 8/13/15.
//  Copyright (c) 2015 SonHojun. All rights reserved.
//

#ifndef __LRUQUEUE__
#define __LRUQUEUE__

#include <iostream>
#include <list>
using namespace std;

template<typename T>
class LruQueue
{
private:
    list<T> que;
    T findData(T data);
public:
    void insertQueue(T data);
    void deleteQueue(T data);
    void updateQueue(T data);
    
};
#endif /* defined(__radixTreeCache__lruQueue__) */
