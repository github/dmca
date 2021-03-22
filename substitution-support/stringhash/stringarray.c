//
//  stringarray.c
//  A pure C array of strings
//  This is not particularly fast for direct access to middle elements in large arrays
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
#include "stringarray.h"

#define EXPORT __attribute__((visibility ("default")))
// array_entry is a single structure consisting of the structre defined below followed
// by the key and the value - value is a direct pointer to array->key + keylen + 1

typedef struct array_entry {
    TAILQ_ENTRY(array_entry) entries;
    size_t valuelen;
    char value[];
} *array_entry_t;

TAILQ_HEAD(array_head, array_entry);

struct stringarray {
    struct array_head head;
    size_t count;
};

EXPORT
stringarray_t stringarray_create(void) {
    stringarray_t array = malloc(sizeof(struct stringarray));
    array->count = 0;
    TAILQ_INIT(&array->head);
    return array;
}

EXPORT
void stringarray_destroy(stringarray_t array) {
    array->count = 0;
    array_entry_t entry, tent;
    TAILQ_FOREACH_SAFE(entry, &array->head, entries, tent) {
        free(entry);
    }
    free(array);
}

static array_entry_t entryForIdx(stringarray_t array, size_t idx) {
    if (idx >= array->count) return NULL;

    char *value = NULL;
    array_entry_t entry;
    if (idx > array->count/2) {
        size_t i=array->count;
        TAILQ_FOREACH_REVERSE(entry, &array->head, array_head, entries) {
            if (--i==idx) return entry;
        }
    } else {
        size_t i=0;
        TAILQ_FOREACH(entry, &array->head, entries) {
            if (i++==idx) return entry;
        }
    }
    return NULL;
}

EXPORT
char *stringarray_copyValue(stringarray_t array, size_t idx) {
    array_entry_t entry = entryForIdx(array, idx);
    if (entry) return strdup(entry->value);
    return NULL;
}

EXPORT
bool stringarray_setIdx(stringarray_t array, size_t idx, const char *value) {
    if (idx>array->count) return false;
    if (idx==array->count) return stringarray_push(array, value);

    size_t valuelen = strlen(value);
    array_entry_t entry = entryForIdx(array, idx);
    if (!entry) return false;

    array_entry_t prev = TAILQ_PREV(entry, array_head, entries);
    TAILQ_REMOVE(&array->head, entry, entries);
    if (entry->valuelen != valuelen) {
        array_entry_t newEntry;
        int i=0;
        do {
            newEntry = realloc(entry, sizeof(struct array_entry) + valuelen + 1);
        } while (newEntry == NULL && errno==ENOMEM && i++<100 && usleep(10)==0);
        if (newEntry == NULL) {
            fprintf(stderr, "FATAL ERROR: Unable to reallocate memory for entry.  Not changed.\n");
            TAILQ_INSERT_AFTER(&array->head, prev, entry, entries);
            return false;
        }
        entry = newEntry;
    }
    memcpy(entry->value, value, valuelen+1);
    TAILQ_INSERT_AFTER(&array->head, prev, entry, entries);
    return true;
}

EXPORT
bool stringarray_push(stringarray_t array, const char *value) {
    size_t valuelen = strlen(value);
    array_entry_t entry;
    int i=0;
    do {
        entry = malloc(sizeof(struct array_entry) + valuelen + 1);
    } while (entry == NULL && errno==ENOMEM && i++<100 && usleep(10)==0);
    if (entry == NULL) {
        fprintf(stderr, "FATAL ERROR: Unable to allocate memory for entry.  Dropped.\n");
        return false;
    }
    entry->valuelen = valuelen;
    memcpy(entry->value, value, valuelen+1);
    TAILQ_INSERT_TAIL(&array->head, entry, entries);
    array->count++;
    return true;
}

EXPORT
char *stringarray_pop(stringarray_t array) {
    array_entry_t entry = TAILQ_LAST(&array->head, array_head);
    if (!entry) return NULL;

    TAILQ_REMOVE(&array->head, entry, entries);
    array->count--;
    char *value = strdup(entry->value);
    free(entry);
    return value;
}

EXPORT
char *stringarray_shift(stringarray_t array) {
    array_entry_t entry = TAILQ_FIRST(&array->head);
    if (!entry) return NULL;

    TAILQ_REMOVE(&array->head, entry, entries);
    array->count--;
    char *value = strdup(entry->value);
    free(entry);
    return value;
}

EXPORT
size_t stringarray_count(stringarray_t array) {
    return array->count;
}

EXPORT
char **stringarray_copyAllValues(stringarray_t array) {
    size_t mem_size = sizeof(char*);
    size_t count = 0;

    array_entry_t entry;
    TAILQ_FOREACH(entry, &array->head, entries) {
        size_t last_mem_size = mem_size;
        size_t last_count = count;
        count++;
        mem_size += sizeof(char *) + entry->valuelen + 1;
        if (count < last_count || mem_size < last_mem_size) {
            fprintf(stderr, "ERROR: count or memory size integer overflow\n");
            return NULL;
        }
    }
    if (count != array->count) {
        fprintf(stderr, "ERROR: inconsistant array\n");
        return NULL;
    }
    char *buffer = malloc(mem_size);
    if (!buffer) return NULL;
    char **valueptr=(char **)buffer, *value=(char *)(buffer + (count+1)*sizeof(char *));
    TAILQ_FOREACH(entry, &array->head, entries) {
        if ((size_t)value - (size_t)buffer + entry->valuelen+1 > mem_size) {
            fprintf(stderr, "ERROR: buffer would overflow by %zu bytes\n", (((size_t)value - (size_t)buffer) + entry->valuelen + 1 + sizeof(char*)) - mem_size);
            free(buffer);
            return NULL;
        }
        *valueptr++ = value;
        memcpy(value, entry->value, entry->valuelen+1);
        value += entry->valuelen+1;
    }
    *valueptr = NULL;
    return (char **)buffer;
}

