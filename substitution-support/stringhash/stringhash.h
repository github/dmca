//
//  stringhash.h
//  C based hash using C strings
//
//  Created by Sam Bingner on 09/14/2019.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#ifndef _STRINGHASH_H
#define _STRINGHASH_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/queue.h>

typedef struct stringhash* stringhash_t;

// Remember to call free() on any non-NULL return value from any function
//   with "copy" in the name
//
// Returns NULL on failure

stringhash_t stringhash_create(size_t size);
void stringhash_destroy(stringhash_t hash);
bool stringhash_hasKey(stringhash_t hash, const char *key);
char *stringhash_copyValueForKey(stringhash_t hash, const char *key);
void stringhash_setKey(stringhash_t hash, const char *key, const char *value);
void stringhash_removeKey(stringhash_t hash, const char *key);
size_t stringhash_count(stringhash_t hash);
char **stringhash_copyAllKeys(stringhash_t hash);

#endif // _STRINGHASH_H
