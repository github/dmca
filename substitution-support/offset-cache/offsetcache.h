//
//  offsetcache.h
//  An offset cache that can be stored in kernel memory
//
//  Created by Sam Bingner on 03/28/2019.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#ifndef _OFFSETCACHE_H
#define _OFFSETCACHE_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/queue.h>

typedef struct offset_entry {
    TAILQ_ENTRY(offset_entry) entries;
    uint64_t addr;
    char name[];
} offset_entry_t;

TAILQ_HEAD(offset_cache, offset_entry);

struct cache_blob {
    size_t size;
    struct offset_cache cache;
    offset_entry_t entries[];
};

// Removes and frees all entries in cache then initializes it
void destroy_cache(void);
// Destroys existing cache and imports blob
void import_cache_blob(struct cache_blob *blob);
// Merges blob without overwriting any existing entries
int merge_cache_blob(struct cache_blob *blob);
// Allocates a buffer and writes the cache to it
// Returns the size of the exported blob (this will need to be freed)
size_t export_cache_blob(struct cache_blob **newblob);
// Copies cache to provided blob - will fail if it is not large enough
//   to hold the cache.  Returns the size used.
size_t copy_cache_blob(struct cache_blob *blob);
// Returns true if the blobs are identical false otherwise
bool compare_cache_blob(struct cache_blob *blob);

// Removes an offset entry
// Returns: true if the entry was found
bool remove_offset(const char *name);
void set_offset(const char *name, uint64_t addr);
// Rerturns 0 if the offset was not found or is set to 0
uint64_t get_offset(const char *name);
// Returns true of the offset is in the cache
//  Might be used if you get 0 but want to know if it was set
bool has_offset(const char *name);


// Returns the buffer size that would be needed to export the cache
size_t get_cache_blob_size(void);
// Creates and initializes an empty cache blob of size
//   Use this to create a buffer to copy a saved blob into
//   *Remember to free it*
struct cache_blob *create_cache_blob(size_t size);
void blob_rebase(struct cache_blob *blob, uint64_t old_base, uint64_t new_base);

void print_cache(void);

#endif
