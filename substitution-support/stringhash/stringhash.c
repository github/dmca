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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/queue.h>
#include "stringhash.h"

#define EXPORT __attribute__((visibility ("default")))
// hash_entry is a single structure consisting of the structre defined below followed
// by the key and the value - value is a direct pointer to hash->key + keylen + 1

typedef struct hash_entry {
    TAILQ_ENTRY(hash_entry) entries;
    size_t keylen;
    size_t valuelen;
    char *value;
    char key[];
} *hash_entry_t;

TAILQ_HEAD(hash_head, hash_entry);

struct stringhash {
    size_t size;
    struct hash_head heads[];
};

//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
// and endian-ness issues if used across multiple platforms.

// 64-bit hash for 64-bit platforms

static uint64_t MurmurHash64A ( const void * key, int len, uint64_t seed )
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    while(data != end)
    {
	uint64_t k = *data++;

	k *= m; 
	k ^= k >> r; 
	k *= m; 

	h ^= k;
	h *= m; 
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
	case 7: h ^= (uint64_t)(data2[6]) << 48;
	case 6: h ^= (uint64_t)(data2[5]) << 40;
	case 5: h ^= (uint64_t)(data2[4]) << 32;
	case 4: h ^= (uint64_t)(data2[3]) << 24;
	case 3: h ^= (uint64_t)(data2[2]) << 16;
	case 2: h ^= (uint64_t)(data2[1]) << 8;
	case 1: h ^= (uint64_t)(data2[0]);
		h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
} 

static size_t indexOf(stringhash_t hash, const char *key, size_t keylen) {
    return MurmurHash64A(key, strlen(key), 0xcafebabedeadbeef) % hash->size; // Random seed chosen by fair dice roll
}

EXPORT
stringhash_t stringhash_create(size_t size) {
    stringhash_t hash = malloc(sizeof(struct stringhash) + size * sizeof(struct hash_head));
    hash->size = size;
    for (size_t i=0; i<size; i++) {
        TAILQ_INIT(&hash->heads[i]);
    }
    return hash;
}

EXPORT
void stringhash_destroy(stringhash_t hash) {
    for (size_t i=0; i<hash->size; i++)  {
        hash_entry_t entry, temp;
        TAILQ_FOREACH_SAFE(entry, &hash->heads[i], entries, temp) {
            TAILQ_REMOVE(&hash->heads[i], entry, entries);
            free(entry);
        }
    }
    free(hash);
}

static hash_entry_t entryForKey(struct hash_head *head, const char *key, size_t keylen) {
    char *value = NULL;
    hash_entry_t entry;
    TAILQ_FOREACH(entry, head, entries) {
        if (keylen == entry->keylen && memcmp(entry->key, key, keylen) == 0) return entry;
    }
    return NULL;
}

EXPORT
bool stringhash_hasKey(stringhash_t hash, const char *key) {
    size_t keylen = strlen(key);
    size_t idx = indexOf(hash, key, keylen);
    return entryForKey(&hash->heads[idx], key, keylen)?true:false;
}

EXPORT
char *stringhash_copyValueForKey(stringhash_t hash, const char *key) {
    size_t keylen = strlen(key);
    size_t idx = indexOf(hash, key, keylen);
    hash_entry_t entry = entryForKey(&hash->heads[idx], key, keylen);
    if (entry) return strdup(entry->value);
    return NULL;
}

EXPORT
void stringhash_setKey(stringhash_t hash, const char *key, const char *value) {
    size_t keylen = strlen(key);
    size_t valuelen = strlen(value);
    size_t idx = indexOf(hash, key, keylen);
    hash_entry_t entry = entryForKey(&hash->heads[idx], key, keylen);
    if (entry) {
        TAILQ_REMOVE(&hash->heads[idx], entry, entries);
        if (entry->valuelen != valuelen) {
            hash_entry_t newEntry;
            int i=0;
            do {
                newEntry = realloc(entry, sizeof(struct hash_entry) + keylen + valuelen + 2);
            } while (newEntry == NULL && errno==ENOMEM && i++<100 && usleep(10)==0);
            if (newEntry == NULL) {
                fprintf(stderr, "FATAL ERROR: Unable to reallocate memory for entry.  Dropped.\n");
                free(entry);
                return;
            }
            entry = newEntry;
        }
    } else {
        int i=0;
        do {
            entry = malloc(sizeof(struct hash_entry) + keylen + valuelen + 2);
        } while (entry == NULL && errno==ENOMEM && i++<100 && usleep(10)==0);
        if (entry == NULL) {
            fprintf(stderr, "FATAL ERROR: Unable to allocate memory for entry.  Dropped.\n");
            return;
        }
        entry->keylen = keylen;
        memcpy(entry->key, key, keylen+1);
    }
    memcpy(entry->key+keylen+1, value, valuelen+1);
    entry->valuelen = valuelen;
    entry->value = entry->key+keylen+1;
    TAILQ_INSERT_HEAD(&hash->heads[idx], entry, entries);
}

EXPORT
void stringhash_removeKey(stringhash_t hash, const char *key) {
    size_t keylen = strlen(key);
    size_t idx = indexOf(hash, key, keylen);
    hash_entry_t entry = entryForKey(&hash->heads[idx], key, keylen);
    if (entry) {
        TAILQ_REMOVE(&hash->heads[idx], entry, entries);
        free(entry);
    }
}

EXPORT
size_t stringhash_count(stringhash_t hash) {
    size_t count = 0;

    for (size_t i=0; i<hash->size; i++) {
        hash_entry_t entry;
        TAILQ_FOREACH(entry, &hash->heads[i], entries) {
            count++;
        }
    }
    return count;
}

EXPORT
char **stringhash_copyAllKeys(stringhash_t hash) {
    size_t count = 0;
    size_t mem_size = sizeof(char*);

    for (size_t i=0; i<hash->size; i++) {
        hash_entry_t entry;
        TAILQ_FOREACH(entry, &hash->heads[i], entries) {
            size_t last_mem_size = mem_size;
            size_t last_count = count;
            count++;
            mem_size += sizeof(char *) + entry->keylen + 1;
            if (count < last_count || mem_size < last_mem_size) {
                fprintf(stderr, "ERROR: count or memory size integer overflow\n");
                return NULL;
            }
        }
    }
    char *buffer = malloc(mem_size);
    if (!buffer) return NULL;
    char **key=(char **)buffer, *value=(char *)(buffer + (count+1)*sizeof(char *));
    for (size_t i=0; i<hash->size; i++) {
        hash_entry_t entry;
        TAILQ_FOREACH(entry, &hash->heads[i], entries) {
            if ((size_t)value - (size_t)buffer + entry->keylen+1 > mem_size) {
                fprintf(stderr, "ERROR: buffer would overflow by %zu bytes at %zu/%zu\n", (((size_t)value - (size_t)buffer) + entry->keylen + 1 + sizeof(char*)) - mem_size, i+1, count);
                free(buffer);
                return NULL;
            }
            *key++ = value;
            memcpy(value, entry->key, entry->keylen+1);
            value += entry->keylen+1;
        }
    }
    *key = NULL;
    return (char **)buffer;
}

