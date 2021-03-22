//
//  libforgery.h
//  Copyright © 2020 Pwn20wnd. All rights reserved.
//

#ifndef libforgery_h
#define libforgery_h

#include <stdio.h>
#include <fcntl.h>
#include <security_private.h>
#include <cs_blobs.h>
#include <ubc_headers.h>

typedef struct {
	CFMutableArrayRef chain;
	SecIdentityRef signer;
} cs_forgery_ctx_t;

#ifdef __cplusplus
extern "C" {
#endif
char *cs_forgery_get_interpreter(const char *path);
bool cs_forgery_iterate(const char *path, void (*handler)(const char *obj, void *context), void *context);
bool cs_forgery_deinit(cs_forgery_ctx_t *ctx);
bool cs_forgery_init(cs_forgery_ctx_t *ctx, const char *signcert);
bool cs_forgery_forge(cs_forgery_ctx_t *ctx, const char *path, CS_SuperBlob **sig_p, size_t *size_p, uint64_t *file_offset_p, uint8_t real_cdhash[CS_CDHASH_LEN], uint8_t fake_cdhash[CS_CDHASH_LEN]);
bool cs_forgery_get_cdhash(const char *path, uint8_t cdhash[CS_CDHASH_LEN]);
#ifdef __cplusplus
}
#endif

#endif /* libforgery_h */
