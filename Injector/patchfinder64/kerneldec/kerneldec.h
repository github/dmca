#ifndef _KERNELDEC_H
#define _KERNELDEC_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
#endif
int decompress_kernel(FILE *inputfh, FILE* outputfh, FILE *kppfh, bool quiet);

#endif
