#ifndef _KERN_FUNCS_H_
#define _KERN_FUNCS_H_
#include <offsetcache.h>

#define SETOFFSET(offset, val) set_offset(#offset, val)
#define GETOFFSET(offset) get_offset(#offset)

extern uint64_t kernel_base;
extern uint64_t kernel_slide;

void set_tfp0(mach_port_t port);
void wk32(uint64_t kaddr, uint32_t val);
void wk64(uint64_t kaddr, uint64_t val);
uint32_t rk32(uint64_t kaddr);
uint64_t rk64(uint64_t kaddr);
uint64_t kmem_alloc(uint64_t size);
size_t kread(uint64_t where, void *p, size_t size);
size_t kwrite(uint64_t where, const void *p, size_t size);
uint64_t task_self_addr(void);
extern int (*pmap_load_trust_cache)(uint64_t kernel_trust, size_t length);
int _pmap_load_trust_cache(uint64_t kernel_trust, size_t length);

#endif // _KERN_FUNCS_H_
