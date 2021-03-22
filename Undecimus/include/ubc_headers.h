#ifndef ubc_headers_h
#define ubc_headers_h

#include <sys/sysctl.h>
#include <sys/mount.h>

/* vnode types (vnode->v_type) */
enum vtype {
    /* 0 */
    VNON,
    /* 1 - 5 */
    VREG, VDIR, VBLK, VCHR, VLNK,
    /* 6 - 10 */
    VSOCK, VFIFO, VBAD, VSTR, VCPLX
};

struct tqe_struct {
    uint64_t tqe_next;
    uint64_t tqe_prev;
};
struct tqh_struct {
    uint64_t tqh_first;
    uint64_t tqh_last;
};

struct vnode_struct {
    uint64_t v_lock[2];
    struct tqe_struct v_freelist;
    struct tqe_struct v_mntvnodes;
    struct tqh_struct v_ncchildren;
    uint64_t v_nclinks;
    uint64_t v_defer_reclaimlist;
    uint32_t v_listflag;            /* flags protected by the vnode_list_lock (see below) */
    uint32_t v_flag;            /* vnode flags (see below) */
    uint16_t v_lflag;            /* vnode local and named ref flags */
    uint8_t     v_iterblkflags;        /* buf iterator flags */
    uint8_t     v_references;            /* number of times io_count has been granted */
    int32_t     v_kusecount;            /* count of in-kernel refs */
    int32_t     v_usecount;            /* reference count of users */
    int32_t     v_iocount;            /* iocounters */
    uint64_t   v_owner;            /* void * act that owns the vnode */
    uint16_t v_type;            /* vnode type */
    uint16_t v_tag;                /* type of underlying data */
    uint32_t v_id;                /* identity of vnode contents */
    union {
        struct mount    *vu_mountedhere;/* ptr to mounted vfs (VDIR) */
        struct socket    *vu_socket;    /* unix ipc (VSOCK) */
        struct specinfo    *vu_specinfo;    /* device (VCHR, VBLK) */
        struct fifoinfo    *vu_fifoinfo;    /* fifo (VFIFO) */
        struct ubc_info *vu_ubcinfo;    /* valid for (VREG) */
    } v_un;
    uint64_t v_cleanblkhd;        /* clean blocklist head */
    uint64_t v_dirtyblkhd;        /* dirty blocklist head */
    uint64_t v_knotes;            /* knotes attached to this vnode */
    /*
     * the following 4 fields are protected
     * by the name_cache_lock held in
     * excluive mode
     */
    uint64_t    v_cred;            /* last authorized credential */
    uint64_t    v_authorized_actions;    /* current authorized actions for v_cred */
    int        v_cred_timestamp;    /* determine if entry is stale for MNTK_AUTH_OPAQUE */
    int        v_nc_generation;    /* changes when nodes are removed from the name cache */
    /*
     * back to the vnode lock for protection
     */
    int32_t        v_numoutput;            /* num of writes in progress */
    int32_t        v_writecount;            /* reference count of writers */
    const char *v_name;            /* name component of the vnode */
    uint64_t v_parent;            /* pointer to parent vnode */
    struct lockf *v_lockf;        /* advisory lock list head */
    int     (**v_op)(void *);        /* vnode operations vector */
    mount_t v_mount;            /* ptr to vfs we are in */
    void *    v_data;                /* private data for fs */
} ;

struct ubc_info {
    uint64_t        ui_pager;               /* pager */
    uint64_t        ui_control;             /* VM control for the pager */
    vnode_t         ui_vnode;               /* vnode for this ubc_info */
    kauth_cred_t    ui_ucred;               /* holds credentials for NFS paging */
    int64_t         ui_size;                /* file size for the vnode */
    uint32_t        ui_flags;               /* flags */
    uint32_t        cs_add_gen;             /* generation count when csblob was validated */

    struct cl_readahead     *cl_rahead;     /* cluster read ahead context */
    struct cl_writebehind   *cl_wbehind;    /* cluster write behind context */

    struct timespec         cs_mtime;       /* modify time of file when first cs_blob was loaded */

    struct cs_blob          *cs_blobs;      /* for CODE SIGNING */
    /* rest removed */
};

struct cs_blob {
    struct          cs_blob *csb_next;
    int                csb_cpu_type;
    unsigned int    csb_flags;
    long long        csb_base_offset;                       /* Offset of Mach-O binary in fat binary */
    long long        csb_start_offset;                      /* Blob coverage area start, from csb_base_offset */
    long long        csb_end_offset;                        /* Blob coverage area end, from csb_base_offset */
    unsigned long    csb_mem_size;
    unsigned long    csb_mem_offset;
    unsigned long    csb_mem_kaddr;
    unsigned char    csb_cdhash[CS_CDHASH_LEN];
    const struct    cs_hash  *csb_hashtype;
    unsigned long    csb_hash_pagesize;                     /* each hash entry represent this many bytes */
    unsigned long    csb_hash_pagemask;
    unsigned long    csb_hash_pageshift;
    unsigned long    csb_hash_firstlevel_pagesize;
    const           CS_CodeDirectory *csb_cd;
    const char *    csb_teamid;
    const           CS_GenericBlob *csb_entitlements_blob;  /* raw blob, subrange of csb_mem_kaddr */
    void *          csb_entitlements;                       /* The entitlements as an OSDictionary */
    unsigned int    csb_signer_type;
    unsigned int    csb_reconstituted;    /* signature has potentially been modified after validation */
    unsigned int    csb_platform_binary;
    unsigned int    csb_platform_path;
    
#if __arm64e__
    void *pmap_cs_entry;
#endif
};

struct cs_hash {
    uint8_t         cs_type;            /* type code as per code signing */
    size_t          cs_size;            /* size of effective hash (may be truncated) */
    size_t          cs_digest_size;     /* size of native hash */
    uint64_t        cs_init;
    uint64_t        cs_update;
    uint64_t        cs_final;
};

struct vnode_attr {
    /* bitfields */
    uint64_t va_supported;
    uint64_t va_active;
    
    /*
     * Control flags.  The low 16 bits are reserved for the
     * ioflags being passed for truncation operations.
     */
    int va_vaflags;
    
    /* traditional stat(2) parameter fields */
    dev_t       va_rdev;                /* device id (device nodes only) */
    uint64_t    va_nlink;               /* number of references to this file */
    uint64_t    va_total_size;          /* size in bytes of all forks */
    uint64_t    va_total_alloc;         /* disk space used by all forks */
    uint64_t    va_data_size;           /* size in bytes of the fork managed by current vnode */
    uint64_t    va_data_alloc;          /* disk space used by the fork managed by current vnode */
    uint32_t    va_iosize;              /* optimal I/O blocksize */
    
    /* file security information */
    uid_t               va_uid;         /* owner UID */
    gid_t               va_gid;         /* owner GID */
    mode_t              va_mode;        /* posix permissions */
    uint32_t            va_flags;       /* file flags */
    struct kauth_acl    *va_acl;        /* access control list */
    
    /* timestamps */
    struct timespec va_create_time;     /* time of creation */
    struct timespec va_access_time;     /* time of last access */
    struct timespec va_modify_time;     /* time of last data modification */
    struct timespec va_change_time;     /* time of last metadata change */
    struct timespec va_backup_time;     /* time of last backup */
};

#endif
