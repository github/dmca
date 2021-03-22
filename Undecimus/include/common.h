#ifndef COMMON_H
#define COMMON_H

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#define RAWLOG(str, args...) do { \
    init_function(); \
    NSLog(@str, ##args); \
} while(false)
#define ADDRSTRING(val) [NSString stringWithFormat:@ADDR, val]
#else
#include <CoreFoundation/CoreFoundation.h>
#ifdef __cplusplus
extern "C" {
#endif
void NSLog(CFStringRef, ...);
#ifdef __cplusplus
}
#endif
#define RAWLOG(str, args...) do { \
    init_function(); \
    NSLog(CFSTR(str), ##args); \
} while(false)
#define BOOL bool
#define YES ((BOOL) true)
#define NO ((BOOL) false)
#endif
#include <mach-o/loader.h>
#include <mach/error.h>
#include <sys/sysctl.h>
#include <sys/syscall.h>
#include <codesign.h>
#if __has_include(<sys/ptrace.h>)
#include <sys/ptrace.h>
#else
#define PT_DENY_ATTACH 31
#endif
#define LOG(str, args...) RAWLOG("[*] " str, ##args)

#define error_log LOG
#define info_log LOG
#define debug_log LOG

#define SafeFree(x) do { if (x) free(x); } while(false)
#define SafeFreeNULL(x) do { SafeFree(x); (x) = NULL; } while(false)
#define CFSafeRelease(x) do { if (x) CFRelease(x); } while(false)
#define CFSafeReleaseNULL(x) do { CFSafeRelease(x); (x) = NULL; } while(false)
#define SafeSFree(x) do { if (KERN_POINTER_VALID(x)) sfree(x); } while(false)
#define SafeSFreeNULL(x) do { SafeSFree(x); (x) = KPTR_NULL; } while(false)
#define SafeIOFree(x, size) do { if (KERN_POINTER_VALID(x)) IOFree(x, size); } while(false)
#define SafeIOFreeNULL(x, size) do { SafeIOFree(x, size); (x) = KPTR_NULL; } while(false)
#define SafeKFree(x, size) do { if (KERN_POINTER_VALID(x)) kfree(x, size); } while(false)
#define SafeKFreeNULL(x, size) do { SafeKFree(x, size); (x) = KPTR_NULL; } while(false)
#define SafeKernelFree(x, size) do { if (KERN_POINTER_VALID(x)) kmem_free(x, size); } while(false)
#define SafeKernelFreeNULL(x, size) do { SafeKernelFree(x, size); (x) = KPTR_NULL; } while(false)
#define OSSafeRelease(x) do { if (KERN_POINTER_VALID(x)) OSObject_Release(x); } while(false)
#define OSSafeReleaseNULL(x) do { OSSafeRelease(x); (x) = KPTR_NULL; } while(false)

__attribute__((always_inline))
__unused static void init_function(void) {
    return;
}

#define ARRAY(type, ...) ((type *)&(const type[]){__VA_ARGS__})

#define kCFCoreFoundationVersionNumber_iOS_13_0 1656.00
#define kCFCoreFoundationVersionNumber_iOS_12_4 1575.17
#define kCFCoreFoundationVersionNumber_iOS_12_2 1570.15
#define kCFCoreFoundationVersionNumber_iOS_12_0 1535.12
#define kCFCoreFoundationVersionNumber_iOS_11_3 1452.23
#define kCFCoreFoundationVersionNumber_iOS_11_1 1445.32
#define kCFCoreFoundationVersionNumber_iOS_11_0 1443.00

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define ADDR "0x%016llx"
#define MACH_HEADER_MAGIC MH_MAGIC_64
#define MACH_LC_SEGMENT LC_SEGMENT_64
typedef struct mach_header_64 mach_hdr_t;
typedef struct segment_command_64 mach_seg_t;
typedef struct load_command mach_lc_t;
typedef uint64_t kptr_t;
#define KPTR_NULL ((kptr_t) 0)
#define VM_MIN_KERNEL_ADDRESS 0xffffffe000000000ULL
#define VM_MAX_KERNEL_ADDRESS 0xfffffff3ffffffffULL
#define KERN_POINTER_VALID(val) (((val) & 0xffffffff) != 0xdeadbeef && (val) >= VM_MIN_KERNEL_ADDRESS && (val) <= VM_MAX_KERNEL_ADDRESS)
#define KERNELCACHE "/System/Library/Caches/com.apple.kernelcaches/kernelcache"

#define MAX_KASLR_SLIDE 0x21000000
#define STATIC_KERNEL_BASE_ADDRESS 0xfffffff007004000

#define message_size_for_kalloc_size(kalloc_size) (((3 * kalloc_size) / 4) - 0x74)

extern kptr_t offset_options;
#define OPT(x) (KERN_POINTER_VALID(offset_options) ? ((ReadKernel64(offset_options) & OPT_ ##x) ? true : false) : false)
#define SETOPT(x) (KERN_POINTER_VALID(offset_options) ? WriteKernel64(offset_options, ReadKernel64(offset_options) | OPT_ ##x) : 0)
#define UNSETOPT(x) (KERN_POINTER_VALID(offset_options) ? WriteKernel64(offset_options, ReadKernel64(offset_options) & ~OPT_ ##x) : 0)
#define OPT_GET_TASK_ALLOW (1<<0)
#define OPT_CS_DEBUGGED (1<<1)
#define OPT_SKIP_ENTITLEMENTS (1<<2)
#define OPT_SKIP_SANDBOX (1<<3)
#define OPT_SKIP_JETSAM_ENTITLEMENT (1<<4)
#define OPT_SKIP_SUBSTRATE (1<<5)
#define OPT_SET_CHILDDRAINED (1<<6)
#define OPT_SET_HAS_CORPSE_FOOTPRINT (1<<7)
#define OPT_SET_PLATFORM_BINARY (1<<8)

extern kptr_t offset_features;
#define FEAT(x) (KERN_POINTER_VALID(offset_features) ? ((ReadKernel64(offset_features) & FEAT_ ##x) ? true : false) : false)
#define SETFEAT(x) (KERN_POINTER_VALID(offset_features) ? WriteKernel64(offset_features, ReadKernel64(offset_features) | FEAT_ ##x) : 0)
#define UNSETFEAT(x) (KERN_POINTER_VALID(offset_features) ? WriteKernel64(offset_features, ReadKernel64(offset_features) & ~FEAT_ ##x) : 0)
#define FEAT_KERNEL_EXECUTION (1<<0)
#define FEAT_USERSPACE_REBOOT (1<<1)

#endif

