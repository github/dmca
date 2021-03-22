//
//  stringhash.c
//  A pure C hash of strings
//
//  Created by Sam Bingner on 09/14/2019.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/errno.h>
#include "stringhash.h"
#include "stringarray.h"

// Example / test code
int main(int argc, char *argv[])
{
    uint64_t t1,t2;
    uint64_t avg = 0;
    size_t hashsize=0x200000, testsize=0x100000;

    if (argc>1) {
        testsize = strtoll(argv[1], NULL, 0);
        printf("Set test size to %zu\n", testsize);
    }
    if (argc>2) {
        hashsize = strtoll(argv[2], NULL, 0);
        printf("Set hash size to %zu\n", hashsize);
    }
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    stringhash_t hash = stringhash_create(hashsize);
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    printf("Created hash in %1.3f ms\n", (t2-t1)/1000000.0);
    stringhash_setKey(hash, "test", "has a value");
    char *value = stringhash_copyValueForKey(hash, "test");
    printf("value for hash{test}: \"%s\"\n", value);
    if (value) free(value);
    stringhash_setKey(hash, "test", "has a new value");
    value = stringhash_copyValueForKey(hash, "test");
    printf("value for hash{test}: \"%s\"\n", value);
    if (value) free(value);
    printf("Number of entries in hash: %zu\n", stringhash_count(hash));
    char key[65];
    size_t i;
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    for (i=0; i<testsize; i++) {
        snprintf(key, 64, "%zu", i);
        stringhash_setKey(hash, key, key);
    }
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    avg = (t2 - t1) / (i + 1);
    printf("Tested %zu sets in %llu ms.  Average time %f ms\n", i, (t2-t1)/1000000, avg/1000000.0);
    size_t count = stringhash_count(hash);
    if (count != i+1) printf("Error incorrect count\n");
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    for (i=0; i<testsize; i++) {
        snprintf(key, 64, "%zu", i);
        char *value = stringhash_copyValueForKey(hash, key);
        if (!value || strcmp(value, key) != 0) {
           printf("Error unexpected key value for key %s\n", key);
           break;
        }
        free(value);
    }
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    avg = (t2 - t1) / (i + 1);
    printf("Tested %zu retrieves in %llu ms.  Average time %f ms\n", i, (t2-t1)/1000000, avg/1000000.0);
    printf("Number of entries in hash: %zu\n", stringhash_count(hash));
    value = stringhash_copyValueForKey(hash, "test");
    printf("value for hash{test}: \"%s\"\n", value);
    free(value);
    stringhash_removeKey(hash, "test");
    count--;
    value = stringhash_copyValueForKey(hash, "test");
    printf("value for hash{test}: \"%s\"\n", value);
    char **allKeys = stringhash_copyAllKeys(hash);
    i=0;
    if (allKeys) {
        for (char **key = allKeys; *key; key++) {
            i++;
            char *value = stringhash_copyValueForKey(hash, *key);
            // printf("Key %zu/%zu: \"%s\" = \"%s\"\n", i, count-1, *key, stringhash_copyValueForKey(hash, *key));
            if (!value) {
                printf("Error: unable to retrieve value for key %s (%zu/%zu)\n", *key, i, count);
                return -1;
            }
            free(value);
        }
        free(allKeys);
    }
    printf("Destroying hash\n");
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    stringhash_destroy(hash);
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    printf("Destroyed hash in %1.3f ms\n", (t2-t1)/1000000.0);
    printf("Hash Tests Completed\n");
    stringarray_t array = stringarray_create();
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    for (i=0; i<testsize; i++) {
        snprintf(key, 64, "value%zu", i);
        stringarray_push(array, key);
    };
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    printf("Created array in %1.3f ms\n", (t2-t1)/1000000.0);
    value = stringarray_copyValue(array, 0);
    printf("value for array[0]: \"%s\"\n", value);
    if (value) free(value);
    t1 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    value = stringarray_copyValue(array, testsize/2);
    free(value);
    t2 = clock_gettime_nsec_np(CLOCK_MONOTONIC);
    value = stringarray_pop(array);
    printf("value for pop array: \"%s\"\n", value);
    free(value);
    value = stringarray_pop(array);
    printf("value for pop array: \"%s\"\n", value);
    free(value);
    value = stringarray_shift(array);
    printf("value for shift array: \"%s\"\n", value);
    free(value);
    value = stringarray_shift(array);
    printf("value for shift array: \"%s\"\n", value);
    free(value);
    printf("iterated array in %1.3f ms\n", (t2-t1)/1000000.0);
    char **values = stringarray_copyAllValues(array);
    if (values) {
        printf("values[0] = %s\n", values[0]);
        printf("values[%zu] = %s\n", stringarray_count(array)-1, values[stringarray_count(array)-1]);
        free(values);
    }
    stringarray_destroy(array);
}
