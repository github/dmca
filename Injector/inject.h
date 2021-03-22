/*
 *  inject.h
 *  
 *  Created by Sam Bingner on 9/27/2018
 *  Copyright 2018 Sam Bingner. All Rights Reserved.
 *
 */

#ifndef _INJECT_H_
#define _INJECT_H_
#include <Foundation/Foundation.h>

NSString *cdhashFor(NSString *file);
int injectTrustCache(NSArray <NSString*> *files, uint64_t trust_chain, int (*pmap_load_trust_cache)(uint64_t, size_t));
bool isInAMFIStaticCache(NSString *path);

#endif
