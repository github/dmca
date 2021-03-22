#include "cs_bypass.h"
#include "Scope.hpp"
#include <common.h>

static add_blob_t add_blob = NULL;
static needs_blob_t needs_blob = NULL;
static cs_forgery_ctx_t _s_cs_bypass_ctx = {0};
cs_forgery_ctx_t *cs_bypass_ctx = &_s_cs_bypass_ctx;

extern "C" void init_cs_bypass(add_blob_t add_blob_ptr, needs_blob_t needs_blob_ptr) {
	add_blob = add_blob_ptr;
	needs_blob = needs_blob_ptr;
}

extern "C" void load_code_signature(const char *obj, void *context) {
	if (add_blob == NULL || needs_blob == NULL) {
		return;
	}
	kern_return_t kr;
	bool needsBlob = true;
	kr = needs_blob(obj, &needsBlob, context);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get blob status: %s", mach_error_string(kr));
		return;
	}
	if (!needsBlob) {
		debug_log("Already has blob: %s", obj);
		return;
	}
	static bool shouldInitialize = true;
	if (shouldInitialize) {
		if (!cs_forgery_init(cs_bypass_ctx, "/usr/share/substitute/signcert.p12")) {
			error_log("Unable to initialize codesign forgery");
			return;
		}
		shouldInitialize = false;
	}
	CS_SuperBlob *blob = NULL;
	size_t blobSize = 0;
	uint64_t fileOffset = 0;
	uint8_t realCDHash[CS_CDHASH_LEN];
	uint8_t fakeCDHash[CS_CDHASH_LEN];
	if (!cs_forgery_forge(cs_bypass_ctx, obj, &blob, &blobSize, &fileOffset, realCDHash, fakeCDHash)) {
		error_log("Unable to forge code signature");
		return;
	}
	_scope({ free(blob); });
	info_log("Forged code signature: %p", blob);
	kr = add_blob(obj, blob, blobSize, fileOffset, realCDHash, fakeCDHash, context);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to add blob: %s", mach_error_string(kr));
		return;
	}
	info_log("Added blob for: %s", obj);
}

extern "C" void load_code_signatures(const char *path, void *context) {
	char *interpreterPath = cs_forgery_get_interpreter(path);
	if (interpreterPath != NULL) {
		_scope({ free(interpreterPath); });
		cs_forgery_iterate(interpreterPath, load_code_signature, context);
	} else {
		cs_forgery_iterate(path, load_code_signature, context);
	}
}
