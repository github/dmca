// Based from xnu-4570.41.2/EXTERNAL_HEADERS/mach-o/loader.h
// https://opensource.apple.com/source/xnu/xnu-4570.41.2/EXTERNAL_HEADERS/mach-o/loader.h.auto.html

#ifndef LIBMACHO_H
#define LIBMACHO_H

#include <stdint.h>

typedef int integer_t;
typedef integer_t cpu_type_t;
typedef integer_t cpu_subtype_t;
typedef int vm_prot_t;

typedef struct mach_header {
    uint32_t        magic;      /* mach magic number identifier */
    cpu_type_t      cputype;    /* cpu specifier */
    cpu_subtype_t   cpusubtype; /* machine specifier */
    uint32_t        filetype;   /* type of file */
    uint32_t        ncmds;      /* number of load commands */
    uint32_t        sizeofcmds; /* the size of all the load commands */
    uint32_t        flags;      /* flags */
} mach_header;

typedef struct mach_header_64 {
    uint32_t        magic;      /* mach magic number identifier */
    cpu_type_t      cputype;    /* cpu specifier */
    cpu_subtype_t   cpusubtype; /* machine specifier */
    uint32_t        filetype;   /* type of file */
    uint32_t        ncmds;      /* number of load commands */
    uint32_t        sizeofcmds; /* the size of all the load commands */
    uint32_t        flags;      /* flags */
    uint32_t        reserved;   /* reserved */
} mach_header_64;

// mach_header_64.magic
#define MH_MAGIC_64 0xfeedfacf /* the 64-bit mach magic number */
#define MH_CIGAM_64 0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */

// mach_header.filetype:
#define MH_OBJECT       0x1  /* relocatable object file */
#define MH_EXECUTE      0x2  /* demand paged executable file */
#define MH_FVMLIB       0x3  /* fixed VM shared library file */
#define MH_CORE         0x4  /* core file */
#define MH_PRELOAD      0x5  /* preloaded executable file */
#define MH_DYLIB        0x6  /* dynamically bound shared library */
#define MH_DYLINKER     0x7  /* dynamic link editor */
#define MH_BUNDLE       0x8  /* dynamically bound bundle file */
#define MH_DYLIB_STUB   0x9  /* shared library stub for static linking only, no section contents */
#define MH_DSYM         0xa  /* companion file with only debug sections */
#define MH_KEXT_BUNDLE  0xb  /* x86_64 kexts */

// mach_header.flags:
#define MH_NOUNDEFS                 0x1         /* the object file has no undefined references */
#define MH_INCRLINK                 0x2         /* the object file is the output of an incremental link against a base file and can't be link edited again */
#define MH_DYLDLINK                 0x4         /* the object file is input for the dynamic linker and can't be staticly link edited again */
#define MH_BINDATLOAD               0x8         /* the object file's undefined references are bound by the dynamic linker when loaded. */
#define MH_PREBOUND                 0x10        /* the file has its dynamic undefined references prebound. */
#define MH_SPLIT_SEGS               0x20        /* the file has its read-only and read-write segments split */
#define MH_LAZY_INIT                0x40        /* the shared library init routine is to be run lazily via catching memory faults to its writeable segments (obsolete) */
#define MH_TWOLEVEL                 0x80        /* the image is using two-level name space bindings */
#define MH_FORCE_FLAT               0x100       /* the executable is forcing all images to use flat name space bindings */
#define MH_NOMULTIDEFS              0x200       /* this umbrella guarantees no multiple defintions of symbols in its sub-images so the two-level namespace hints can always be used. */
#define MH_NOFIXPREBINDING          0x400       /* do not have dyld notify the prebinding agent about this executable */
#define MH_PREBINDABLE              0x800       /* the binary is not prebound but can have its prebinding redone. only used when MH_PREBOUND is not set. */
#define MH_ALLMODSBOUND             0x1000      /* indicates that this binary binds to all two-level namespace modules of its dependent libraries. only used when MH_PREBINDABLE and MH_TWOLEVEL are both set. */
#define MH_SUBSECTIONS_VIA_SYMBOLS  0x2000      /* safe to divide up the sections into sub-sections via symbols for dead code stripping */
#define MH_CANONICAL                0x4000      /* the binary has been canonicalized via the unprebind operation */
#define MH_WEAK_DEFINES             0x8000      /* the final linked image contains external weak symbols */
#define MH_BINDS_TO_WEAK            0x10000     /* the final linked image uses weak symbols */
#define MH_ALLOW_STACK_EXECUTION    0x20000     /* When this bit is set, all stacks  in the task will be given stack execution privilege.  Only used in MH_EXECUTE filetypes. */
#define MH_ROOT_SAFE                0x40000     /* When this bit is set, the binary  declares it is safe for use in processes with uid zero */
#define MH_SETUID_SAFE              0x80000     /* When this bit is set, the binary declares it is safe for use in processes when issetugid() is true */
#define MH_NO_REEXPORTED_DYLIBS     0x100000    /* When this bit is set on a dylib,  the static linker does not need to examine dependent dylibs to see if any are re-exported */
#define MH_PIE                      0x200000    /* When this bit is set, the OS will load the main executable at a random address.  Only used in MH_EXECUTE filetypes. */
#define MH_DEAD_STRIPPABLE_DYLIB    0x400000    /* Only for use on dylibs.  When linking against a dylib that has this bit set, the static linker will automatically not create a LC_LOAD_DYLIB load command to the dylib if no symbols are being referenced from the dylib. */
#define MH_HAS_TLV_DESCRIPTORS      0x800000    /* Contains a section of type S_THREAD_LOCAL_VARIABLES */
#define MH_NO_HEAP_EXECUTION        0x1000000   /* When this bit is set, the OS will run the main executable with a non-executable heap even on platforms (e.g. i386) that don't require it. Only used in MH_EXECUTE filetypes. */
#define MH_APP_EXTENSION_SAFE       0x2000000   /* The code was linked for use in an application extension. */


typedef struct load_command {
    uint32_t    cmd;        /* type of load command */
    uint32_t    cmdsize;    /* total size of command in bytes */
} load_command;

// load_command.cmd
#define LC_REQ_DYLD                 0x80000000
#define LC_SEGMENT                  0x1                     /* segment of this file to be mapped */
#define LC_SYMTAB                   0x2                     /* link-edit stab symbol table info */
#define LC_SYMSEG                   0x3                     /* link-edit gdb symbol table info (obsolete) */
#define LC_THREAD                   0x4                     /* thread */
#define LC_UNIXTHREAD               0x5                     /* unix thread (includes a stack) */
#define LC_LOADFVMLIB               0x6                     /* load a specified fixed VM shared library */
#define LC_IDFVMLIB                 0x7                     /* fixed VM shared library identification */
#define LC_IDENT                    0x8                     /* object identification info (obsolete) */
#define LC_FVMFILE                  0x9                     /* fixed VM file inclusion (internal use) */
#define LC_PREPAGE                  0xa                     /* prepage command (internal use) */
#define LC_DYSYMTAB                 0xb                     /* dynamic link-edit symbol table info */
#define LC_LOAD_DYLIB               0xc                     /* load a dynamically linked shared library */
#define LC_ID_DYLIB                 0xd                     /* dynamically linked shared lib ident */
#define LC_LOAD_DYLINKER            0xe                     /* load a dynamic linker */
#define LC_ID_DYLINKER              0xf                     /* dynamic linker identification */
#define LC_PREBOUND_DYLIB           0x10                    /* modules prebound for a dynamically linked shared library */
#define LC_ROUTINES                 0x11                    /* image routines */
#define LC_SUB_FRAMEWORK            0x12                    /* sub framework */
#define LC_SUB_UMBRELLA             0x13                    /* sub umbrella */
#define LC_SUB_CLIENT               0x14                    /* sub client */
#define LC_SUB_LIBRARY              0x15                    /* sub library */
#define LC_TWOLEVEL_HINTS           0x16                    /* two-level namespace lookup hints */
#define LC_PREBIND_CKSUM            0x17                    /* prebind checksum */
#define LC_LOAD_WEAK_DYLIB          (0x18 | LC_REQ_DYLD)    /* load a dynamically linked shared library that is allowed to be missing (all symbols are weak imported). */
#define LC_SEGMENT_64               0x19                    /* 64-bit segment of this file to be mapped */
#define LC_ROUTINES_64              0x1a                    /* 64-bit image routines */
#define LC_UUID                     0x1b                    /* the uuid */
#define LC_RPATH                    (0x1c | LC_REQ_DYLD)    /* runpath additions */
#define LC_CODE_SIGNATURE           0x1d                    /* local of code signature */
#define LC_SEGMENT_SPLIT_INFO       0x1e                    /* local of info to split segments */
#define LC_REEXPORT_DYLIB           (0x1f | LC_REQ_DYLD)    /* load and re-export dylib */
#define LC_LAZY_LOAD_DYLIB          0x20                    /* delay load of dylib until first use */
#define LC_ENCRYPTION_INFO          0x21                    /* encrypted segment information */
#define LC_DYLD_INFO                0x22                    /* compressed dyld information */
#define LC_DYLD_INFO_ONLY           (0x22 | LC_REQ_DYLD)    /* compressed dyld information only */
#define LC_LOAD_UPWARD_DYLIB        (0x23 | LC_REQ_DYLD)    /* load upward dylib */
#define LC_VERSION_MIN_MACOSX       0x24                    /* build for MacOSX min OS version */
#define LC_VERSION_MIN_IPHONEOS     0x25                    /* build for iPhoneOS min OS version */
#define LC_FUNCTION_STARTS          0x26                    /* compressed table of function start addresses */
#define LC_DYLD_ENVIRONMENT         0x27                    /* string for dyld to treat like environment variable */
#define LC_MAIN                     (0x28 | LC_REQ_DYLD)    /* replacement for LC_UNIXTHREAD */
#define LC_DATA_IN_CODE             0x29                    /* table of non-instructions in __text */
#define LC_SOURCE_VERSION           0x2A                    /* source version used to build binary */
#define LC_DYLIB_CODE_SIGN_DRS      0x2B                    /* Code signing DRs copied from linked dylibs */
#define LC_ENCRYPTION_INFO_64       0x2C                    /* 64-bit encrypted segment information */
#define LC_LINKER_OPTION            0x2D                    /* linker options in MH_OBJECT files */
#define LC_LINKER_OPTIMIZATION_HINT 0x2E                    /* optimization hints in MH_OBJECT files */
#define LC_VERSION_MIN_TVOS         0x2F                    /* build for AppleTV min OS version */
#define LC_VERSION_MIN_WATCHOS      0x30                    /* build for Watch min OS version */
#define LC_NOTE                     0x31                    /* arbitrary data included within a Mach-O file */
#define LC_BUILD_VERSION            0x32                    /* build for platform min OS version */

union lc_str {
    uint32_t    offset;   /* offset to the string */
};

struct segment_command {    /* for 32-bit architectures */
    uint32_t   cmd;         /* LC_SEGMENT */
    uint32_t   cmdsize;     /* includes sizeof section structs */
    char       segname[16]; /* segment name */
    uint32_t   vmaddr;      /* memory address of this segment */
    uint32_t   vmsize;      /* memory size of this segment */
    uint32_t   fileoff;     /* file offset of this segment */
    uint32_t   filesize;    /* amount to map from the file */
    vm_prot_t  maxprot;     /* maximum VM protection */
    vm_prot_t  initprot;    /* initial VM protection */
    uint32_t   nsects;      /* number of sections in segment */
    uint32_t   flags;       /* flags */
};

typedef struct segment_command_64 {  /* for 64-bit architectures */
    uint32_t    cmd;         /* LC_SEGMENT_64 */
    uint32_t    cmdsize;     /* includes sizeof section_64 structs */
    char        segname[16]; /* segment name */
    uint64_t    vmaddr;      /* memory address of this segment */
    uint64_t    vmsize;      /* memory size of this segment */
    uint64_t    fileoff;     /* file offset of this segment */
    uint64_t    filesize;    /* amount to map from the file */
    vm_prot_t   maxprot;     /* maximum VM protection */
    vm_prot_t   initprot;    /* initial VM protection */
    uint32_t    nsects;      /* number of sections in segment */
    uint32_t    flags;       /* flags */
} segment_command_64;

// segment_comment.flags:
#define SG_HIGHVM               0x1 /* the file contents for this segment is for the high part of the VM space, the low part is zero filled (for stacks in core files) */
#define SG_FVMLIB               0x2 /* this segment is the VM that is allocated by a fixed VM library, for overlap checking in the link editor */
#define SG_NORELOC              0x4 /* this segment has nothing that was relocated in it and nothing relocated to it, that is it maybe safely replaced without relocation */
#define SG_PROTECTED_VERSION_1  0x8 /* This segment is protected.  If the segment starts at file offset 0, the first page of the segment is not protected.  All other pages of the segment are protected. */

struct section {                /* for 32-bit architectures */
    char        sectname[16];   /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint32_t    addr;           /* memory address of this section */
    uint32_t    size;           /* size in bytes of this section */
    uint32_t    offset;         /* file offset of this section */
    uint32_t    align;          /* section alignment (power of 2) */
    uint32_t    reloff;         /* file offset of relocation entries */
    uint32_t    nreloc;         /* number of relocation entries */
    uint32_t    flags;          /* flags (section type and attributes)*/
    uint32_t    reserved1;      /* reserved (for offset or index) */
    uint32_t    reserved2;      /* reserved (for count or sizeof) */
};

typedef struct section_64 {             /* for 64-bit architectures */
    char        sectname[16];   /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint64_t    addr;           /* memory address of this section */
    uint64_t    size;           /* size in bytes of this section */
    uint32_t    offset;         /* file offset of this section */
    uint32_t    align;          /* section alignment (power of 2) */
    uint32_t    reloff;         /* file offset of relocation entries */
    uint32_t    nreloc;         /* number of relocation entries */
    uint32_t    flags;          /* flags (section type and attributes)*/
    uint32_t    reserved1;      /* reserved (for offset or index) */
    uint32_t    reserved2;      /* reserved (for count or sizeof) */
    uint32_t    reserved3;      /* reserved */
} section_64;

// section.flags masks
#define SECTION_TYPE        0x000000ff /* 256 section types */
#define SECTION_ATTRIBUTES  0xffffff00 /* 24 section attributes */

// section.flags.type:
#define S_REGULAR                               0x0  /* regular section */
#define S_ZEROFILL                              0x1  /* zero fill on demand section */
#define S_CSTRING_LITERALS                      0x2  /* section with only literal C strings*/
#define S_4BYTE_LITERALS                        0x3  /* section with only 4 byte literals */
#define S_8BYTE_LITERALS                        0x4  /* section with only 8 byte literals */
#define S_LITERAL_POINTERS                      0x5  /* section with only pointers to literals */
#define S_NON_LAZY_SYMBOL_POINTERS              0x6  /* section with only non-lazy symbol pointers */
#define S_LAZY_SYMBOL_POINTERS                  0x7  /* section with only lazy symbol pointers */
#define S_SYMBOL_STUBS                          0x8  /* section with only symbol stubs, byte size of stub in the reserved2 field */
#define S_MOD_INIT_FUNC_POINTERS                0x9  /* section with only function pointers for initialization*/
#define S_MOD_TERM_FUNC_POINTERS                0xa  /* section with only function pointers for termination */
#define S_COALESCED                             0xb  /* section contains symbols that are to be coalesced */
#define S_GB_ZEROFILL                           0xc  /* zero fill on demand section (that can be larger than 4 gigabytes) */
#define S_INTERPOSING                           0xd  /* section with only pairs of function pointers for interposing */
#define S_16BYTE_LITERALS                       0xe  /* section with only 16 byte literals */
#define S_DTRACE_DOF                            0xf  /* section contains DTrace Object Format */
#define S_LAZY_DYLIB_SYMBOL_POINTERS            0x10 /* section with only lazy symbol pointers to lazy loaded dylibs */
#define S_THREAD_LOCAL_REGULAR                  0x11 /* template of initial values for TLVs */
#define S_THREAD_LOCAL_ZEROFILL                 0x12 /* template of initial values for TLVs */
#define S_THREAD_LOCAL_VARIABLES                0x13 /* TLV descriptors */
#define S_THREAD_LOCAL_VARIABLE_POINTERS        0x14 /* pointers to TLV descriptors */
#define S_THREAD_LOCAL_INIT_FUNCTION_POINTERS   0x15 /* functions to call to initialize TLV values */

// section.flags.attributes:
#define SECTION_ATTRIBUTES_USR      0xff000000 /* User setable attributes */
#define S_ATTR_PURE_INSTRUCTIONS    0x80000000 /* section contains only true machine instructions */
#define S_ATTR_NO_TOC               0x40000000 /* section contains coalesced symbols that are not to be in a ranlib table of contents */
#define S_ATTR_STRIP_STATIC_SYMS    0x20000000 /* ok to strip static symbols in this section in files with the MH_DYLDLINK flag */
#define S_ATTR_NO_DEAD_STRIP        0x10000000 /* no dead stripping */
#define S_ATTR_LIVE_SUPPORT         0x08000000 /* blocks are live if they reference live blocks */
#define S_ATTR_SELF_MODIFYING_CODE  0x04000000 /* Used with i386 code stubs written on by dyld */
#define S_ATTR_DEBUG                0x02000000 /* a debug section */
#define SECTION_ATTRIBUTES_SYS      0x00ffff00 /* system setable attributes */
#define S_ATTR_SOME_INSTRUCTIONS    0x00000400 /* section contains some machine instructions */
#define S_ATTR_EXT_RELOC            0x00000200 /* section has external relocation entries */
#define S_ATTR_LOC_RELOC            0x00000100 /* section has local relocation entries */

// Some segment/section names:
#define SEG_PAGEZERO        "__PAGEZERO"        /* the pagezero segment which has no protections and catches NULL references for MH_EXECUTE files */
#define SEG_TEXT            "__TEXT"            /* the tradition UNIX text segment */
#define SECT_TEXT           "__text"            /* the real text part of the text section no headers, and no padding */
#define SECT_FVMLIB_INIT0   "__fvmlib_init0"    /* the fvmlib initialization section */
#define SECT_FVMLIB_INIT1   "__fvmlib_init1"    /* the section following the fvmlib initialization section */
#define SEG_DATA            "__DATA"            /* the tradition UNIX data segment */
#define SECT_DATA           "__data"            /* the real initialized data section no padding, no bss overlap */
#define SECT_BSS            "__bss"             /* the real uninitialized data section no padding */
#define SECT_COMMON         "__common"          /* the section common symbols are allocated in by the link editor */
#define SEG_OBJC            "__OBJC"            /* objective-C runtime segment */
#define SECT_OBJC_SYMBOLS   "__symbol_table"    /* symbol table */
#define SECT_OBJC_MODULES   "__module_info"     /* module information */
#define SECT_OBJC_STRINGS   "__selector_strs"   /* string table */
#define SECT_OBJC_REFS      "__selector_refs"   /* string table */
#define SEG_ICON            "__ICON"            /* the icon segment */
#define SECT_ICON_HEADER    "__header"          /* the icon headers */
#define SECT_ICON_TIFF      "__tiff"            /* the icons in tiff format */
#define SEG_LINKEDIT        "__LINKEDIT"        /* the segment containing all structs created and maintained by the link editor.  Created with -seglinkedit option to ld(1) for MH_EXECUTE and FVMLIB file types only */
#define SEG_UNIXSTACK       "__UNIXSTACK"       /* the unix stack segment */
#define SEG_IMPORT          "__IMPORT"          /* the segment for the self (dyld) modifing code stubs that has read, write and execute permissions */

// Some load command structs:
struct dylib {
    union lc_str    name;                   /* library's path name */
    uint32_t        timestamp;              /* library's build time stamp */
    uint32_t        current_version;        /* library's current version number */
    uint32_t        compatibility_version;  /* library's compatibility vers number*/
};

struct dylib_command {
    uint32_t        cmd;        /* LC_ID_DYLIB, LC_LOAD_{,WEAK_}DYLIB, LC_REEXPORT_DYLIB */
    uint32_t        cmdsize;    /* includes pathname string */
    struct dylib    dylib;      /* the library identification */
};

struct sub_framework_command {
    uint32_t        cmd;        /* LC_SUB_FRAMEWORK */
    uint32_t        cmdsize;    /* includes umbrella string */
    union lc_str    umbrella;   /* the umbrella framework name */
};

struct sub_client_command {
    uint32_t        cmd;        /* LC_SUB_CLIENT */
    uint32_t        cmdsize;    /* includes client string */
    union lc_str    client;     /* the client name */
};

struct sub_umbrella_command {
    uint32_t        cmd;            /* LC_SUB_UMBRELLA */
    uint32_t        cmdsize;        /* includes sub_umbrella string */
    union lc_str    sub_umbrella;   /* the sub_umbrella framework name */
};

struct sub_library_command {
    uint32_t        cmd;            /* LC_SUB_LIBRARY */
    uint32_t        cmdsize;        /* includes sub_library string */
    union lc_str    sub_library;    /* the sub_library name */
};

struct prebound_dylib_command {
    uint32_t        cmd;            /* LC_PREBOUND_DYLIB */
    uint32_t        cmdsize;        /* includes strings */
    union lc_str    name;           /* library's path name */
    uint32_t        nmodules;       /* number of modules in library */
    union lc_str    linked_modules; /* bit vector of linked modules */
};

struct dylinker_command {
    uint32_t        cmd;        /* LC_ID_DYLINKER, LC_LOAD_DYLINKER or LC_DYLD_ENVIRONMENT */
    uint32_t        cmdsize;    /* includes pathname string */
    union lc_str    name;       /* dynamic linker's path name */
};

struct routines_command {       /* for 32-bit architectures */
    uint32_t    cmd;            /* LC_ROUTINES */
    uint32_t    cmdsize;        /* total size of this command */
    uint32_t    init_address;   /* address of initialization routine */
    uint32_t    init_module;    /* index into the module table that the init routine is defined in */
    uint32_t    reserved1;
    uint32_t    reserved2;
    uint32_t    reserved3;
    uint32_t    reserved4;
    uint32_t    reserved5;
    uint32_t    reserved6;
};

struct routines_command_64 {    /* for 64-bit architectures */
    uint32_t    cmd;            /* LC_ROUTINES_64 */
    uint32_t    cmdsize;        /* total size of this command */
    uint64_t    init_address;   /* address of initialization routine */
    uint64_t    init_module;    /* index into the module table that the init routine is defined in */
    uint64_t    reserved1;
    uint64_t    reserved2;
    uint64_t    reserved3;
    uint64_t    reserved4;
    uint64_t    reserved5;
    uint64_t    reserved6;
};

struct symtab_command {
    uint32_t    cmd;        /* LC_SYMTAB */
    uint32_t    cmdsize;    /* sizeof(struct symtab_command) */
    uint32_t    symoff;     /* symbol table offset */
    uint32_t    nsyms;      /* number of symbol table entries */
    uint32_t    stroff;     /* string table offset */
    uint32_t    strsize;    /* string table size in bytes */
};

struct dysymtab_command {
    uint32_t    cmd;            /* LC_DYSYMTAB */
    uint32_t    cmdsize;        /* sizeof(struct dysymtab_command) */
    uint32_t    ilocalsym;      /* index to local symbols */
    uint32_t    nlocalsym;      /* number of local symbols */
    uint32_t    iextdefsym;     /* index to externally defined symbols */
    uint32_t    nextdefsym;     /* number of externally defined symbols */
    uint32_t    iundefsym;      /* index to undefined symbols */
    uint32_t    nundefsym;      /* number of undefined symbols */
    uint32_t    tocoff;         /* file offset to table of contents */
    uint32_t    ntoc;           /* number of entries in table of contents */
    uint32_t    modtaboff;      /* file offset to module table */
    uint32_t    nmodtab;        /* number of module table entries */
    uint32_t    extrefsymoff;   /* offset to referenced symbol table */
    uint32_t    nextrefsyms;    /* number of referenced symbol table entries */
    uint32_t    indirectsymoff; /* file offset to the indirect symbol table */
    uint32_t    nindirectsyms;  /* number of indirect symbol table entries */
    uint32_t    extreloff;      /* offset to external relocation entries */
    uint32_t    nextrel;        /* number of external relocation entries */
    uint32_t    locreloff;      /* offset to local relocation entries */
    uint32_t    nlocrel;        /* number of local relocation entries */
};

#define INDIRECT_SYMBOL_LOCAL 0x80000000
#define INDIRECT_SYMBOL_ABS  0x40000000


struct prebind_cksum_command {
    uint32_t    cmd;        /* LC_PREBIND_CKSUM */
    uint32_t    cmdsize;    /* sizeof(struct prebind_cksum_command) */
    uint32_t    cksum;      /* the check sum or zero */
};

struct uuid_command {
    uint32_t    cmd;        /* LC_UUID */
    uint32_t    cmdsize;    /* sizeof(struct uuid_command) */
    uint8_t     uuid[16];   /* the 128-bit uuid */
};

struct rpath_command {
    uint32_t        cmd;        /* LC_RPATH */
    uint32_t        cmdsize;    /* includes string */
    union lc_str    path;       /* path to add to run path */
};

struct linkedit_data_command {
    uint32_t    cmd;        /* LC_CODE_SIGNATURE, LC_SEGMENT_SPLIT_INFO, LC_FUNCTION_STARTS, LC_DATA_IN_CODE, LC_DYLIB_CODE_SIGN_DRS or LC_LINKER_OPTIMIZATION_HINT. */
    uint32_t    cmdsize;    /* sizeof(struct linkedit_data_command) */
    uint32_t    dataoff;    /* file offset of data in __LINKEDIT segment */
    uint32_t    datasize;   /* file size of data in __LINKEDIT segment  */
};

struct encryption_info_command {
    uint32_t    cmd;        /* LC_ENCRYPTION_INFO */
    uint32_t    cmdsize;    /* sizeof(struct encryption_info_command) */
    uint32_t    cryptoff;   /* file offset of encrypted range */
    uint32_t    cryptsize;  /* file size of encrypted range */
    uint32_t    cryptid;    /* which encryption system, 0 means not-encrypted yet */
};

struct encryption_info_command_64 {
    uint32_t    cmd;        /* LC_ENCRYPTION_INFO_64 */
    uint32_t    cmdsize;    /* sizeof(struct encryption_info_command_64) */
    uint32_t    cryptoff;   /* file offset of encrypted range */
    uint32_t    cryptsize;  /* file size of encrypted range */
    uint32_t    cryptid;    /* which encryption system, 0 means not-encrypted yet */
    uint32_t    pad;        /* padding to make this struct's size a multiple of 8 bytes */
};

struct version_min_command {
    uint32_t    cmd;        /* LC_VERSION_MIN_MACOSX or LC_VERSION_MIN_IPHONEOS or LC_VERSION_MIN_WATCHOS or LC_VERSION_MIN_TVOS */
    uint32_t    cmdsize;    /* sizeof(struct min_version_command) */
    uint32_t    version;    /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    uint32_t    sdk;        /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
};

struct build_version_command {
    uint32_t    cmd;        /* LC_BUILD_VERSION */
    uint32_t    cmdsize;    /* sizeof(struct build_version_command) plus ntools * sizeof(struct build_tool_version) */
    uint32_t    platform;   /* platform */
    uint32_t    minos;      /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    uint32_t    sdk;        /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    uint32_t    ntools;     /* number of tool entries following this */
};

struct build_tool_version {
    uint32_t    tool;       /* enum for the tool */
    uint32_t    version;    /* version number of the tool */
};

// build_version_command.platform:
#define PLATFORM_MACOS 1
#define PLATFORM_IOS 2
#define PLATFORM_TVOS 3
#define PLATFORM_WATCHOS 4

// build_tool_version.tool:
#define TOOL_CLANG 1
#define TOOL_SWIFT 2
#define TOOL_LD 3

struct dyld_info_command {
    uint32_t    cmd;            /* LC_DYLD_INFO or LC_DYLD_INFO_ONLY */
    uint32_t    cmdsize;        /* sizeof(struct dyld_info_command) */
    uint32_t    rebase_off;     /* file offset to rebase info  */
    uint32_t    rebase_size;    /* size of rebase info   */
    uint32_t    bind_off;       /* file offset to binding info   */
    uint32_t    bind_size;      /* size of binding info  */
    uint32_t    weak_bind_off;  /* file offset to weak binding info   */
    uint32_t    weak_bind_size; /* size of weak binding info  */
    uint32_t    lazy_bind_off;  /* file offset to lazy binding info */
    uint32_t    lazy_bind_size; /* size of lazy binding infs */
    uint32_t    export_off;     /* file offset to lazy binding info */
    uint32_t    export_size;    /* size of lazy binding infs */
};

// Rebase definitions:
#define REBASE_TYPE_POINTER                                 1
#define REBASE_TYPE_TEXT_ABSOLUTE32                         2
#define REBASE_TYPE_TEXT_PCREL32                            3
#define REBASE_OPCODE_MASK                                  0xF0
#define REBASE_IMMEDIATE_MASK                               0x0F
#define REBASE_OPCODE_DONE                                  0x00
#define REBASE_OPCODE_SET_TYPE_IMM                          0x10
#define REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB           0x20
#define REBASE_OPCODE_ADD_ADDR_ULEB                         0x30
#define REBASE_OPCODE_ADD_ADDR_IMM_SCALED                   0x40
#define REBASE_OPCODE_DO_REBASE_IMM_TIMES                   0x50
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES                  0x60
#define REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB               0x70
#define REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB    0x80

// Bind definitions:
#define BIND_TYPE_POINTER                               1
#define BIND_TYPE_TEXT_ABSOLUTE32                       2
#define BIND_TYPE_TEXT_PCREL32                          3
#define BIND_SPECIAL_DYLIB_SELF                         0
#define BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE              -1
#define BIND_SPECIAL_DYLIB_FLAT_LOOKUP                  -2
#define BIND_SYMBOL_FLAGS_WEAK_IMPORT                   0x1
#define BIND_SYMBOL_FLAGS_NON_WEAK_DEFINITION           0x8
#define BIND_OPCODE_MASK                                0xF0
#define BIND_IMMEDIATE_MASK                             0x0F
#define BIND_OPCODE_DONE                                0x00
#define BIND_OPCODE_SET_DYLIB_ORDINAL_IMM               0x10
#define BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB              0x20
#define BIND_OPCODE_SET_DYLIB_SPECIAL_IMM               0x30
#define BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM       0x40
#define BIND_OPCODE_SET_TYPE_IMM                        0x50
#define BIND_OPCODE_SET_ADDEND_SLEB                     0x60
#define BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB         0x70
#define BIND_OPCODE_ADD_ADDR_ULEB                       0x80
#define BIND_OPCODE_DO_BIND                             0x90
#define BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB               0xA0
#define BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED         0xB0
#define BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB    0xC0


// Export flag values
#define EXPORT_SYMBOL_FLAGS_KIND_MASK           0x03
#define EXPORT_SYMBOL_FLAGS_KIND_REGULAR        0x00
#define EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL   0x01
#define EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION     0x04
#define EXPORT_SYMBOL_FLAGS_REEXPORT            0x08
#define EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER   0x10

struct linker_option_command {
    uint32_t    cmd;        /* LC_LINKER_OPTION only used in MH_OBJECT filetypes */
    uint32_t    cmdsize;
    uint32_t    count;      /* number of strings */
};

struct symseg_command {
    uint32_t    cmd;        /* LC_SYMSEG */
    uint32_t    cmdsize;    /* sizeof(struct symseg_command) */
    uint32_t    offset;     /* symbol segment offset */
    uint32_t    size;       /* symbol segment size in bytes */
};

struct ident_command {
    uint32_t    cmd;        /* LC_IDENT */
    uint32_t    cmdsize;    /* strings that follow this command */
};

struct fvmfile_command {
    uint32_t        cmd;            /* LC_FVMFILE */
    uint32_t        cmdsize;        /* includes pathname string */
    union lc_str    name;           /* files pathname */
    uint32_t        header_addr;    /* files virtual address */
};

struct entry_point_command {
    uint32_t    cmd;        /* LC_MAIN only used in MH_EXECUTE filetypes */
    uint32_t    cmdsize;    /* 24 */
    uint64_t    entryoff;   /* file (__TEXT) offset of main() */
    uint64_t    stacksize;  /* if not zero, initial stack size */
};

struct source_version_command {
    uint32_t    cmd;        /* LC_SOURCE_VERSION */
    uint32_t    cmdsize;    /* 16 */
    uint64_t    version;    /* A.B.C.D.E packed as a24.b10.c10.d10.e10 */
};

struct data_in_code_entry {
    uint32_t    offset; /* from mach_header to start of data range*/
    uint16_t    length; /* number of bytes in data range */
    uint16_t    kind;   /* a DICE_KIND_* value  */
};

// data_in_code_entry.kind
#define DICE_KIND_DATA              0x0001
#define DICE_KIND_JUMP_TABLE8       0x0002
#define DICE_KIND_JUMP_TABLE16      0x0003
#define DICE_KIND_JUMP_TABLE32      0x0004
#define DICE_KIND_ABS_JUMP_TABLE32  0x0005

struct note_command {
    uint32_t    cmd;            /* LC_NOTE */
    uint32_t    cmdsize;        /* sizeof(struct note_command) */
    char        data_owner[16]; /* owner name for this LC_NOTE */
    uint64_t    offset;         /* file offset of this data */
    uint64_t    size;           /* length of data region */
};



// API Functions:
load_command* libmacho_first_load_command(mach_header_64* header);
load_command* libmacho_next_load_command(load_command* cmd);

#endif
