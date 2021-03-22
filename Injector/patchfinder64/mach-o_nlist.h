#ifndef _MACHO_NLIST_H_
#define _MACHO_NLIST_H_

#include <stdint.h>

struct nlist {
    union {
#ifndef __LP64__
        char *n_name;   /* for use when in-core */
#endif
        uint32_t n_strx;    /* index into the string table */
    } n_un;
    uint8_t n_type;     /* type flag, see below */
    uint8_t n_sect;     /* section number or NO_SECT */
    int16_t n_desc;     /* see <mach-o/stab.h> */
    uint32_t n_value;   /* value of this symbol (or stab offset) */
};

struct nlist_64 {
    union {
        uint32_t  n_strx; /* index into the string table */
    } n_un;
    uint8_t n_type;        /* type flag, see below */
    uint8_t n_sect;        /* section number or NO_SECT */
    uint16_t n_desc;       /* see <mach-o/stab.h> */
    uint64_t n_value;      /* value of this symbol (or stab offset) */
};

#define N_STAB  0xe0  /* if any of these bits set, a symbolic debugging entry */
#define N_PEXT  0x10  /* private external symbol bit */
#define N_TYPE  0x0e  /* mask for the type bits */
#define N_EXT   0x01  /* external symbol bit, set for external symbols */

#define N_UNDF  0x0     /* undefined, n_sect == NO_SECT */
#define N_ABS   0x2     /* absolute, n_sect == NO_SECT */
#define N_SECT  0xe     /* defined in section number n_sect */
#define N_PBUD  0xc     /* prebound undefined (defined in a dylib) */
#define N_INDR  0xa     /* indirect */

#endif
