#include "cs_dingling.h"
#include <sys/mman.h>
#include <spawn.h>
#include <sys/stat.h>
#include <mach-o/loader.h>
#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonDigest.h>

// Finds the LC_CODE_SIGNATURE load command
const void *find_code_signature(img_info_t *info, uint32_t *cs_size) {
    init_function();
    if (info == NULL || info->addr == NULL) {
        return NULL;
    }
    
    // mach_header_64 is mach_header + reserved for padding
    const struct mach_header *mh = (const struct mach_header*)info->addr;
    
    uint32_t sizeofmh = 0;
    
    switch (mh->magic) {
        case MH_MAGIC_64:
            sizeofmh = sizeof(struct mach_header_64);
            break;
        case MH_MAGIC:
            sizeofmh = sizeof(struct mach_header);
            break;
        default:
            LOG("your magic is not valid in these lands: %08x", mh->magic);
            return NULL;
    }
    
    if (mh->sizeofcmds < mh->ncmds * sizeof(struct load_command)) {
        LOG("Corrupted macho (sizeofcmds < ncmds * sizeof(lc))");
        return NULL;
    }
    if (mh->sizeofcmds + sizeofmh > info->size) {
        LOG("Corrupted macho (sizeofcmds + sizeof(mh) > size)");
        return NULL;
    }
    
    const struct load_command *cmd = (const struct load_command *)((uintptr_t) info->addr + sizeofmh);
    for (int i = 0; i != mh->ncmds; ++i) {
        if (cmd->cmd == LC_CODE_SIGNATURE) {
            const struct linkedit_data_command* cscmd = (const struct linkedit_data_command*)cmd;
            if (cscmd->dataoff + cscmd->datasize > info->size) {
                LOG("Corrupted LC_CODE_SIGNATURE: dataoff + datasize > fsize");
                return NULL;
            }
            
            if (cs_size) {
                *cs_size = cscmd->datasize;
            }
            
            return (const uint8_t*)((uintptr_t)info->addr + cscmd->dataoff);
        }
        
        cmd = (const struct load_command *)((uintptr_t)cmd + cmd->cmdsize);
        if ((uintptr_t)cmd + sizeof(struct load_command) > (uintptr_t)info->addr + info->size) {
            LOG("Corrupted macho: Unexpected end of file while parsing load commands");
            return NULL;
        }
    }
    
    LOG("Didnt find the code signature");
    return NULL;
}

#define BLOB_FITS(blob, size) ((size >= sizeof(*blob)) && (size >= ntohl(blob->length)))

// xnu-3789.70.16/bsd/kern/ubc_subr.c#470
int find_best_codedir(const void *csblob,
                      uint32_t blob_size,
                      const CS_CodeDirectory **chosen_cd,
                      uint32_t *csb_offset,
                      const CS_GenericBlob **entitlements,
                      uint32_t *entitlements_offset) {
    init_function();
    *chosen_cd = NULL;
    *entitlements = NULL;
    
    const CS_GenericBlob *blob = (const CS_GenericBlob *)csblob;
    
    if (!BLOB_FITS(blob, blob_size)) {
        LOG("csblob too small even for generic blob");
        return 1;
    }
    
    uint32_t length = ntohl(blob->length);
    
    if (ntohl(blob->magic) == CSMAGIC_EMBEDDED_SIGNATURE) {
        const CS_CodeDirectory *best_cd = NULL;
        int best_rank = 0;
        
        const CS_SuperBlob *sb = (const CS_SuperBlob *)csblob;
        uint32_t count = ntohl(sb->count);
        
        if (!BLOB_FITS(sb, blob_size)) {
            LOG("csblob too small for superblob");
            return 1;
        }
        
        for (int n = 0; n < count; n++){
            const CS_BlobIndex *blobIndex = &sb->index[n];
            
            uint32_t type = ntohl(blobIndex->type);
            uint32_t offset = ntohl(blobIndex->offset);
            
            if (length < offset) {
                LOG("offset of blob #%d overflows superblob length", n);
                return 1;
            }
            
            const CS_GenericBlob *subBlob = (const CS_GenericBlob *)((uintptr_t)csblob + offset);
            
            if (type == CSSLOT_CODEDIRECTORY || (type >= CSSLOT_ALTERNATE_CODEDIRECTORIES && type < CSSLOT_ALTERNATE_CODEDIRECTORY_LIMIT)) {
                const CS_CodeDirectory *candidate = (const CS_CodeDirectory *)subBlob;
                
                unsigned int rank = hash_rank(candidate);
                
                // Apple's code: `rank > best_rank` (kind of obvious, right?)
                // So why is it I have to switch it to get it to work?
                // macos-10.12.6-sierra/xnu-3789.70.16/bsd/kern/ubc_subr.c#534
                if (best_cd == NULL || ((kCFCoreFoundationVersionNumber >= 1443.00)?rank > best_rank:rank < best_rank)) {
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
        *chosen_cd = (const CS_CodeDirectory *)blob;
        *csb_offset = 0;
    } else {
        LOG("Unknown magic at csblob start: %08x", ntohl(blob->magic));
        return 1;
    }
    
    if (chosen_cd == NULL) {
        LOG("didn't find codedirectory to hash");
        return 1;
    }
    
    return 0;
}

// xnu-3789.70.16/bsd/kern/ubc_subr.c#231
static unsigned int hash_rank(const CS_CodeDirectory *cd) {
    init_function();
    uint32_t type = cd->hashType;
    
    for (unsigned int n = 0; n < sizeof(hashPriorities) / sizeof(hashPriorities[0]); ++n) {
        if (hashPriorities[n] == type) {
            return n + 1;
        }
    }
    
    return 0;
}

int hash_code_directory(const CS_CodeDirectory *directory, uint8_t hash[CS_CDHASH_LEN]) {
    init_function();
    uint32_t realsize = ntohl(directory->length);
    
    if (ntohl(directory->magic) != CSMAGIC_CODEDIRECTORY) {
        LOG("expected CSMAGIC_CODEDIRECTORY");
        return 1;
    }
    
    uint8_t out[CS_HASH_MAX_SIZE];
    uint8_t hash_type = directory->hashType;
    
    switch (hash_type) {
        case CS_HASHTYPE_SHA1:
            CC_SHA1(directory, realsize, out);
            break;
            
        case CS_HASHTYPE_SHA256:
        case CS_HASHTYPE_SHA256_TRUNCATED:
            CC_SHA256(directory, realsize, out);
            break;
            
        case CS_HASHTYPE_SHA384:
            CC_SHA384(directory, realsize, out);
            break;
            
        default:
            LOG("Unknown hash type: 0x%x", hash_type);
            return 2;
    }
    
    memcpy(hash, out, CS_CDHASH_LEN);
    return 0;
}

const char *get_hash_name(uint8_t hash_type) {
    init_function();
    switch (hash_type) {
        case CS_HASHTYPE_SHA1:
            return "SHA1";
        
        case CS_HASHTYPE_SHA256:
        case CS_HASHTYPE_SHA256_TRUNCATED:
            return "SHA256";
            
        case CS_HASHTYPE_SHA384:
            return "SHA384";
            
        default:
            return "UNKNWON";
    }
    
    return "";
}

int open_img(img_info_t* info) {
    init_function();
    int ret = -1;
    
    if (info == NULL) {
        LOG("img info is NULL");
        return ret;
    }
    
    info->fd = -1;
    info->size = 0;
    info->addr = NULL;
    
    info->fd = open(info->name, O_RDONLY);
    if (info->fd == -1) {
        LOG("Couldn't open file");
        ret = 1;
        goto out;
    }
    
    struct stat s;
    if (fstat(info->fd, &s) != 0) {
        LOG("fstat: 0x%x (%s)", errno, strerror(errno));
        ret = 2;
        goto out;
    }
    
    size_t fsize = s.st_size;
    info->size = fsize - info->file_off;
    const void *map = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, info->fd, 0);
    
    if (map == MAP_FAILED) {
        LOG("mmap: 0x%x (%s)", errno, strerror(errno));
        ret = 4;
        goto out;
    }
    
    info->addr = (const void*) ((uintptr_t) map + info->file_off);
    ret = 0;
    
    out:;
    if (ret) {
        close_img(info);
    }
    return ret;
}

void close_img(img_info_t* info) {
    init_function();
    if (info == NULL) {
        return;
    }
    
    if (info->addr != NULL) {
        const void *map = (void*) ((uintptr_t) info->addr - info->file_off);
        size_t fsize = info->size + info->file_off;
        
        munmap((void*)map, fsize);
    }
    
    if (info->fd != -1) {
        close(info->fd);
    }
}
