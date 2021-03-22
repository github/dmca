//
//  libforgery.c
//  Copyright © 2020 Pwn20wnd. All rights reserved.
//

#include "libforgery.h"
#include "Scope.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <CommonCrypto/CommonCrypto.h>
#include <common.h>
#include <mach-o/arch.h>
#include <mach-o/dyld_images.h>
#include <mach-o/fat.h>
#include <mach-o/swap.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <syslog.h>
#include <dlfcn.h>
extern "C" {
#include "stringarray.h"
#include "stringhash.h"
}

/*
 * For #! interpreter parsing
 */
#define IS_WHITESPACE(ch) ((ch == ' ') || (ch == '\t'))
#define IS_EOL(ch) ((ch == '#') || (ch == '\n'))
#define IMG_SHSIZE 512 /* largest shell interpreter, in bytes */

#ifdef __LP64__
#define arch_abi CPU_ARCH_ABI64
#else
#define arch_abi 0
#endif

#ifdef DEBUG
#define SecCall(name, ...) do { \
	OSStatus status = name(__VA_ARGS__); \
	if (status != errSecSuccess) { \
		CFStringRef error = SecCopyErrorMessageString(status, NULL); \
		if (error != NULL) { \
			_scope({ CFRelease(error); }); \
			error_log("%s: %s", #name, CFStringGetCStringPtr(error, kCFStringEncodingUTF8)); \
		} \
		return false; \
	} \
} while (false)
#define SecSym(name) name
#else
#define SecCall(name, ...) do { \
	static __typeof__(name) *sym = NULL; \
	if (sym == NULL) { \
		sym = (__typeof__(sym))dlsym(RTLD_DEFAULT, #name); \
	} \
	OSStatus status = sym(__VA_ARGS__); \
	if (status != errSecSuccess) { \
		return false; \
	} \
} while (false)
#define SecSym(name) ({ \
	static __typeof__(name) *sym = NULL; \
	if (sym == NULL) { \
		sym = (__typeof__(sym))dlsym(RTLD_DEFAULT, #name); \
	} \
	*sym; \
})
#endif

static const NXArchInfo *arch = NULL;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static void init(void) {
	arch = NXGetLocalArchInfo();
	debug_log("arch: %p", arch);
}

static CS_GenericBlob *csblob_find_blob_bytes(const uint8_t *addr, size_t length, uint32_t type, uint32_t magic) {
	CS_GenericBlob *blob = (CS_GenericBlob *)(const void *)addr;

	if (ntohl(blob->magic) == CSMAGIC_EMBEDDED_SIGNATURE) {
		const CS_SuperBlob *sb = (const CS_SuperBlob *)blob;
		size_t n, count = ntohl(sb->count);

		for (n = 0; n < count; n++) {
			if (ntohl(sb->index[n].type) != type)
				continue;
			uint32_t offset = ntohl(sb->index[n].offset);
			if (length - sizeof(const CS_GenericBlob) < offset)
				return NULL;
			blob = (CS_GenericBlob *)(const void *)(addr + offset);
			if (ntohl(blob->magic) != magic)
				continue;
			return blob;
		}
	} else if (type == CSSLOT_CODEDIRECTORY && ntohl(blob->magic) == CSMAGIC_CODEDIRECTORY && magic == CSMAGIC_CODEDIRECTORY)
		return blob;
	return NULL;
}

void *csblob_insert_blob(CS_SuperBlob *old, void *blob, uint32_t blobSize, uint32_t oldSize, uint32_t *newSize_p, uint32_t magic, uint32_t type, CS_GenericBlob **addedBlob) {
	uint32_t newSize = oldSize + sizeof(CS_BlobIndex) + sizeof(CS_GenericBlob) + blobSize;
	*newSize_p = newSize;

	CS_SuperBlob *new_blob = (CS_SuperBlob *)malloc(newSize);
	if (new_blob == NULL) {
		error_log("Unable to allocate memory for new blob: %s", strerror(errno));
		return NULL;
	}

	new_blob->magic = old->magic;
	new_blob->length = ntohl(ntohl(old->length) + sizeof(CS_BlobIndex) + sizeof(CS_GenericBlob) + blobSize);
	new_blob->count = ntohl(ntohl(old->count) + 1);

	int i;
	CS_GenericBlob *oldb = NULL;
	CS_GenericBlob *newb = NULL;

	for (i = 0; i < ntohl(old->count); i++) {
		new_blob->index[i].type = old->index[i].type;
		new_blob->index[i].offset = ntohl(ntohl(old->index[i].offset) + sizeof(CS_BlobIndex));

		oldb = (CS_GenericBlob *)(((uint64_t)old) + ntohl(old->index[i].offset));
		newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
		memcpy(newb, oldb, ntohl(oldb->length));
	}

	new_blob->index[i].type = ntohl(type);

	if (newb) {
		new_blob->index[i].offset = ntohl(ntohl(new_blob->index[i - 1].offset) + ntohl(newb->length));
	} else {
		new_blob->index[i].offset = ntohl(sizeof(CS_SuperBlob) + sizeof(CS_BlobIndex));
	}

	CS_GenericBlob *newgb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
	newgb->magic = ntohl(magic);
	newgb->length = ntohl(sizeof(CS_GenericBlob) + blobSize);
	memcpy(newgb->data, blob, blobSize);

	if (addedBlob) {
		*addedBlob = newgb;
	}

	return new_blob;
}

static void *csblob_patch(CS_SuperBlob *old, void *blob, uint32_t blobSize, uint32_t oldBlobSize, uint32_t oldSize, uint32_t *newSize_p, uint32_t magic, uint32_t type, CS_GenericBlob **addedBlob) {

	uint32_t added = blobSize - oldBlobSize;

	uint32_t newSize = oldSize + added;
	*newSize_p = newSize;

	CS_SuperBlob *new_blob = (CS_SuperBlob *)malloc(newSize);
	if (new_blob == NULL) {
		error_log("Unable to allocate memory for new blob: %s", strerror(errno));
		return NULL;
	}

	new_blob->magic = old->magic;
	new_blob->length = ntohl(ntohl(old->length) + added);
	new_blob->count = old->count;

	int i;
	CS_GenericBlob *oldb = NULL;
	CS_GenericBlob *newb = NULL;

	uint32_t blobOffset = 0;
	for (i = 0; i < ntohl(old->count); i++) {
		if (old->index[i].type == ntohl(type)) {
			new_blob->index[i].type = ntohl(type);
			new_blob->index[i].offset = old->index[i].offset;
			blobOffset = ntohl(old->index[i].offset);

			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			newb->magic = ntohl(magic);
			newb->length = ntohl(sizeof(CS_GenericBlob) + blobSize);
			memcpy(newb->data, blob, blobSize);

			if (addedBlob) {
				*addedBlob = newb;
			}
			break;
		}
	}
	for (i = 0; i < ntohl(old->count); i++) {
		if (ntohl(old->index[i].offset) > blobOffset) {
			new_blob->index[i].type = old->index[i].type;
			new_blob->index[i].offset = ntohl(ntohl(old->index[i].offset) + added);

			oldb = (CS_GenericBlob *)(((uint64_t)old) + ntohl(old->index[i].offset));
			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			memcpy(newb, oldb, ntohl(oldb->length));
		} else if (ntohl(old->index[i].offset) < blobOffset) {
			new_blob->index[i].type = old->index[i].type;
			new_blob->index[i].offset = ntohl(ntohl(old->index[i].offset));

			oldb = (CS_GenericBlob *)(((uint64_t)old) + ntohl(old->index[i].offset));
			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			memcpy(newb, oldb, ntohl(oldb->length));
		}
	}
	return new_blob;
}

static void *csblob_patch_at_offset(CS_SuperBlob *old, void *blob, uint32_t blobSize, uint32_t oldBlobSize, uint32_t oldSize, uint32_t *newSize_p, uint32_t magic, uint32_t type, uint32_t offset, CS_GenericBlob **addedBlob) {

	uint32_t added = blobSize - oldBlobSize;

	uint32_t newSize = oldSize + added;
	*newSize_p = newSize;

	CS_SuperBlob *new_blob = (CS_SuperBlob *)malloc(newSize);
	if (new_blob == NULL) {
		error_log("Unable to allocate memory for new blob: %s", strerror(errno));
		return NULL;
	}

	new_blob->magic = old->magic;
	new_blob->length = ntohl(ntohl(old->length) + added);
	new_blob->count = old->count;

	int i;
	CS_GenericBlob *oldb = NULL;
	CS_GenericBlob *newb = NULL;

	uint32_t blobOffset = 0;
	for (i = 0; i < ntohl(old->count); i++) {
		if (old->index[i].offset == ntohl(offset)) {
			new_blob->index[i].type = ntohl(type);
			new_blob->index[i].offset = old->index[i].offset;
			blobOffset = ntohl(old->index[i].offset);

			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			newb->magic = ntohl(magic);
			newb->length = ntohl(sizeof(CS_GenericBlob) + blobSize);
			memcpy(newb->data, blob, blobSize);

			if (addedBlob) {
				*addedBlob = newb;
			}
			break;
		}
	}
	for (i = 0; i < ntohl(old->count); i++) {
		if (ntohl(old->index[i].offset) > blobOffset) {
			new_blob->index[i].type = old->index[i].type;
			new_blob->index[i].offset = ntohl(ntohl(old->index[i].offset) + added);

			oldb = (CS_GenericBlob *)(((uint64_t)old) + ntohl(old->index[i].offset));
			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			memcpy(newb, oldb, ntohl(oldb->length));
		} else if (ntohl(old->index[i].offset) < blobOffset) {
			new_blob->index[i].type = old->index[i].type;
			new_blob->index[i].offset = ntohl(ntohl(old->index[i].offset));

			oldb = (CS_GenericBlob *)(((uint64_t)old) + ntohl(old->index[i].offset));
			newb = (CS_GenericBlob *)(((uint64_t)new_blob) + ntohl(new_blob->index[i].offset));
			memcpy(newb, oldb, ntohl(oldb->length));
		}
	}
	return new_blob;
}

// xnu-3789.70.16/bsd/kern/ubc_subr.c#231
static unsigned int hash_rank(CS_CodeDirectory *cd) {
	uint32_t type = cd->hashType;

	for (unsigned int n = 0; n < sizeof(hashPriorities) / sizeof(hashPriorities[0]); ++n) {
		if (hashPriorities[n] == type) {
			return n + 1;
		}
	}

	return 0;
}

#define BLOB_FITS(blob, size) ((size >= sizeof(*blob)) && (size >= ntohl(blob->length)))
// xnu-3789.70.16/bsd/kern/ubc_subr.c#470
static bool find_best_codedir(const void *csblob, uint32_t blob_size, CS_CodeDirectory **chosen_cd, uint32_t *csb_offset, CS_GenericBlob **entitlements, uint32_t *entitlements_offset) {
	*chosen_cd = NULL;
	*entitlements = NULL;

	CS_GenericBlob *blob = (CS_GenericBlob *)csblob;

	if (!BLOB_FITS(blob, blob_size)) {
		error_log("csblob too small even for generic blob");
		return false;
	}

	uint32_t length = ntohl(blob->length);

	if (ntohl(blob->magic) == CSMAGIC_EMBEDDED_SIGNATURE) {
		CS_CodeDirectory *best_cd = NULL;
		int best_rank = 0;

		CS_SuperBlob *sb = (CS_SuperBlob *)csblob;
		uint32_t count = ntohl(sb->count);

		if (!BLOB_FITS(sb, blob_size)) {
			error_log("csblob too small for superblob");
			return false;
		}

		for (int n = 0; n < count; n++) {
			CS_BlobIndex *blobIndex = &sb->index[n];

			uint32_t type = ntohl(blobIndex->type);
			uint32_t offset = ntohl(blobIndex->offset);

			if (length < offset) {
				error_log("offset of blob #%d overflows superblob length", n);
				return false;
			}

			CS_GenericBlob *subBlob = (CS_GenericBlob *)((uintptr_t)csblob + offset);

			if (type == CSSLOT_CODEDIRECTORY || (type >= CSSLOT_ALTERNATE_CODEDIRECTORIES && type < CSSLOT_ALTERNATE_CODEDIRECTORY_LIMIT)) {
				CS_CodeDirectory *candidate = (CS_CodeDirectory *)subBlob;

				unsigned int rank = hash_rank(candidate);

				// Apple's code: `rank > best_rank` (kind of obvious, right?)
				// So why is it I have to switch it to get it to work?
				// macos-10.12.6-sierra/xnu-3789.70.16/bsd/kern/ubc_subr.c#534
				if (best_cd == NULL || ((kCFCoreFoundationVersionNumber >= 1443.00) ? rank > best_rank : rank < best_rank)) {
					best_cd = candidate;
					best_rank = rank;

					*chosen_cd = best_cd;
					*csb_offset = offset;
				}
			} else if (type == CSSLOT_ENTITLEMENTS) {
				*entitlements = subBlob;
				*entitlements_offset = offset;
			}
		}
	} else if (ntohl(blob->magic) == CSMAGIC_CODEDIRECTORY) {
		*chosen_cd = (CS_CodeDirectory *)blob;
		*csb_offset = 0;
	} else {
		error_log("Unknown magic at csblob start: %08x", ntohl(blob->magic));
		return false;
	}

	if (chosen_cd == NULL) {
		error_log("didn't find codedirectory to hash");
		return false;
	}

	return true;
}

static bool hash_code_directory(const CS_CodeDirectory *directory, uint8_t hash[CS_CDHASH_LEN]) {
	uint32_t realsize = ntohl(directory->length);

	if (ntohl(directory->magic) != CSMAGIC_CODEDIRECTORY) {
		error_log("expected CSMAGIC_CODEDIRECTORY");
		return false;
	}

	uint8_t out[CS_HASH_MAX_SIZE];
	uint8_t hash_type = directory->hashType;

	const unsigned char *ptr = (__typeof__(ptr))directory;
	switch (hash_type) {
	case CS_HASHTYPE_SHA1:
		CC_SHA1(ptr, realsize, out);
		break;

	case CS_HASHTYPE_SHA256:
	case CS_HASHTYPE_SHA256_TRUNCATED:
		CC_SHA256(ptr, realsize, out);
		break;

	case CS_HASHTYPE_SHA384:
		CC_SHA384(ptr, realsize, out);
		break;

	default:
		error_log("Unknown hash type: 0x%x", hash_type);
		return false;
	}

	memcpy(hash, out, CS_CDHASH_LEN);
	return true;
}

static bool build_cms_blob(cs_forgery_ctx_t *ctx, const void *cdir, size_t cdir_len, const void *alt_cdir, size_t alt_cdir_len, char **blob_data_p, size_t *blob_size_p) {
	uint8_t cdhash[CS_CDHASH_LEN];
	if (!hash_code_directory((CS_CodeDirectory *)cdir, cdhash)) {
		error_log("Unable to hash_code_directory");
		return false;
	}
	const void *bytesPtr;
	size_t bytesLen;
	if (alt_cdir_len != 0) {
		bytesPtr = alt_cdir;
		bytesLen = alt_cdir_len;
	} else {
		bytesPtr = cdir;
		bytesLen = cdir_len;
	}
	CFDataRef hash = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (UInt8 *)cdhash, sizeof(cdhash), kCFAllocatorNull);
	if (hash == NULL) {
		error_log("Unable to CFDataCreateWithBytesNoCopy");
		return false;
	}
	_scope({ CFRelease(hash); });
	CFArrayRef hashes = CFArrayCreate(kCFAllocatorDefault, (CFTypeRef *)&hash, 1, &kCFTypeArrayCallBacks);
	if (hashes == NULL) {
		error_log("Unable to CFArrayCreate");
		return false;
	}
	_scope({ CFRelease(hashes); });
	CFStringRef hashesKey = CFSTR("cdhashes");
	CFDictionaryRef hashDict = CFDictionaryCreate(kCFAllocatorDefault, (CFTypeRef *)&hashesKey, (CFTypeRef *)&hashes, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (hashDict == NULL) {
		error_log("Unable to CFDictionaryCreate");
		return false;
	}
	_scope({ CFRelease(hashDict); });
	CFPropertyListRef hashPlist = (CFPropertyListRef)hashDict;
	CFDataRef hashBag = CFPropertyListCreateData(kCFAllocatorDefault, hashPlist, kCFPropertyListXMLFormat_v1_0, 0, NULL);
	if (hashBag == NULL) {
		error_log("Unable to CFPropertyListCreateData");
		return false;
	}
	_scope({ CFRelease(hashBag); });
	CMSEncoderRef encoder = NULL;
	SecCall(CMSEncoderCreate, &encoder);
	_scope({ CFRelease(encoder); });
	SecCall(CMSEncoderAddSupportingCerts, encoder, ctx->chain);
	SecCall(CMSEncoderSetCertificateChainMode, encoder, kCMSCertificateSignerOnly);
	SecCall(CMSEncoderAddSigners, encoder, ctx->signer);
	SecCall(CMSEncoderSetSignerAlgorithm, encoder, SecSym(kCMSEncoderDigestAlgorithmSHA1));
	SecCall(CMSEncoderSetHasDetachedContent, encoder, true);
	SecCall(CMSEncoderAddSignedAttributes, encoder, kCMSAttrAppleCodesigningHashAgility);
	SecCall(CMSEncoderSetAppleCodesigningHashAgility, encoder, hashBag);
	SecCall(CMSEncoderUpdateContent, encoder, bytesPtr, bytesLen);
	// HACK: Fix for memory leak in CMSEncoderUpdateContent
	_scope({ CFRelease(*(CFTypeRef **)((uintptr_t)encoder + 0x68)); });
	CFDataRef signatureData = NULL;
	SecCall(CMSEncoderCopyEncodedContent, encoder, &signatureData);
	_scope({ CFRelease(signatureData); });
	CFIndex signatureLen = CFDataGetLength(signatureData);
	char *signatureBuffer = (char *)malloc((size_t)signatureLen);
	if (signatureBuffer == NULL) {
		error_log("Unable to allocate memory for signature buffer: %s", strerror(errno));
		return false;
	}
	CFDataGetBytes(signatureData, CFRangeMake(0, signatureLen), (UInt8 *)signatureBuffer);
	*blob_data_p = signatureBuffer;
	*blob_size_p = (size_t)signatureLen;
	return true;
}

char *build_path(char *dirPath, char *name) {
	char *path = (char *)malloc(strlen(dirPath) + strlen("/") + strlen(name) + sizeof(char));
	if (path == NULL) {
		error_log("Unable to allocate memory for path: %s", strerror(errno));
		return NULL;
	}
	strcpy(path, dirPath);
	strcat(path, "/");
	strcat(path, name);
	char *newPath = realpath(path, NULL);
	free(path);
	path = newPath;
	return path;
}

char *find_dir_path(const char *path) {
	char *dirPath;
	const char *lastSlash = strrchr(path, '/');
	if (lastSlash == NULL) {
		dirPath = strdup(path);
	} else {
		dirPath = strndup(path, strlen(path) - strlen(lastSlash));
	}
	if (dirPath == NULL) {
		error_log("Unable to allocate directory path: %s", strerror(errno));
		return NULL;
	}
	char *newDirPath = realpath(dirPath, NULL);
	free(dirPath);
	dirPath = newDirPath;
	return dirPath;
}

char *resolve_path(char *path, stringarray_t rpath, char *executablePath, char *loaderPath, bool findDir) {
	struct stat st;
	char *tofree, *string;
	tofree = string = strdup(path);
	if (string == NULL) {
		return NULL;
	}
	_scope({ free(tofree); });
	char *dynamicType = strsep(&string, "/");
	char *staticPath = &path[strlen(dynamicType) + 1];
	if (strcmp(dynamicType, "@rpath") == 0) {
		size_t rpathCnt = stringarray_count(rpath);
		for (size_t cnt = 0; cnt < rpathCnt; cnt++) {
			char *search = stringarray_copyValue(rpath, cnt);
			if (search == NULL) {
				continue;
			}
			char *tryPath = build_path(search, staticPath);
			free(search);
			if (tryPath == NULL) {
				continue;
			}
			st.st_mode = 0;
			if (stat(tryPath, &st) == 0 && S_ISDIR(st.st_mode) == findDir) {
				return tryPath;
			}
			free(tryPath);
		}
	} else if (strcmp(dynamicType, "@executable_path") == 0) {
		char *tryPath = build_path(executablePath, staticPath);
		if (tryPath == NULL) {
			return NULL;
		}
		st.st_mode = 0;
		if (stat(tryPath, &st) == 0 && S_ISDIR(st.st_mode) == findDir) {
			return tryPath;
		}
		free(tryPath);
	} else if (strcmp(dynamicType, "@loader_path") == 0) {
		char *tryPath = build_path(loaderPath, staticPath);
		if (tryPath == NULL) {
			return NULL;
		}
		st.st_mode = 0;
		if (stat(tryPath, &st) == 0 && S_ISDIR(st.st_mode) == findDir) {
			return tryPath;
		}
		free(tryPath);
	} else {
		error_log("Unknown dynamic type: \"%s\"", dynamicType);
	}
	return NULL;
}

bool get_object_header(uint8_t *map, size_t size, uint8_t **unusedMap, size_t *unusedSize, struct mach_header_64 **mhOut, bool *bitswap, uint64_t *file_offset) {

	struct mach_header_64 *mh = (struct mach_header_64 *)map;
	uint8_t *unused_map = map;
	size_t unused_size = size;

	bool isFat = false;
	*bitswap = false;

	switch (mh->magic) {
	case FAT_CIGAM:
		*bitswap = true;
	case FAT_MAGIC:
		isFat = true;
		break;
	case FAT_CIGAM_64:
		*bitswap = true;
	case FAT_MAGIC_64:
		isFat = true;
		break;
	case MH_CIGAM_64:
		*bitswap = true;
		break;
	case MH_MAGIC_64:
		break;
	default:
		error_log("Can't handle this magic");
		return false;
	}
	if (!isFat) {
		unused_map += sizeof(struct mach_header_64);
		unused_size -= sizeof(struct mach_header_64);
		if (file_offset != NULL) {
			*file_offset = 0;
		}
	} else {
		struct fat_header *fatHdr = (struct fat_header *)unused_map;
		unused_map += sizeof(struct fat_header);
		unused_size -= sizeof(struct fat_header);
		if (bitswap)
			swap_fat_header(fatHdr, arch->byteorder);

		if (fatHdr->magic == FAT_MAGIC_64) {
			size_t fat_arch_size = fatHdr->nfat_arch * sizeof(struct fat_arch_64);
			if (unused_size < fat_arch_size)
				return false;

			struct fat_arch_64 *fat_archs = (struct fat_arch_64 *)unused_map;
			unused_map += fat_arch_size;
			unused_size -= fat_arch_size;
			if (bitswap)
				swap_fat_arch_64(fat_archs, fatHdr->nfat_arch, arch->byteorder);

			struct fat_arch_64 *best_fat = NXFindBestFatArch_64(arch->cputype | arch_abi, arch->cpusubtype, fat_archs, fatHdr->nfat_arch);
			if (!best_fat)
				return false;

			if (size < best_fat->offset + best_fat->size)
				return false;

			if (file_offset != NULL) {
				*file_offset = best_fat->offset;
			}

			unused_map = map + best_fat->offset;
			unused_size = best_fat->size;
		} else {
			size_t fat_arch_size = fatHdr->nfat_arch * sizeof(struct fat_arch);
			if (unused_size < fat_arch_size)
				return false;
			struct fat_arch *fat_archs = (struct fat_arch *)unused_map;
			unused_map += fat_arch_size;
			unused_size -= fat_arch_size;
			if (*bitswap)
				swap_fat_arch(fat_archs, fatHdr->nfat_arch, arch->byteorder);

			struct fat_arch *best_fat = NXFindBestFatArch(arch->cputype | arch_abi, arch->cpusubtype, fat_archs, fatHdr->nfat_arch);
			if (!best_fat)
				return false;

			if (size < best_fat->offset + best_fat->size)
				return false;

			if (file_offset != NULL) {
				*file_offset = best_fat->offset;
			}

			unused_map = map + best_fat->offset;
			unused_size = best_fat->size;
		}
		mh = (struct mach_header_64 *)unused_map;
		unused_map += sizeof(struct mach_header_64);
		unused_size -= sizeof(struct mach_header_64);
	}
	*bitswap = false;
	switch (mh->magic) {
	case MH_CIGAM_64:
		*bitswap = true;
	case MH_MAGIC_64:
		break;
	default:
		error_log("Can't handle this magic");
		return false;
	}
	if (*bitswap) {
		swap_mach_header_64(mh, arch->byteorder);
	}
	*mhOut = mh;
	*unusedMap = unused_map;
	*unusedSize = unused_size;
	return true;
}

struct segment_command_64 *linkedit_segment_64(struct mach_header_64 *mh, size_t size, bool bitswap, uint8_t **linkedit, size_t *linkeditSize) {
	uint8_t *unused_map = (uint8_t *)mh + sizeof(struct mach_header_64);
	size_t unused_size = size - sizeof(struct mach_header_64);

	if (unused_size < mh->sizeofcmds)
		return NULL;
	struct load_command lc = {0};
	for (int i = 0; i < mh->ncmds; i++) {
		if (unused_size < sizeof(struct load_command))
			return NULL;
		lc = *(struct load_command *)unused_map;
		if (bitswap)
			swap_load_command(&lc, arch->byteorder);
		switch (lc.cmd) {
		case LC_SEGMENT_64: {
			if (unused_size < sizeof(struct segment_command_64))
				return NULL;
			struct segment_command_64 *cmd = (struct segment_command_64 *)unused_map;
			if (bitswap)
				swap_segment_command_64(cmd, arch->byteorder);
			debug_log("Segment: %s", cmd->segname);
			if (strcmp(cmd->segname, SEG_LINKEDIT))
				break;

			// Found LINKEDIT SEG
			*linkedit = (uint8_t *)mh + cmd->fileoff;
			*linkeditSize = cmd->filesize;
			debug_log("Found linkedit at %llx", cmd->fileoff);
			return cmd;
		} break;
		default:
			break;
		}
		unused_map += lc.cmdsize;
		unused_size -= lc.cmdsize;
	}
	error_log("Failed to find __LINKEDIT");
	return NULL;
}

bool get_object_signature(const char *obj, CS_SuperBlob **out_sig, size_t *out_sig_size, uint64_t *file_offset, bool *is_dylib) {
	int fd = open(obj, O_RDONLY);
	if (fd == -1) {
		error_log("Unable to open object: \"%s\"", obj);
		return false;
	}
	_scope({ close(fd); });
	struct stat st;
	st.st_mode = 0;
	st.st_size = 0;
	if (fstat(fd, &st) != 0) {
		error_log("Unable to stat object: \"%s\"", obj);
		return false;
	}
	if (S_ISDIR(st.st_mode)) {
		error_log("Object is a directory: \"%s\"", obj);
		return false;
	}
	size_t size = st.st_size;
	if (size < sizeof(struct mach_header_64)) {
		error_log("Object is too small: \"%s\"", obj);
		return false;
	}
	uint8_t *map = (uint8_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED) {
		error_log("Unable to map object: \"%s\"", obj);
		return false;
	}
	_scope({ munmap(map, size); });
	uint8_t *unused_map = map;
	size_t unused_size = size;
	struct mach_header_64 *mh;
	uint8_t *linkedit;
	size_t linkedit_size;
	struct segment_command_64 *linkedit_cmd;
	bool bitswap;
	struct load_command lc = {0};
	if (!get_object_header(map, size, &unused_map, &unused_size, &mh, &bitswap, file_offset) || !(linkedit_cmd = linkedit_segment_64(mh, unused_size + sizeof(struct mach_header_64), bitswap, &linkedit, &linkedit_size))) {
		error_log("Unable to get mach header for object: \"%s\"", obj);
		return false;
	}

	if (is_dylib != NULL) {
		*is_dylib = (mh->filetype == MH_DYLIB);
	}

	if (unused_size < mh->sizeofcmds) {
		error_log("Invalid object: \"%s\"", obj);
		return false;
	}
	for (int i = 0; i < mh->ncmds; i++) {
		if (unused_size < sizeof(struct load_command)) {
			error_log("Invalid object: \"%s\"", obj);
			return false;
		}
		lc = *(struct load_command *)unused_map;
		if (bitswap)
			swap_load_command(&lc, arch->byteorder);
		switch (lc.cmd) {
		case LC_CODE_SIGNATURE: {
			if (unused_size < sizeof(struct linkedit_data_command)) {
				error_log("Invalid object: \"%s\"", obj);
				return false;
			}
			struct linkedit_data_command *cmd = (struct linkedit_data_command *)unused_map;
			if (bitswap)
				swap_linkedit_data_command(cmd, arch->byteorder);
			size_t sig_offset = (uint64_t)mh - (uint64_t)map + cmd->dataoff;
			if (sig_offset + cmd->datasize > size) {
				error_log("Invalid object: \"%s\"", obj);
				return false;
			}
			CS_SuperBlob *sig_ptr = (CS_SuperBlob *)(map + sig_offset);

			size_t sig_size = cmd->datasize;
			if (sig_offset + sig_size > size) {
				error_log("Invalid object: \"%s\"", obj);
				return false;
			}

			if (!sig_ptr || sig_size < sizeof(CS_SuperBlob) || ntohl(sig_ptr->magic) != CSMAGIC_EMBEDDED_SIGNATURE) {
				error_log("Unable to find signature in object: \"%s\"", obj);
				return false;
			}

			CS_SuperBlob *sig = (CS_SuperBlob *)malloc(sig_size);
			if (sig == NULL) {
				error_log("Unable to allocate memory for signature: %s", strerror(errno));
				return false;
			}
			memcpy(sig, sig_ptr, sig_size);

			*out_sig_size = sig_size;
			*out_sig = sig;
			return true;
		} break;
		default:
			break;
		}
		unused_map += lc.cmdsize;
		unused_size -= lc.cmdsize;
	}
	return false;
}

extern "C" bool cs_forgery_deinit(cs_forgery_ctx_t *ctx) {
	if (ctx->chain != NULL) {
		CFRelease(ctx->chain);
		ctx->chain = NULL;
	}
	if (ctx->signer != NULL) {
		CFRelease(ctx->signer);
		ctx->signer = NULL;
	}
	return true;
}

void get_all_objects(const char *obj, stringhash_t libs, stringarray_t rpath, char *executablePath) {
	uint8_t *saved_map;
	size_t saved_size;

	if (stringhash_hasKey(libs, obj)) {
		return;
	}
	int fd = open(obj, O_RDONLY);
	if (fd == -1) {
		return;
	}
	_scope({ close(fd); });
	struct stat st;
	st.st_mode = 0;
	st.st_size = 0;
	if (fstat(fd, &st) != 0) {
		return;
	}
	if (S_ISDIR(st.st_mode)) {
		return;
	}
	stringhash_setKey(libs, obj, "");
	size_t size = st.st_size;
	if (size < sizeof(struct mach_header_64)) {
		return;
	}
	char *loaderPath = find_dir_path(obj);
	if (loaderPath == NULL) {
		return;
	}
	_scope({ free(loaderPath); });

	uint8_t *map = (uint8_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED) {
		return;
	}
	_scope({ munmap(map, size); });

	uint8_t *unused_map = map;
	size_t unused_size = size;
	bool bitswap;
	struct load_command lc = {0};

	struct mach_header_64 *mh;
	if (!get_object_header(map, size, &unused_map, &unused_size, &mh, &bitswap, NULL)) {
		error_log("Invalid object: \"%s\"", obj);
		return;
	}

	if (unused_size < mh->sizeofcmds) {
		error_log("Invalid object: \"%s\"", obj);
		return;
	}

	saved_map = unused_map;
	saved_size = unused_size;

	for (int i = 0; i < mh->ncmds; i++) {
		if (unused_size < sizeof(struct load_command)) {
			error_log("Invalid object: \"%s\"", obj);
			return;
		}
		lc = *(struct load_command *)unused_map;
		if (bitswap)
			swap_load_command(&lc, arch->byteorder);
		if (lc.cmd == LC_RPATH) {
			if (unused_size < sizeof(struct rpath_command)) {
				error_log("Invalid object: \"%s\"", obj);
				return;
			}
			struct rpath_command *rpath_cmd = (struct rpath_command *)unused_map;
			if (bitswap)
				swap_rpath_command(rpath_cmd, arch->byteorder);
			char *rpath_entry = strdup((const char *)((uintptr_t)rpath_cmd + rpath_cmd->path.offset));
			if (rpath_entry[0] == '@') {
				char *new_rpath_entry = resolve_path(rpath_entry, rpath, executablePath, loaderPath, true);
				free(rpath_entry);
				rpath_entry = new_rpath_entry;
			}
			if (rpath_entry != NULL) {
				stringarray_push(rpath, rpath_entry);
				debug_log("rpath_entry: %s", rpath_entry);
			}
		}
		unused_map += lc.cmdsize;
		unused_size -= lc.cmdsize;
	}
	unused_map = saved_map;
	unused_size = saved_size;
	for (int i = 0; i < mh->ncmds; i++) {
		if (unused_size < sizeof(struct load_command)) {
			error_log("Invalid object: \"%s\"", obj);
			return;
		}
		lc = *(struct load_command *)unused_map;
		switch (lc.cmd) {
		case LC_LOAD_DYLIB:
		case LC_LOAD_WEAK_DYLIB:
		case LC_REEXPORT_DYLIB: {
			if (unused_size < sizeof(struct dylib_command)) {
				error_log("Invalid object: \"%s\"", obj);
				return;
			}
			struct dylib_command *dl = (struct dylib_command *)unused_map;
			if (bitswap)
				swap_dylib_command(dl, arch->byteorder);
			char *dylib_name = strdup((const char *)(uintptr_t)dl + dl->dylib.name.offset);
			if (dylib_name[0] == '@') {
				char *new_dylib_name = resolve_path(dylib_name, rpath, executablePath, loaderPath, false);
				free(dylib_name);
				dylib_name = new_dylib_name;
			}
			debug_log("dylib %s", dylib_name);
			if (dylib_name != NULL) {
				get_all_objects(dylib_name, libs, rpath, executablePath);
				free(dylib_name);
			}
		} break;
		}
		unused_map += lc.cmdsize;
		unused_size -= lc.cmdsize;
	}
}

bool build_entitlements(const char *path, CS_CodeDirectory *codedir, CS_GenericBlob *entitlements, char *&xml_data, size_t &xml_size, bool &updated_entitlements, bool &added_entitlements) {
	CFMutableDictionaryRef entitlementsDict;
	CFPropertyListFormat plistFormat;
	if (entitlements != NULL) {
		size_t entitlementsSize = ntohl(entitlements->length) - sizeof(CS_GenericBlob) + 1;
		CFDataRef entitlementsData = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (UInt8 *)entitlements->data, (CFIndex)entitlementsSize, kCFAllocatorNull);
		if (entitlementsData == NULL) {
			error_log("Unable to CFDataCreateWithBytesNoCopy");
			return false;
		}
		CFPropertyListRef entitlementsPlist = CFPropertyListCreateWithData(kCFAllocatorDefault, entitlementsData, kCFPropertyListMutableContainersAndLeaves, &plistFormat, NULL);
		CFRelease(entitlementsData);
		if (entitlementsPlist == NULL) {
			error_log("Unable to CFPropertyListCreateWithData");
			return false;
		}
		entitlementsDict = (CFMutableDictionaryRef)entitlementsPlist;
	} else {
		plistFormat = kCFPropertyListXMLFormat_v1_0;
		entitlementsDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		if (entitlementsDict == NULL) {
			error_log("Unable to CFDictionaryCreateMutable");
			return false;
		}
		added_entitlements = true;
	}
	_scope({ CFRelease(entitlementsDict); });
	auto setEntitlement = [&](CFStringRef name, CFTypeRef value) {
		if (!added_entitlements) {
			CFTypeRef currentValue = CFDictionaryGetValue(entitlementsDict, name);
			if (currentValue != NULL && CFEqual(currentValue, value)) {
				return;
			}
		}
		CFDictionarySetValue(entitlementsDict, name, value);
		updated_entitlements = true;
	};
	setEntitlement(CFSTR("platform-application"), kCFBooleanTrue);
	setEntitlement(CFSTR("com.apple.private.skip-library-validation"), kCFBooleanTrue);
	setEntitlement(CFSTR("get-task-allow"), kCFBooleanTrue);
	if (strcmp(path, "/bin/launchctl") == 0) {
		setEntitlement(CFSTR("com.apple.private.xpc.launchd.userspace-reboot"), kCFBooleanTrue);
	}
	if ((ntohl(codedir->flags) & CS_ADHOC) == 0) {
		const char *applicationsDir = "/Applications/";
		if (strncmp(path, applicationsDir, strlen(applicationsDir)) == 0) {
			setEntitlement(CFSTR("com.apple.private.security.no-container"), kCFBooleanTrue);
		}
		const char *dataFS = "/private/var/";
		if (strncmp(path, dataFS, strlen(dataFS)) == 0) {
			const char *containersDir = "/private/var/containers/Bundle/Application/";
			if (strncmp(path, containersDir, strlen(containersDir)) != 0) {
				setEntitlement(CFSTR("com.apple.private.security.container-required"), kCFBooleanFalse);
			}
		}
	}
	if (updated_entitlements) {
		CFDataRef newEntitlementsData = CFPropertyListCreateData(kCFAllocatorDefault, (CFPropertyListRef)entitlementsDict, plistFormat, 0, NULL);
		if (newEntitlementsData == NULL) {
			error_log("Unable to CFPropertyListCreateXMLData");
			return false;
		}
		_scope({ CFRelease(newEntitlementsData); });
		xml_size = (size_t)CFDataGetLength(newEntitlementsData);
		xml_data = (char *)malloc(xml_size + sizeof(char));
		if (xml_data == NULL) {
			error_log("Unable to allocate memory for new entitlements: %s", strerror(errno));
			return false;
		}
		CFDataGetBytes(newEntitlementsData, CFRangeMake(0, (CFIndex)xml_size), (UInt8 *)xml_data);
		xml_data[xml_size] = '\0';
		debug_log("Created new entitlements: %s", xml_data);
	}
	return true;
}

bool build_signature(cs_forgery_ctx_t *ctx, bool &added_entitlements, bool &updated_entitlements, char *xml_data, size_t xml_size, CS_SuperBlob *&sig, size_t &sig_size, CS_CodeDirectory *&codedir, uint32_t &csb_offset, CS_GenericBlob *&entitlements, uint32_t &entitlements_offset) {
	if ((ntohl(codedir->flags) & CS_ADHOC) != 0) {
		codedir->flags = ntohl(ntohl(codedir->flags) & ~CS_ADHOC);
	}
	if (added_entitlements) {
		CS_GenericBlob *entblob = NULL;
		CS_SuperBlob *new_sig = (CS_SuperBlob *)csblob_insert_blob(sig, (void *)xml_data, (uint32_t)xml_size, (uint32_t)sig_size, (uint32_t *)&sig_size, CSMAGIC_EMBEDDED_ENTITLEMENTS, CSSLOT_ENTITLEMENTS, &entblob);
		free(sig);
		sig = new_sig;
		if (new_sig == NULL) {
			error_log("Unable to insert blob");
			return false;
		}
		find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset);
		uint8_t enthash[CC_SHA256_DIGEST_LENGTH];
		size_t size = 0;
		if (codedir->hashType == CS_HASHTYPE_SHA1) {
			CC_SHA1((uint8_t *)entblob, ntohl(entblob->length), enthash);
			size = CC_SHA1_DIGEST_LENGTH;
		} else {
			CC_SHA256((uint8_t *)entblob, ntohl(entblob->length), enthash);
			size = CC_SHA256_DIGEST_LENGTH;
		}
		if (ntohl(codedir->nSpecialSlots) < CSSLOT_ENTITLEMENTS) {
			uint32_t add = (CSSLOT_ENTITLEMENTS - ntohl(codedir->nSpecialSlots)) * (uint32_t)size;
			uint32_t newSize = ntohl(codedir->length) + add;
			CS_CodeDirectory *newcodedir = (CS_CodeDirectory *)malloc(newSize);
			if (newcodedir == NULL) {
				error_log("Unable to allocate memory for new code directory: %s", strerror(errno));
				return false;
			}
			_scope({ free(newcodedir); });
			memcpy(newcodedir, codedir, ntohl(codedir->hashOffset));
			newcodedir->length = ntohl(newSize);
			newcodedir->nSpecialSlots = ntohl(CSSLOT_ENTITLEMENTS);
			newcodedir->hashOffset = ntohl(ntohl(codedir->hashOffset) + add);
			memcpy(((uint8_t *)newcodedir) + ntohl(newcodedir->hashOffset), ((uint8_t *)codedir) + ntohl(codedir->hashOffset), ntohl(codedir->length) - ntohl(codedir->hashOffset));
			if (ntohl(codedir->identOffset) >= ntohl(codedir->hashOffset)) {
				newcodedir->identOffset = ntohl(ntohl(codedir->identOffset) + add);
			}
			if (ntohl(codedir->scatterOffset) >= ntohl(codedir->hashOffset)) {
				newcodedir->scatterOffset = ntohl(ntohl(codedir->scatterOffset) + add);
			}
			if (ntohl(codedir->teamOffset) >= ntohl(codedir->hashOffset)) {
				newcodedir->teamOffset = ntohl(ntohl(codedir->teamOffset) + add);
			}
			uint8_t *hashes = ((uint8_t *)newcodedir) + ntohl(newcodedir->hashOffset);
			memset(hashes - CSSLOT_ENTITLEMENTS * size, 0, CSSLOT_ENTITLEMENTS * size);
			if (ntohl(codedir->nSpecialSlots) >= CSSLOT_INFOSLOT) {
				uint8_t *infoHash = ((uint8_t *)codedir) + ntohl(codedir->hashOffset) - CSSLOT_INFOSLOT * size;
				memcpy(hashes - CSSLOT_INFOSLOT * size, infoHash, size);
			}
			if (ntohl(codedir->nSpecialSlots) >= CSSLOT_REQUIREMENTS) {
				uint8_t *reqHash = ((uint8_t *)codedir) + ntohl(codedir->hashOffset) - CSSLOT_REQUIREMENTS * size;
				memcpy(hashes - CSSLOT_REQUIREMENTS * size, reqHash, size);
			}
			memcpy(hashes - CSSLOT_ENTITLEMENTS * size, enthash, size);
			new_sig = (CS_SuperBlob *)csblob_patch_at_offset(sig, (void *)((uintptr_t)newcodedir + 8), newSize, ntohl(codedir->length) - sizeof(CS_GenericBlob), (uint32_t)sig_size, (uint32_t *)&sig_size, CSMAGIC_CODEDIRECTORY, CSSLOT_CODEDIRECTORY, csb_offset, (CS_GenericBlob **)&codedir);
			free(sig);
			sig = new_sig;
			if (sig == NULL) {
				error_log("Unable patch blob at offset");
				return false;
			}
			find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset);
		} else {
			uint8_t *hashes = (uint8_t *)codedir + ntohl(codedir->hashOffset);
			memcpy(hashes - CSSLOT_ENTITLEMENTS * size, enthash, size);
		}
	} else if (updated_entitlements) {
		CS_GenericBlob *entblob = NULL;
		CS_SuperBlob *new_sig = (CS_SuperBlob *)csblob_patch(sig, (void *)xml_data, (uint32_t)xml_size, ntohl(entitlements->length) - sizeof(CS_GenericBlob), (uint32_t)sig_size, (uint32_t *)&sig_size, CSMAGIC_EMBEDDED_ENTITLEMENTS, CSSLOT_ENTITLEMENTS, &entblob);
		free(sig);
		sig = new_sig;
		if (sig == NULL) {
			error_log("Unable to patch blob");
			return false;
		}
		find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset);
		uint8_t enthash[CC_SHA256_DIGEST_LENGTH];
		size_t size = 0;
		if (codedir->hashType == CS_HASHTYPE_SHA1) {
			CC_SHA1((uint8_t *)entblob, ntohl(entblob->length), enthash);
			size = CC_SHA1_DIGEST_LENGTH;
		} else {
			CC_SHA256((uint8_t *)entblob, ntohl(entblob->length), enthash);
			size = CC_SHA256_DIGEST_LENGTH;
		}
		uint8_t *hashes = (uint8_t *)codedir + ntohl(codedir->hashOffset);
		memcpy(hashes - CSSLOT_ENTITLEMENTS * size, enthash, size);
	}
	CS_GenericBlob *cms = csblob_find_blob_bytes((const uint8_t *)sig, sig_size, CSSLOT_SIGNATURESLOT, CSMAGIC_BLOBWRAPPER);
	CS_CodeDirectory *other_cdir = (CS_CodeDirectory *)csblob_find_blob_bytes((const uint8_t *)sig, sig_size, CSSLOT_CODEDIRECTORY, CSMAGIC_CODEDIRECTORY);
	CS_GenericBlob *new_cms = NULL;
	size_t new_cms_size = 0;
	if (!build_cms_blob(ctx, codedir, ntohl(codedir->length), other_cdir, ntohl(other_cdir->length), (char **)&new_cms, &new_cms_size)) {
		error_log("Unable to build CMS blob");
		return false;
	}
	_scope({ free(new_cms); });
	CS_SuperBlob *new_sig;
	if (cms != NULL) {
		new_sig = (CS_SuperBlob *)csblob_patch(sig, (void *)new_cms, (uint32_t)new_cms_size, ntohl(cms->length) - sizeof(CS_GenericBlob), (uint32_t)sig_size, (uint32_t *)&sig_size, CSMAGIC_BLOBWRAPPER, CSSLOT_SIGNATURESLOT, NULL);
		if (new_sig == NULL) {
			error_log("Unable to patch CMS blob");
			return false;
		}
	} else {
		new_sig = (CS_SuperBlob *)csblob_insert_blob(sig, (void *)new_cms, (uint32_t)new_cms_size, (uint32_t)sig_size, (uint32_t *)&sig_size, CSMAGIC_BLOBWRAPPER, CSSLOT_SIGNATURESLOT, NULL);
		if (new_sig == NULL) {
			error_log("Unable to insert CMS blob");
			return false;
		}
	}
	free(sig);
	sig = new_sig;
	find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset);
	debug_log("new_sig: %p", new_sig);
	return true;
}

extern "C" bool cs_forgery_init(cs_forgery_ctx_t *ctx, const char *signcert) {
	pthread_once(&init_once, init);
	bzero(ctx, sizeof(*ctx));
	int fd = open(signcert, O_RDONLY);
	if (fd == -1) {
		error_log("Unable to open signing certificate: %s", strerror(errno));
		return false;
	}
	_scope({ close(fd); });
	struct stat st;
	if (fstat(fd, &st) == -1) {
		error_log("Unable to stat signing certificate: %s", strerror(errno));
		return false;
	}
	void *cert = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (cert == MAP_FAILED) {
		error_log("Unable to map signing certificate: %s", strerror(errno));
		return false;
	}
	_scope({ munmap(cert, st.st_size); });
	CFDataRef pkcs12_data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (UInt8 *)cert, (CFIndex)st.st_size, kCFAllocatorNull);
	if (pkcs12_data == NULL) {
		error_log("Unable to CFDataCreateWithBytesNoCopy");
		return false;
	}
	_scope({ CFRelease(pkcs12_data); });
	CFTypeRef keys[] = {SecSym(kSecImportExportPassphrase)};
	CFTypeRef values[] = {CFSTR("")};
	CFDictionaryRef options = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (options == NULL) {
		error_log("Unable to CFDictionaryCreate");
		return false;
	}
	_scope({ CFRelease(options); });
	CFArrayRef items = NULL;
	SecCall(SecPKCS12Import, pkcs12_data, options, &items);
	_scope({ CFRelease(items); });
	CFDictionaryRef item = (CFDictionaryRef)CFArrayGetValueAtIndex(items, 0);
	SecIdentityRef signer = (SecIdentityRef)CFDictionaryGetValue(item, SecSym(kSecImportItemIdentity));
	if (signer == NULL) {
		error_log("Unable to get signer");
		return false;
	}
	CFRetain(signer);
	ctx->signer = signer;
	CFArrayRef fullchain = (CFArrayRef)CFDictionaryGetValue(item, SecSym(kSecImportItemCertChain));
	if (fullchain == NULL) {
		error_log("Unable to get full chain");
		return false;
	}
	CFMutableArrayRef chain = CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, fullchain);
	if (chain == NULL) {
		error_log("Unable to CFArrayCreateMutableCopy");
		return false;
	}
	CFArrayRemoveValueAtIndex(chain, 0);
	ctx->chain = chain;
	return true;
}

extern "C" bool cs_forgery_forge(cs_forgery_ctx_t *ctx, const char *path, CS_SuperBlob **sig_p, size_t *size_p, uint64_t *file_offset_p, uint8_t real_cdhash[CS_CDHASH_LEN], uint8_t fake_cdhash[CS_CDHASH_LEN]) {
	pthread_once(&init_once, init);
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		error_log("Unable to open \"%s\": %s", path, strerror(errno));
		return false;
	}
	_scope({ close(fd); });
	struct stat st;
	if (fstat(fd, &st) == -1) {
		error_log("Unable to stat \"%s\": %s", path, strerror(errno));
		return false;
	}
	CS_SuperBlob *sig = NULL;
	size_t sig_size = 0;
	uint64_t file_offset = 0;
	bool is_dylib = false;
	if (!get_object_signature(path, &sig, &sig_size, &file_offset, &is_dylib)) {
		error_log("Unable to get signature for object: \"%s\"", path);
		return false;
	}
	_scope({
		if (sig != NULL)
			free(sig);
	});
	CS_CodeDirectory *codedir = NULL;
	uint32_t csb_offset = 0;
	CS_GenericBlob *entitlements = NULL;
	uint32_t entitlements_offset = 0;
	if (!find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset)) {
		error_log("Unable to find code directory for object: \"%s\"", path);
		return false;
	}
	if (!hash_code_directory(codedir, real_cdhash)) {
		error_log("Unable to hash code directory for object: \"%s\"", path);
		return false;
	}
	bool updated_entitlements, added_entitlements;
	updated_entitlements = added_entitlements = false;
	char *xml_data = NULL;
	size_t xml_size = 0;
	debug_log("is_dylib: %d", is_dylib);
	if (!is_dylib && !build_entitlements(path, codedir, entitlements, xml_data, xml_size, updated_entitlements, added_entitlements)) {
		error_log("Unable to build entitlements for object: \"%s\"", path);
		return false;
	}
	_scope({
		if (xml_data != NULL)
			free(xml_data);
	});
	if (!build_signature(ctx, added_entitlements, updated_entitlements, xml_data, xml_size, sig, sig_size, codedir, csb_offset, entitlements, entitlements_offset)) {
		error_log("Unable to build signature for object: \"%s\"", path);
		return false;
	}
	if (!hash_code_directory(codedir, fake_cdhash)) {
		error_log("Unable to hash new code directory for object: \"%s\"", path);
		return false;
	}
	*sig_p = sig;
	*size_p = sig_size;
	*file_offset_p = file_offset;
	sig = NULL;
	return true;
}

extern "C" bool cs_forgery_iterate(const char *path, void (*handler)(const char *obj, void *context), void *context) {
	pthread_once(&init_once, init);
	size_t hashSize = 0x10000;
	stringhash_t libs = stringhash_create(hashSize);
	if (libs == NULL) {
		error_log("Unable to create libraries dictionary");
		return false;
	}
	_scope({ stringhash_destroy(libs); });
	stringarray_t rpath = stringarray_create();
	if (rpath == NULL) {
		error_log("Unable to create rpath array");
		return false;
	}
	_scope({ stringarray_destroy(rpath); });
	stringarray_push(rpath, "/usr/local/lib");
	stringarray_push(rpath, "/usr/lib");
	stringarray_push(rpath, "/Library/Frameworks");
	stringarray_push(rpath, "/Network/Library/Frameworks");
	stringarray_push(rpath, "/System/Library/Frameworks");
	char *executablePath = find_dir_path(path);
	if (executablePath == NULL) {
		error_log("Unable to find executable path");
		return false;
	}
	_scope({ free(executablePath); });
	get_all_objects(path, libs, rpath, executablePath);
	if (stringhash_count(libs) == 0) {
		error_log("Unable to find libraries");
		return false;
	}
	char **libKeys = stringhash_copyAllKeys(libs);
	if (libKeys == NULL) {
		error_log("Unable to copy library keys");
		return false;
	}
	_scope({ free(libKeys); });
	for (char **keyPtr = libKeys; *keyPtr != NULL; keyPtr++) {
		char *key = *keyPtr;
		debug_log("Processing %s", key);
		handler(key, context);
	}
	return true;
}

extern "C" bool cs_forgery_get_cdhash(const char *path, uint8_t cdhash[CS_CDHASH_LEN]) {
	pthread_once(&init_once, init);
	CS_SuperBlob *sig = NULL;
	size_t sig_size = 0;
	uint64_t file_offset = 0;
	if (!get_object_signature(path, &sig, &sig_size, &file_offset, NULL)) {
		error_log("Unable to get signature for object: \"%s\"", path);
		return false;
	}
	_scope({ free(sig); });
	CS_CodeDirectory *codedir = NULL;
	uint32_t csb_offset = 0;
	CS_GenericBlob *entitlements = NULL;
	uint32_t entitlements_offset = 0;
	if (!find_best_codedir((void *)sig, (uint32_t)sig_size, &codedir, &csb_offset, &entitlements, &entitlements_offset)) {
		error_log("Unable to find code directory for object: \"%s\"", path);
		return false;
	}
	if (!hash_code_directory(codedir, cdhash)) {
		error_log("Unable to hash code directory for object: \"%s\"", path);
		return false;
	}
	return true;
}

extern "C" char *cs_forgery_get_interpreter(const char *path) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		return NULL;
	}
	struct stat st;
	if (fstat(fd, &st)) {
		close(fd);
		return NULL;
	}
	char *vdata = (char *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (vdata == MAP_FAILED) {
		return NULL;
	}
	char *ihp;
	char *line_startp, *line_endp;
	char *interp;
	char buffer[MAXPATHLEN];
	/*
	 * Make sure it's a shell script.
	 */
	if (vdata[0] != '#' || vdata[1] != '!') {
		munmap(vdata, st.st_size);
		return NULL;
	}
	buffer[0] = '\0';
	/* Try to find the first non-whitespace character */
	for (ihp = &vdata[2]; ihp < &vdata[IMG_SHSIZE]; ihp++) {
		if (IS_EOL(*ihp)) {
			/* Did not find interpreter, "#!\n" */
			munmap(vdata, st.st_size);
			return NULL;
		} else if (IS_WHITESPACE(*ihp)) {
			/* Whitespace, like "#!    /bin/sh\n", keep going. */
		} else {
			/* Found start of interpreter */
			break;
		}
	}

	if (ihp == &vdata[IMG_SHSIZE]) {
		/* All whitespace, like "#!           " */
		munmap(vdata, st.st_size);
		return NULL;
	}

	line_startp = ihp;

	/* Try to find the end of the interpreter+args string */
	for (; ihp < &vdata[IMG_SHSIZE]; ihp++) {
		if (IS_EOL(*ihp)) {
			/* Got it */
			break;
		} else {
			/* Still part of interpreter or args */
		}
	}

	if (ihp == &vdata[IMG_SHSIZE]) {
		/* A long line, like "#! blah blah blah" without end */
		munmap(vdata, st.st_size);
		return NULL;
	}

	/* Backtrack until we find the last non-whitespace */
	while (IS_EOL(*ihp) || IS_WHITESPACE(*ihp)) {
		ihp--;
	}

	/* The character after the last non-whitespace is our logical end of line */
	line_endp = ihp + 1;

	/*
	 * Now we have pointers to the usable part of:
	 *
	 * "#!  /usr/bin/int first    second   third    \n"
	 *      ^ line_startp                       ^ line_endp
	 */

	/* copy the interpreter name */

	interp = buffer;
	for (ihp = line_startp; (ihp < line_endp) && !IS_WHITESPACE(*ihp); ihp++)
		*interp++ = *ihp;
	*interp = '\0';

	munmap(vdata, st.st_size);
	return strdup(buffer);
}
