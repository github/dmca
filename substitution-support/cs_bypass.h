#ifndef _CS_BYPASS_H
#define _CS_BYPASS_H
#include <stdio.h>
#include <mach/mach.h>
#include "libforgery.h"
#ifdef __cplusplus
extern "C" {
#endif
extern cs_forgery_ctx_t *cs_bypass_ctx;
typedef kern_return_t (*add_blob_t)(const char *obj, CS_SuperBlob *blob, size_t blobSize, uint64_t fileOffset, uint8_t realCDHash[20], uint8_t fakeCDHash[20], void *context);
typedef kern_return_t (*needs_blob_t)(const char *obj, bool *needsBlob, void *context);
void init_cs_bypass(add_blob_t add_blob, needs_blob_t needs_blob);
void load_code_signature(const char *obj, void *context);
void load_code_signatures(const char *obj, void *context);
#ifdef __cplusplus
}
#endif
#endif
