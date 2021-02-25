#include <kernel.h>

typedef int quad __attribute__((mode(TI)));
typedef int int128 __attribute__((mode(TI)));
typedef unsigned int uquad __attribute__((mode(TI)));
typedef unsigned int uint128 __attribute__((mode(TI)));

#define MAKE128(RES,MSB,LSB) \
	__asm__ ( "pcpyld %0, %1, %2" : "=r" (RES) : "r" ((uint64)MSB), "r" ((uint64)LSB))
#define UINT64(LOW,HIGH) (((uint64)HIGH)<<32 | ((uint64)LOW))
#define MAKEQ(RES,W0,W1,W2,W3) MAKE128(RES,UINT64(W2,W3),UINT64(W0,W1))

#define BIT64(v,s) (((uint64)(v)) << (s))

#include "mem.h"
#include "gs.h"

extern uint128 packetbuf[128];

void waitDMA(volatile uint32 *chcr);
void toGIF(void *src, int n);

void drawcube(void);
