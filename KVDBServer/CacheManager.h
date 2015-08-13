//
//  CacheMgr.h
//  radixTreeCache
//
//  Created by SonHojun on 8/12/15.
//  Copyright (c) 2015 SonHojun. All rights reserved.
//

#ifndef __radixTreeCache__CacheMgr__
#define __radixTreeCache__CacheMgr__

#include <stdio.h>
#include "namedcache.h"
#include "SuperBlock.h"
#include "bufferCache.h"

class CacheMgr
{
    
private:
    NamedCache nc;
    SuperBlock sb;
    BufferCache bc;
public:
    
};
#endif /* defined(__radixTreeCache__CacheMgr__) */
