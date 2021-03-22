//
//  stringarray.h
//  C based array using C strings
//
//  Created by Sam Bingner on 09/14/2019.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#ifndef _STRINGARRAY_H
#define _STRINGARRAY_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/queue.h>

typedef struct stringarray* stringarray_t;

// Remember to call free() on any non-NULL return value from any function
//   with "copy" in the name
//
// Returns NULL on failure

stringarray_t stringarray_create(void);
void stringarray_destroy(stringarray_t array);
char *stringarray_copyValue(stringarray_t array, size_t idx);
bool stringarray_setIdx(stringarray_t array, size_t idx, const char *value);
bool stringarray_removeIdx(stringarray_t array, size_t idx);
char *stringarray_pop(stringarray_t array);
char *stringarray_shift(stringarray_t array);
bool stringarray_push(stringarray_t array, const char *value);
bool stringarray_unshift(stringarray_t array, const char *value);
bool stringarray_insertAfter(stringarray_t array, size_t idx, const char *value);
bool stringarray_insertBefore(stringarray_t array, size_t idx, const char *value);
size_t stringarray_count(stringarray_t array);
char **stringarray_copyAllValues(stringarray_t array);

#endif // _STRINGARRAY_H
