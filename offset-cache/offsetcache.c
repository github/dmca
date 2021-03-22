//
//  offsetcache.c
//  An offset cache that can be stored in kernel memory
//
//  Created by Sam Bingner on 03/28/2019.
//  Copyright © 2019 Sam Bingner. All rights reserved.
//

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/queue.h>
#include "offsetcache.h"

static struct offset_cache cache = TAILQ_HEAD_INITIALIZER(cache);

bool remove_offset(const char *name)
{
    offset_entry_t *np, *np_t;
    TAILQ_FOREACH_SAFE(np, &cache, entries, np_t) {
        if (strcmp(np->name, name) == 0) {
            TAILQ_REMOVE(&cache, np, entries);
            free(np);
            return true;
        }
    }
    return false;
}

void set_offset(const char *name, uint64_t addr)
{
    offset_entry_t *entry = malloc(sizeof(offset_entry_t) + strlen(name) + 1);
    entry->addr = addr;
    strcpy(entry->name, name);
    // Remove any existing reference to this offset to avoid dupes
    remove_offset(name);
    TAILQ_INSERT_TAIL(&cache, entry, entries);
}

uint64_t get_offset(const char *name)
{
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        if (strcmp(name, np->name) == 0) {
            return np->addr;
        }
    }
    return 0;
}

bool has_offset(const char *name)
{
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        if (strcmp(name, np->name) == 0) {
            return true;
        }
    }
    return false;
}

#define ENTRY_SIZE(x) (sizeof(offset_entry_t) + strlen((x)->name) + 1)
size_t get_cache_blob_size()
{
    size_t cache_size = sizeof(struct offset_cache);
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        cache_size += ENTRY_SIZE(np);
    }
    return cache_size;
}

size_t copy_cache_blob(struct cache_blob *blob)
{
    offset_entry_t *np;
    struct offset_cache *local_cache = &blob->cache;
    local_cache->tqh_first = NULL;
    local_cache->tqh_last = &(local_cache->tqh_first);

    offset_entry_t *newentry = blob->entries;
    TAILQ_FOREACH(np, &cache, entries) {
        size_t entry_size = sizeof(offset_entry_t);
        if (((char *)newentry + entry_size) > (char*)blob + blob->size) return 0;
        entry_size += strlen(np->name) + 1;
        memcpy(newentry, np, entry_size);
        TAILQ_INSERT_TAIL(local_cache, newentry, entries);
        newentry = (offset_entry_t*)((char *)newentry + entry_size);
    }
    return (uint64_t)newentry - (uint64_t)blob;
}

struct cache_blob *create_cache_blob(size_t size)
{
    struct cache_blob *blob = calloc(size, 1);
    blob->size = size;
    TAILQ_INIT(&blob->cache);
    return blob;
}

size_t export_cache_blob(struct cache_blob **newblob)
{
    size_t blob_size = get_cache_blob_size();
    blob_size += sizeof(struct cache_blob) - sizeof(struct offset_cache);

    *newblob = create_cache_blob(blob_size);
    if (copy_cache_blob(*newblob) != blob_size) {
        fprintf(stderr, "Error: unable to copy blob - dest buffer too small?\n");
        free(*newblob);
        *newblob = NULL;
        return 0;
    }
    return blob_size;
}

#define REBASE(x, old_base, new_base) (x = (typeof(x))((char *)(x) - (char *)old_base + (char *)new_base))
void blob_rebase(struct cache_blob *blob, uint64_t old_base, uint64_t new_base)
{   
#ifdef DEBUG
    printf("Rebasing blob from %llx to %llx\n", old_base, new_base);
#endif
    struct offset_cache *local_cache = &(blob->cache);
    REBASE(local_cache->tqh_last, old_base, new_base);
    if (local_cache->tqh_first == NULL) return;

    REBASE(local_cache->tqh_first, old_base, new_base);
    for (offset_entry_t *entry = (offset_entry_t *)(local_cache + 1);;
            entry = (offset_entry_t *)((char *)entry + ENTRY_SIZE(entry))) {
        REBASE(entry->entries.tqe_prev, old_base, new_base);
        if (entry->entries.tqe_next == NULL) return;
        REBASE(entry->entries.tqe_next, old_base, new_base);
    }
}

static void rebase_if_needed(struct cache_blob *blob)
{
    struct offset_cache *local_cache = &blob->cache;

    if ((local_cache->tqh_first && (uint64_t)local_cache->tqh_first != (uint64_t)blob + sizeof(struct cache_blob)) ||
        (!local_cache->tqh_first && local_cache->tqh_last != &local_cache->tqh_first)) {
        uint64_t old_base = (uint64_t)local_cache->tqh_first - sizeof(struct cache_blob);
        blob_rebase(blob, old_base, (uint64_t)blob);
    }
}

int merge_cache_blob(struct cache_blob *blob)
{
    struct cache_blob *local_blob = malloc(blob->size);
    memcpy(local_blob, blob, blob->size);
    rebase_if_needed(local_blob);

    struct offset_cache *local_cache = &local_blob->cache;
    
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        remove_offset(np->name);
    }

    int merged_count = 0;
    TAILQ_FOREACH(np, local_cache, entries) {
        set_offset(np->name, np->addr);
        merged_count++;
    }
    free(local_blob);
    return merged_count;
}

void import_cache_blob(struct cache_blob *blob)
{
    offset_entry_t *np;

    rebase_if_needed(blob);
    struct offset_cache *local_cache = &blob->cache;

    destroy_cache();
    TAILQ_FOREACH(np, local_cache, entries) {
        set_offset(np->name, np->addr);
    }
}

void init_cache()
{
    TAILQ_INIT(&cache);
}

void destroy_cache()
{
    offset_entry_t *np, *np_t;
    TAILQ_FOREACH_SAFE(np, &cache, entries, np_t) {
        free(np);
    }
    init_cache();
}

bool compare_cache_blob(struct cache_blob *blob)
{
    offset_entry_t *np;
    offset_entry_t *entry = TAILQ_FIRST(&blob->cache);
    TAILQ_FOREACH(np, &cache, entries) {
        // We ran out of blob entries before cache entries
        if (!entry) return false;
        // It isn't equal
        if (strcmp(entry->name, np->name)) return false;
        if (entry->addr != np->addr) return false;
        entry = TAILQ_NEXT(entry, entries);
    }
    // everything matched but there are more entries
    if (entry) return false;
    return true;
}

void print_cache()
{
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        printf("Entry %p\n"
               "\taddr: 0x%llx\n"
               "\tname: %s\n",
               np, np->addr, np->name);
    }
}

#ifdef MAIN
// Example / test code
int main()
{
    set_offset("kernel_base", 0xdeadbeefdead0000);
    set_offset("kernel_slide", 0x50);
    set_offset("kernel_slide", 0x69);
    offset_entry_t *np;
    TAILQ_FOREACH(np, &cache, entries) {
        printf("Entry %p\n\taddr: 0x%llx\n\tname: %s\n", np, np->addr, np->name);
    }
    printf("\n");
    struct cache_blob *blob;
    printf("Generating a contiguous blob\n");
    size_t blob_size = export_cache_blob(&blob);
    if (blob_size < sizeof(struct cache_blob)) {
        fprintf(stderr, "Unable to export blob\n");
        return -1;
    }
    printf("Blob\tsize: %lx\n"
               "\tbegin: %p\n"
               "\tend: %p\n",
               blob_size, blob, (char*)blob + blob_size);
    printf("\tFirst entry: %p\n", blob->cache.tqh_first);
    if (blob_size>0) TAILQ_FOREACH(np, &blob->cache, entries) {
        printf("Entry %p\n\taddr: 0x%llx\n\tname: %s\n", np, np->addr, np->name);
    }
    printf("\n");
    printf("Rebasing blob to \"kernel\" address %p\n", (char*)0xdeadbeefdead0000);
    blob_rebase(blob, (uint64_t)blob, 0xdeadbeefdead0000);
    printf("Blob\tsize: %lx\n"
               "\tbegin: %p\n"
               "\tend: %p\n",
               blob_size, blob, (char*)blob + blob_size);
    offset_entry_t *testentry = blob->entries;
    testentry = (offset_entry_t*)((char *)testentry + sizeof(offset_entry_t) + strlen(testentry->name) + 1);
    printf("Importing \"kernel\" blob\n");
    import_cache_blob(blob);

    printf("get_offset(kernel_base): %llx\n", get_offset("kernel_base"));
    TAILQ_FOREACH(np, &cache, entries) {
        printf("Entry %p\n"
               "\taddr: 0x%llx\n"
               "\tname: %s\n",
               np, np->addr, np->name);
    }
    free(blob);
    destroy_cache();
}
#endif
