//
//  jailbreak.c
//  Undecimus
//
//  Created by Pwn20wnd on 5/11/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#include "jailbreak.h"
#define FAIL_LABEL
#import "JailbreakViewController.h"
#undef FAIL_LABEL
#include <sys/snapshot.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <copyfile.h>
#include <spawn.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sysexits.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/sysctl.h>
#include <os/log.h>
#include <mach-o/dyld.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/syscall.h>
#include <common.h>
#include <iokit.h>
#include <NSTask.h>
#include <MobileGestalt.h>
#include <netdb.h>
#include <reboot.h>
#import <snappy.h>
#import <inject.h>
#include <sched.h>
#import <patchfinder64.h>
#import <offsetcache.h>
#import <kerneldec.h>
#include <mach-o/dyld_images.h>
#include <pwd.h>
#include "KernelOffsets.h"
#include "KernelMemory.h"
#include "KernelExecution.h"
#include "KernelUtilities.h"
#include "remote_memory.h"
#include "remote_call.h"
#include "async_wake.h"
#include "utils.h"
#include "ArchiveFile.h"
#include "FakeApt.h"
#include "voucher_swap.h"
#include "kernel_memory.h"
#include "kernel_slide.h"
#include "find_port.h"
#include "machswap_offsets.h"
#include "machswap_pwn.h"
#include "machswap2_pwn.h"
#include "prefs.h"
#include "exploits.h"
#include "substitutors.h"
#include "sock_puppet.h"
#include <ubc_headers.h>
#include <cs_blobs.h>
#include <ubc_headers.h>
#include <libproc.h>
#include "amfidServer.c"
#include "amfid_types.h"
#include <Security/Security.h>
#include <CSCommon.h>
#include "sock_port.h"
#include "cs_dingling.h"
#include <sbops.h>
#include "tw_exploit.h"
#if __has_include("pac_userspace.h")
#include "pac_userspace.h"
#else
void exp_start(void);
extern mach_port_t tfp0_port;
extern uint64_t kaslr;
#endif
#define MNTK_UNMOUNT_PREFLIGHT 0x00020000 /* mounted file system wants preflight check during unmount */
#ifdef os_release
#undef os_release
#endif

int stage = __COUNTER__;
extern int maxStage;

#define _update_stage() do { \
    dispatch_async(dispatch_get_main_queue(), ^{ \
        init_function(); \
        [UIView performWithoutAnimation:^{ \
            init_function(); \
            [[[JailbreakViewController sharedController] jailbreakProgressBar] setProgress:(float)((float) stage/ (float) maxStage) animated:YES]; \
            [[[JailbreakViewController sharedController] jailbreakProgressBar] setProgress:(float)((float) stage/ (float) maxStage) animated:YES]; \
            [[JailbreakViewController sharedController] exploitProgressLabel].text = [NSString stringWithFormat:@"%d/%d", stage, maxStage]; \
        }]; \
    }); \
} while (false)

#define _upstage() do { \
    __COUNTER__; \
    stage++; \
    _update_stage(); \
} while (false)

typedef boolean_t (*dispatch_mig_callback_t)(mach_msg_header_t *message, mach_msg_header_t *reply);
mach_msg_return_t dispatch_mig_server(dispatch_source_t ds, size_t maxmsgsz, dispatch_mig_callback_t callback);
kern_return_t bootstrap_check_in(mach_port_t bootstrap_port, const char *service, mach_port_t *server_port);
void xpc_transaction_begin(void);
void xpc_transaction_end(void);

kern_return_t verify_code_directory(mach_port_t amfid_port, amfid_path_t path, uint64_t file_offset, int32_t a4, int32_t a5, int32_t a6, int32_t *entitlements_valid, int32_t *signature_valid, int32_t *unrestrict, int32_t *signer_type, int32_t *is_apple, int32_t *is_developer_code, amfid_a13_t a13, amfid_cdhash_t cdhash, audit_token_t audit) {
	audit_token_t kernel_token = KERNEL_AUDIT_TOKEN_VALUE;
	if (memcmp(&audit, &kernel_token, sizeof(audit)) != 0) {
		return KERN_FAILURE;
	}
	kern_return_t ret = KERN_FAILURE;
	img_info_t info;
	info.name = path;
	info.file_off = file_offset;
	if (open_img(&info) == 0) {
		uint32_t cs_size = 0;
		const void *csblob = find_code_signature(&info, &cs_size);
		if (csblob != NULL) {
			const CS_CodeDirectory *chosen_cd = NULL;
			uint32_t csb_offset = 0;
			const CS_GenericBlob *entitlements = NULL;
			uint32_t entitlements_offset = 0;
			if (find_best_codedir(csblob, cs_size, &chosen_cd, &csb_offset, &entitlements, &entitlements_offset) == 0) {
				if (hash_code_directory(chosen_cd, cdhash) == 0) {
					*entitlements_valid = true;
					*signature_valid = true;
					*signer_type = false;
					*is_apple = false;
					*is_developer_code = false;
					if (strncmp(path, "/private/var/containers/Bundle/Application", strlen("/private/var/containers/Bundle/Application")) == 0) {
						*unrestrict = false;
					} else {
						*unrestrict = true;
					}
					ret = KERN_SUCCESS;
				}
			}
		}
		close_img(&info);
	}
    return ret;
}

kern_return_t permit_unrestricted_debugging(mach_port_t amfid_port, int32_t *unrestricted_debugging, audit_token_t audit) {
    return KERN_FAILURE;
}

#define host_set_special_port(host_priv, which, port) kernel_host_set_special_port(host_priv, which, port)

kern_return_t kernel_host_set_special_port(host_priv_t host_priv, int which, mach_port_t port) {
	kptr_t proc_kptr = proc_struct_addr();
	kptr_t host_priv_kptr = get_address_of_port(proc_kptr, host_priv);
	kptr_t port_kptr = get_address_of_port(proc_kptr, port);
	kptr_t realhost_kptr = ReadKernel64(host_priv_kptr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
	kptr_t slot_kptr = realhost_kptr + koffset(KSTRUCT_OFFSET_HOST_SPECIAL) + which * sizeof(kptr_t);
	WriteKernel64(slot_kptr, port_kptr);
	return KERN_SUCCESS;
}

NSString *cdhashFor(NSString *file);

void jailbreak()
{
    init_function();
    _status(localize(@"Jailbreaking"), false, false);
    
    __block int __assertion_failure = false;
#define __block_assertion_handler() do { \
    if (false) { \
    fail:; \
        __assertion_failure = true; \
        return; \
    } \
} while(false)
#define __check_assertion_failure() do { \
    if (__assertion_failure) goto fail; \
} while(false)
    int rv = 0;
    bool usedPersistedKernelTaskPort = NO;
    pid_t my_pid = getpid();
    uid_t my_uid = getuid();
    host_t myHost = HOST_NULL;
    host_t myOriginalHost = HOST_NULL;
    kptr_t myProcAddr = KPTR_NULL;
	kptr_t myCredAddr = KPTR_NULL;
	kptr_t myLabelAddr = KPTR_NULL;
	kptr_t kernelCredAddr = KPTR_NULL;
	kptr_t kernelLabelAddr = KPTR_NULL;
    prefs_t *prefs = copy_prefs();
	if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
		prefs->ssh_only = false;
		prefs->read_only_rootfs = false;
	}
    bool needStrap = NO;
    bool needSubstitutor = NO;
    bool skipSubstitutor = NO;
    NSString *homeDirectory = NSHomeDirectory();
    NSString *temporaryDirectory = NSTemporaryDirectory();
    NSMutableArray *debsToInstall = [NSMutableArray new];
    NSMutableString *status = [NSMutableString new];
    time_t start_time = time(NULL);
    JailbreakViewController *sharedController = [JailbreakViewController sharedController];
    NSMutableArray *resources = [NSMutableArray new];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    bool doInject = false;
    const char *success_file = [temporaryDirectory stringByAppendingPathComponent:@"jailbreak.completed"].UTF8String;
    NSString *NSJailbreakDirectory = @"/jb";
    const char *jailbreakDirectory = NSJailbreakDirectory.UTF8String;
    struct passwd *root_pw = getpwnam("root");
    struct passwd *mobile_pw = getpwnam("mobile");
    code_substitutor_t *substitutor = get_substitutor_by_name(prefs->code_substitutor);
    __block mach_port_t real_amfid_port = MACH_PORT_NULL;
    __block mach_port_t fake_amfid_port = MACH_PORT_NULL;
    dispatch_queue_t amfid_queue = NULL;
    uint8_t *real_cdhash = NULL;
    uint8_t *fake_cdhash = NULL;
    pid_t amfidPid = 0;
    kptr_t myCSBlobsAddr = KPTR_NULL;
	bool shouldRebootUserspace = false;
    _assert(my_uid == mobile_pw->pw_uid, localize(@"Unable to verify my user id."));
#define _mem_assert(ptr) do { \
    _assert(ptr != (__typeof__(ptr))NULL, localize(@"Memory allocation failure: %s", #ptr)); \
} while(false)
#define _NSJailbreakFile(x) ({ \
    NSString *path = [NSJailbreakDirectory stringByAppendingPathComponent:x]; \
    if (prefs->read_only_rootfs) { \
        NSString *tryPath = [path stringByAppendingString:@".ro"]; \
        if ([[NSFileManager defaultManager] fileExistsAtPath:tryPath]) { \
            path = tryPath; \
        } \
    } \
    path; \
})
#define _jailbreak_file(x) (_NSJailbreakFile(@(x)).UTF8String)
    _assert(clean_file(success_file), localize(@"Unable to clean success file."));
#define _insert_status(x) do { [status appendString:x]; } while (false)
#define _sync_prefs() do { _assert(set_prefs(prefs), localize(@"Unable to synchronize app preferences. Please restart the app and try again.")); } while (false)
#define _write_test_file(file) do { \
    _assert(create_file(file, root_pw->pw_uid, 0644), localize(@"Unable to create test file.")); \
    _assert(clean_file(file), localize(@"Unable to clean test file.")); \
} while (false)
#define _kerncred(expr) ({ \
	__block __typeof__(expr) ret; \
	_assert(execute_with_credentials(myProcAddr, kernelCredAddr, ^{ \
		ret = (expr); \
	}), localize(@"Unable to execute with kernel credentials.")); \
	ret; \
})
    
    _upstage();
    
    {
        // Exploit kernel.
        
        _progress(localize(@"Exploiting kernel..."));
        bool exploit_success = NO;
        myHost = mach_host_self();
        _assert(MACH_PORT_VALID(myHost), localize(@"Unable to get host port."));
        myOriginalHost = myHost;
        if (restore_kernel_task_port(&tfp0) &&
            restore_kernel_base(&kernel_base, &kernel_slide) &&
            restore_kernel_offset_cache()) {
            usedPersistedKernelTaskPort = YES;
            exploit_success = YES;
        } else if (strcmp(prefs->kernel_exploit, K_ASYNC_WAKE) == 0) {
            if (async_wake_go() &&
                MACH_PORT_VALID(tfp0)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_VOUCHER_SWAP) == 0) {
            voucher_swap();
            if (MACH_PORT_VALID(tfp0)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_SOCKPUPPET) == 0) {
            if (sock_puppet() &&
                MACH_PORT_VALID(tfp0)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_TIME_WASTE) == 0) {
            if (time_waste() && MACH_PORT_VALID(tfp0)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_SOCKPORT) == 0) {
            if (sock_port() &&
                MACH_PORT_VALID(tfp0)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_MACH_SWAP) == 0) {
            machswap_offsets_t *machswap_offsets = get_machswap_offsets();
            if (machswap_offsets != NULL &&
                machswap_exploit(machswap_offsets) == ERR_SUCCESS &&
                MACH_PORT_VALID(tfp0) &&
                KERN_POINTER_VALID(kernel_base)) {
                exploit_success = YES;
            }
        } else if (strcmp(prefs->kernel_exploit, K_MACH_SWAP_2) == 0) {
            machswap_offsets_t *machswap_offsets = get_machswap_offsets();
            if (machswap_offsets != NULL &&
                machswap2_exploit(machswap_offsets) == ERR_SUCCESS &&
                MACH_PORT_VALID(tfp0) &&
                KERN_POINTER_VALID(kernel_base)) {
                exploit_success = YES;
            }
        }
#if __arm64e__
        else if (strcmp(prefs->kernel_exploit, K_ZECOPS_PAC) == 0) {
            exp_start();
            if (MACH_PORT_VALID(tfp0) &&
                KERN_POINTER_VALID(kernel_base)) {
                exploit_success = true;
            }
        }
#endif
        else {
            _notice(localize(@"No exploit selected."), false, false);
            _status(localize(@"Jailbreak"), true, true);
            return;
        }
        LOG("tfp0: 0x%x", tfp0);
        if (exploit_success && !verify_tfp0()) {
            LOG("Unable to verify TFP0.");
            exploit_success = NO;
        }
        if (!exploit_success) {
            _notice(localize(@"Unable to exploit kernel. This is not an error. Reboot and try again."), true, false);
            exit(EXIT_FAILURE);
            _assert(false, localize(@"Unable to exit."));
        }
        _insert_status(localize(@"Exploited kernel.\n"));
        LOG("Successfully exploited kernel.");
    }
    
    _upstage();
    
    {
        // Initialize jailbreak.
        _progress(localize(@"Initializing jailbreak..."));
        LOG("Finding kernel offsets...");
        __block kptr_t myOriginalSandboxAddr = KPTR_NULL;
        _assert(find_kernel_offsets(^{
            myOriginalSandboxAddr = swap_sandbox_for_proc(proc_struct_addr(), KPTR_NULL);
            _assert(myOriginalSandboxAddr != KPTR_NULL, localize(@"Unable to swap my process sandbox in kernel memory."));
            __block_assertion_handler();
        }, ^{
            if (myOriginalSandboxAddr == KPTR_NULL) return;
            _assert(swap_sandbox_for_proc(proc_struct_addr(), myOriginalSandboxAddr) == KPTR_NULL, localize(@"Unable to unswap my process sandbox in kernel memory."));
            __block_assertion_handler();
        }), localize(@"Unable to find kernel offsets."));
        __check_assertion_failure();
        LOG("Escaping sandbox...");
        myProcAddr = proc_struct_addr();
        LOG("myProcAddr = " ADDR, myProcAddr);
        _assert(KERN_POINTER_VALID(myProcAddr), localize(@"Unable to find my process in kernel memory."));
		myCredAddr = ReadKernel64(myProcAddr + koffset(KSTRUCT_OFFSET_PROC_UCRED));
		LOG("myCredAddr = " ADDR, myCredAddr);
		_assert(KERN_POINTER_VALID(myCredAddr), localize(@"Unable to find my process's credentials in kernel memory."));
		myLabelAddr = ReadKernel64(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
		LOG("myLabelAddr = " ADDR, myLabelAddr);
		_assert(KERN_POINTER_VALID(myLabelAddr), localize(@"Unable to find my process's MAC label in kernel memory."));
		kernelCredAddr = get_kernel_cred_addr();
        LOG("kernelCredAddr = " ADDR, kernelCredAddr);
        _assert(KERN_POINTER_VALID(kernelCredAddr), localize(@"Unable to find kernel's credentials in kernel memory."));
		kernelLabelAddr = ReadKernel64(kernelCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
		LOG("kernelLabelAddr = " ADDR, kernelLabelAddr);
		_assert(KERN_POINTER_VALID(kernelLabelAddr), localize(@"Unable to find kernel's label in kernel memory."));
		WriteKernel64(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL), kernelLabelAddr);
		WriteKernel32(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_UID), root_pw->pw_uid);
		WriteKernel32(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_RUID), root_pw->pw_uid);
        _assert(setuid(root_pw->pw_uid) == ERR_SUCCESS, localize(@"Unable to set user id."));
        _assert(getuid() == root_pw->pw_uid, localize(@"Unable to verify user id."));
        myHost = mach_host_self();
        _assert(MACH_PORT_VALID(myHost), localize(@"Unable to upgrade host port."));
		if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
			goto skip_profiles;
		}
        kptr_t platform_profile_kptr = ReadKernel64(kernel_offset(platform_profile));
        _assert(KERN_POINTER_VALID(platform_profile_kptr), localize(@"Unable to read platform profile pointer in kernel memory."));
        kptr_t platform_profile = ReadKernel64(platform_profile_kptr);
        _assert(KERN_POINTER_VALID(platform_profile), localize(@"Unable to read platform profile in kernel memory."));
        set_kernel_offset(real_platform_profile, platform_profile);
        uint32_t platform_profile_size = ReadKernel32(platform_profile_kptr + sizeof(kptr_t));
        _assert(platform_profile_size > 0, localize(@"Unable to read platform profile size in kernel memory"));
        kptr_t platform_profile_copy = kmem_alloc(platform_profile_size);
        _assert(KERN_POINTER_VALID(platform_profile_copy), localize(@"Unable to allocate platform profile copy in kernel memory."));
        _assert(kmemcpy(platform_profile_copy, platform_profile, platform_profile_size), localize(@"Unable to copy platform profile in kernel memory."));
        _assert(WriteKernel64(platform_profile_kptr, platform_profile_copy), localize(@"Unable to update platform profile in kernel memory."));
        kptr_t sandbox = get_sandbox(myLabelAddr);
        _assert(KERN_POINTER_VALID(sandbox), localize(@"Unable to find my sandbox in kernel memory."));
        kptr_t container_profile_kptr = ReadKernel64(sandbox);
        _assert(KERN_POINTER_VALID(container_profile_kptr), localize(@"Unable to read container profile pointer in kernel memory."));
        kptr_t container_profile = ReadKernel64(container_profile_kptr);
        _assert(KERN_POINTER_VALID(container_profile), localize(@"Unable to read container profile in kerneel memory"));
        set_kernel_offset(real_container_profile, container_profile);
        uint32_t container_profile_size = ReadKernel32(container_profile_kptr + sizeof(kptr_t));
        _assert(container_profile_size > 0, localize(@"Unable to read container profile size in kernel memory"));
        kptr_t container_profile_copy = kmem_alloc(container_profile_size);
        _assert(KERN_POINTER_VALID(container_profile_copy), localize(@"Unable to allocate container profile copy in kernel memory."));
        _assert(kmemcpy(container_profile_copy, container_profile, container_profile_size), localize(@"Unable to copy container profile in kernel memory."));
        set_kernel_offset(container_profile, container_profile_copy);
        static const char *sbops_allow[] = {
            "process-exec-interpreter",
            "file-map-executable",
            "file-read*",
            "file-read-data",
            "file-read-metadata",
            "mach-lookup",
            "mach-register",
            "load-unsigned-code",
            "dynamic-code-generation",
            "storage-class-map",
            NULL,
        };
        uint32_t *profiles = NULL;
        uint16_t *patches = NULL;
        uint32_t profiles_size = 0;
        uint32_t patches_size = 0;
        int n_profiles = 0;
        while (true) {
            uint32_t profile_offset = sizeof(kptr_t) + sizeof(uint16_t) * 2;
            if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0)
                profile_offset += sizeof(uint16_t);
            profile_offset += (kernel_size(sbops) + sizeof(uint16_t)) * sizeof(uint16_t) * n_profiles;
            uint16_t name_offset = ReadKernel16(container_profile_copy + profile_offset);
            if (name_offset == 0) break;
            uint32_t name_size = ReadKernel32(container_profile_copy + name_offset * sizeof(kptr_t));
            if (name_size == 0) break;
            if (name_size > INT32_MAX) break;
            char *name = calloc(1, name_size);
            if (name == NULL) break;
            rkbuffer(container_profile_copy + name_offset * sizeof(kptr_t) + sizeof(uint32_t), name, name_size);
            size_t len = strlen(name);
            SafeFreeNULL(name);
            if (len == 0) break;
            uint16_t patch_offset = 0;
            for (uint32_t sbop_value = 0, index = 0; sbop_value != true; index++) {
                patch_offset = ReadKernel16(container_profile_copy + profile_offset + index * sizeof(uint16_t));
                sbop_value = ReadKernel32(container_profile_copy + patch_offset * sizeof(kptr_t));
            }
            profiles_size += sizeof(*profiles);
            patches_size += sizeof(*patches);
            if (n_profiles == 0) {
                profiles = malloc(profiles_size);
                if (profiles == NULL) break;
                patches = malloc(patches_size);
                if (patches == NULL) break;
            } else {
                profiles = reallocf(profiles, profiles_size);
                if (profiles == NULL) break;
                patches = reallocf(patches, patches_size);
                if (patches == NULL) break;
            }
            profiles[n_profiles] = profile_offset;
            patches[n_profiles] = patch_offset;
            n_profiles++;
        }
        uint16_t platform_profile_patch = 0;
        for (uint32_t sbop_value = 0, index = 0; sbop_value != true; index++) {
            platform_profile_patch = ReadKernel16(platform_profile_copy + index * sizeof(uint16_t));
            sbop_value = ReadKernel32(platform_profile_copy + platform_profile_patch * sizeof(kptr_t));
        }
        for (const char **allow = sbops_allow; *allow; allow++) {
            size_t idx = (size_t)get_offset(*allow);
            WriteKernel16(platform_profile_copy + sizeof(kptr_t) + sizeof(uint16_t) * 2 + idx * sizeof(uint16_t), platform_profile_patch);
            for (int i = 0; i < n_profiles; i++) {
                WriteKernel16(container_profile_copy + profiles[i] + sizeof(uint32_t) + idx * sizeof(uint16_t), patches[i]);
            }
        }
        SafeFreeNULL(profiles);
        SafeFreeNULL(patches);
	skip_profiles:;
        LOG("Successfully escaped sandbox.");
		LOG("Setting HSP#4 as TFP0...");
		if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
			_assert(kernel_host_set_special_port(myHost, 4, tfp0) == KERN_SUCCESS, localize(@"Unable to set HSP#4 as TFP0."));
		} else {
			_assert(remap_tfp0_set_hspn(4, tfp0), localize(@"Unable to remap TFP0 and set HSP#4."));
		}
		LOG("Successfully set HSP#4 as TFP0.");
		_insert_status(localize(@"Set HSP#4 as TFP0.\n"));
		LOG("Setting kernel task info...");
        _assert(set_kernel_task_info(), localize(@"Unable to set kernel task info."));
        LOG("Successfully set kernel task info.");
        _insert_status(localize(@"Set kernel task info.\n"));
        LOG("Platformizing...");
        _assert(set_platform_binary(myProcAddr, true), localize(@"Unable to make my task a platform task."));
        _assert(set_cs_platform_binary(myProcAddr, true), localize(@"Unable to make my codesign blob a platform blob."));
		amfidPid = pidOfProcess("/usr/libexec/amfid");
        if (amfidPid != 0) {
            real_cdhash = calloc(1, CS_CDHASH_LEN);
            _assert(real_cdhash != NULL, localize(@"Unable to allocate memory for real cdhash."));
            _assert(csops(my_pid, CS_OPS_CDHASH, real_cdhash, CS_CDHASH_LEN) == 0, localize(@"Unable to copy my cdhash."));
			fake_cdhash = calloc(1, CS_CDHASH_LEN);
            _assert(fake_cdhash != NULL, localize(@"Unable to allocate memory for fake cdhash."));
            _assert(csops(amfidPid, CS_OPS_CDHASH, fake_cdhash, CS_CDHASH_LEN) == 0, localize(@"Unable to copy amfid's cdhash."));
			kptr_t myVnodeAddr = ReadKernel64(myProcAddr + koffset(KSTRUCT_OFFSET_PROC_TEXTVP));
            _assert(KERN_POINTER_VALID(myVnodeAddr), localize(@"Unable to find my vnode address in kernel memory."));
            kptr_t myUbcInfoAddr = ReadKernel64(myVnodeAddr + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
            _assert(KERN_POINTER_VALID(myUbcInfoAddr), localize(@"Unable to find my ubcinfo address in kernel memory."));
            myCSBlobsAddr = ReadKernel64(myUbcInfoAddr + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
            _assert(KERN_POINTER_VALID(myCSBlobsAddr), localize(@"Unable to find my csblobs address in kernel memory."));
            kptr_t currentCSBlob = myCSBlobsAddr;
            while (KERN_POINTER_VALID(currentCSBlob)) {
                wkbuffer(currentCSBlob + offsetof(struct cs_blob, csb_cdhash), fake_cdhash, CS_CDHASH_LEN);
                currentCSBlob = ReadKernel64(currentCSBlob + offsetof(struct cs_blob, csb_next));
            }
            _assert(host_get_amfid_port(myHost, &real_amfid_port) == KERN_SUCCESS, localize(@"Unable to get original amfid port."));
            mach_port_options_t options;
            options.flags = MPO_INSERT_SEND_RIGHT;
            _assert(mach_port_construct(mach_task_self(), &options, 0, &fake_amfid_port) == KERN_SUCCESS, localize(@"Unable to get fake amfid port."));
            _assert(host_set_amfid_port(myHost, fake_amfid_port) == KERN_SUCCESS, localize(@"Unable to set fake amfid port."));
            amfid_queue = dispatch_queue_create("science.xnu.MobileFileIntegrity", DISPATCH_QUEUE_CONCURRENT);
            _assert(amfid_queue != NULL, localize(@"Unable to create amfid queue."));
            dispatch_source_t amfid_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_MACH_RECV, fake_amfid_port, 0, amfid_queue);
            _assert(amfid_queue != NULL, localize(@"Unable to create amfid queue."));
            dispatch_source_set_event_handler(amfid_source, ^{
                dispatch_mig_server(amfid_source, 4184, amfid_server);
            });
            dispatch_resume(amfid_source);
        }
		
		ensure_directory("/private/var/mnt", 0, 0755);
		ensure_directory("/private/var/mnt/jb", 0, 0755);
        LOG("Successfully initialized jailbreak.");
    }
	
    _upstage();
    
    {
        if (prefs->export_kernel_task_port) {
            // Export kernel task port.
            _progress(localize(@"Exporting kernel task port..."));
            _assert(export_tfp0(myOriginalHost), localize(@"Unable to export TFP0."));
            LOG("Successfully exported kernel task port.");
            _insert_status(localize(@"Exported kernel task port.\n"));
        } else {
            // Unexport kernel task port.
            _progress(localize(@"Unexporting kernel task port..."));
            _assert(unexport_tfp0(myOriginalHost), localize(@"Unable to unexport TFP0."));
            LOG("Successfully unexported kernel task port.");
            _insert_status(localize(@"Unexported kernel task port.\n"));
        }
    }
    
    _upstage();
    
    {
        // Write a test file to UserFS.
        
        _progress(localize(@"Writing a test file to UserFS..."));
        const char *testFile = [NSString stringWithFormat:@"/var/mobile/test-%lu.txt", time(NULL)].UTF8String;
        _write_test_file(testFile);
        LOG("Successfully wrote a test file to UserFS.");
    }
    
    _upstage();
    
    {
        if (prefs->dump_apticket) {
            NSString *originalFile = @"/System/Library/Caches/apticket.der";
            NSString *dumpFile = [homeDirectory stringByAppendingPathComponent:@"Documents/apticket.der"];
            if (![sha1sum(originalFile) isEqualToString:sha1sum(dumpFile)]) {
                // Dump APTicket.
                
                _progress(localize(@"Dumping APTicket..."));
                NSData *fileData = [NSData dataWithContentsOfFile:originalFile];
                _assert(([fileData writeToFile:dumpFile atomically:YES]), localize(@"Unable to dump APTicket."));
                LOG("Successfully dumped APTicket.");
            }
            _insert_status(localize(@"Dumped APTicket.\n"));
        }
    }
    
    _upstage();
    
    if (prefs->overwrite_boot_nonce) {
        _progress(localize(@"Overwriting boot nonce..."));
        _assert(kernel_set_generator(prefs->boot_nonce), localize(@"Unable to set generator."));
        LOG("Overwrote boot nonce.");
        _insert_status(localize(@"Overwrote boot nonce.\n"));
    }
    
    _upstage();
    
    {
        // Log slide.
        
        _progress(localize(@"Logging slide..."));
        NSString *file = @(SLIDE_FILE);
        NSData *fileData = [[NSString stringWithFormat:@(ADDR "\n"), kernel_slide] dataUsingEncoding:NSUTF8StringEncoding];
        if (![[NSData dataWithContentsOfFile:file] isEqual:fileData]) {
            _assert(clean_file(file.UTF8String), localize(@"Unable to clean old kernel slide log."));
            _assert(create_file_data(file.UTF8String, root_pw->pw_uid, 0644, fileData), localize(@"Unable to log kernel slide."));
        }
        LOG("Successfully logged slide.");
        _insert_status(localize(@"Logged slide.\n"));
    }
    
    _upstage();
    
    {
        // Log ECID.
        
        _progress(localize(@"Logging ECID..."));
        NSString *ECID = getECID();
        if (ECID != nil) {
            prefs->ecid = ECID.UTF8String;
            _sync_prefs();
        } else {
            LOG("I couldn't get the ECID... Am I running on a real device?");
        }
        LOG("Successfully logged ECID.");
        _insert_status(localize(@"Logged ECID.\n"));
    }
    
    _upstage();
    
    {
        NSArray *array = @[
			@"/var/MobileAsset/Assets/com_apple_MobileAsset_SoftwareUpdate",
			@"/var/MobileAsset/Assets/com_apple_MobileAsset_SoftwareUpdateDocumentation",
			@"/var/MobileAsset/AssetsV2/com_apple_MobileAsset_SoftwareUpdate",
			@"/var/MobileAsset/AssetsV2/com_apple_MobileAsset_SoftwareUpdateDocumentation",
			@"/var/MobileSoftwareUpdate/MobileAsset/AssetsV2"
		];
        if (prefs->disable_auto_updates && !prefs->restore_rootfs) {
            // Disable Auto Updates.
            
            _progress(localize(@"Disabling Auto Updates..."));
            for (NSString *path in array) {
                ensure_symlink("/dev/null", path.UTF8String);
            }
            _assert(modifyPlist(@"/var/mobile/Library/Preferences/com.apple.Preferences.plist", ^(id plist) {
                plist[@"kBadgedForSoftwareUpdateKey"] = @NO;
                plist[@"kBadgedForSoftwareUpdateJumpOnceKey"] = @NO;
            }), localize(@"Unable to disable software update badge."));
            LOG("Successfully disabled Auto Updates.");
            _insert_status(localize(@"Disabled Auto Updates.\n"));
        } else {
            // Enable Auto Updates.
            
            _progress(localize(@"Enabling Auto Updates..."));
            for (NSString *path in array) {
                ensure_directory(path.UTF8String, root_pw->pw_uid, 0755);
            }
            _assert(modifyPlist(@"/var/mobile/Library/Preferences/com.apple.Preferences.plist", ^(id plist) {
                plist[@"kBadgedForSoftwareUpdateKey"] = @YES;
                plist[@"kBadgedForSoftwareUpdateJumpOnceKey"] = @YES;
            }), localize(@"Unable to enable software update badge."));
            _insert_status(localize(@"Enabled Auto Updates.\n"));
        }
    }
    
    if (prefs->read_only_rootfs) {
        NSJailbreakDirectory = [@"/private/var/containers/Bundle" stringByAppendingPathComponent:NSJailbreakDirectory];
        jailbreakDirectory = [NSJailbreakDirectory UTF8String];
        ensure_symlink(jailbreakDirectory, "/private/var/jb");
        ensure_symlink(jailbreakDirectory, "/private/var/containers/Bundle/iosbinpack64");
        needStrap = YES;
        goto install;
    } else {
        clean_file("/private/var/containers/Bundle/jb");
        ensure_symlink(jailbreakDirectory, "/private/var/jb");
        clean_file("/private/var/containers/Bundle/iosbinpack64");
    }
    
    _upstage();
    
    {
        // Remount RootFS.
        
        _progress(localize(@"Remounting RootFS..."));
        int rootfd = open("/", O_RDONLY);
        _assert(rootfd > 0, localize(@"Unable to open RootFS."));
        kptr_t rootfs_vnode = find_vnode_with_fd(myProcAddr, rootfd);
        _assert(KERN_POINTER_VALID(rootfs_vnode), localize(@"Unable to get vnode for RootFS."));
        kptr_t v_mount = ReadKernel64(rootfs_vnode + koffset(KSTRUCT_OFFSET_VNODE_V_MOUNT));
        _assert(KERN_POINTER_VALID(v_mount), localize(@"Unable to get mount info for RootFS."));
        const char **snapshots = snapshot_list(rootfd);
        char *systemSnapshot = copySystemSnapshot();
        _assert(systemSnapshot != NULL, localize(@"Unable to copy system snapshot."));
        char *original_snapshot = "orig-fs";
        bool has_original_snapshot = NO;
        char *thedisk = "/dev/disk0s1s1";
        char *oldest_snapshot = NULL;
        _assert(runCommand("/sbin/mount", NULL) == ERR_SUCCESS, localize(@"Unable to print mount list."));
        if (snapshots == NULL) {
			// Purge updates.
			LOG("Purging updates...");
			ArchiveFile *updatePurger = [ArchiveFile archiveWithFile:pathForResource(@"purgeUpdates.tar.lzma")];
			_assert([updatePurger extractToPath:@"/private/var/containers/Bundle/"], localize(@"Unable to extract update purger."));
			_assert(runCommand("/private/var/containers/Bundle/purgeUpdates", NULL) != EXIT_SUCCESS, localize(@"Unable to purge updates."));
			clean_file(_jailbreak_file("/private/var/containers/Bundle/purgeUpdates"));
			
            // Mount RootFS.
            
            LOG("Mounting RootFS...");
            kptr_t devVnode = ReadKernel64(v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_DEVVP));
            _assert(KERN_POINTER_VALID(devVnode), localize(@"Unable to get vnode for root device."));
            kptr_t v_specinfo = ReadKernel64(devVnode + koffset(KSTRUCT_OFFSET_VNODE_VU_SPECINFO));
            _assert(KERN_POINTER_VALID(v_specinfo), localize(@"Unable to get specinfo for root device."));
            WriteKernel32(v_specinfo + koffset(KSTRUCT_OFFSET_SPECINFO_SI_FLAGS), 0);
            NSString *invalidRootMessage = localize(@"RootFS already mounted, delete OTA file from Settings - Storage if present and reboot.");
            _assert(!is_mountpoint("/var/MobileSoftwareUpdate/mnt1"), invalidRootMessage);
            char *rootFsMountPoint = "/private/var/mnt/jb/mnt1";
            if (is_mountpoint(rootFsMountPoint)) {
                _assert(unmount(rootFsMountPoint, MNT_FORCE) == ERR_SUCCESS, localize(@"Unable to unmount old RootFS mount point."));
            }
            _assert(clean_file(rootFsMountPoint), localize(@"Unable to clean old RootFS mount point."));
            _assert(ensure_directory(rootFsMountPoint, root_pw->pw_uid, 0755), localize(@"Unable to create RootFS mount point."));
            const char *argv[] = {"/sbin/mount_apfs", thedisk, rootFsMountPoint, NULL};
            _assert(runCommandv(argv[0], 3, argv, ^(pid_t pid) {
                kptr_t procStructAddr = get_proc_struct_for_pid(pid);
                _assert(KERN_POINTER_VALID(procStructAddr), localize(@"Unable to find mount_apfs's process in kernel memory."));
                give_creds_to_process_at_addr(procStructAddr, kernelCredAddr);
                __block_assertion_handler();
            }, true) == ERR_SUCCESS, localize(@"Unable to mount RootFS."));
            __check_assertion_failure();
            _assert(runCommand("/sbin/mount", NULL) == ERR_SUCCESS, localize(@"Unable to print new mount list."));
            const char *systemSnapshotLaunchdPath = [@(rootFsMountPoint) stringByAppendingPathComponent:@"sbin/launchd"].UTF8String;
            _assert(waitForFile(systemSnapshotLaunchdPath) == ERR_SUCCESS, localize(@"Unable to verify newly mounted RootFS."));
            LOG("Successfully mounted RootFS.");
            
            // Rename system snapshot.
            
            LOG("Renaming system snapshot...");
            close(rootfd);
            rootfd = open(rootFsMountPoint, O_RDONLY);
            _assert(rootfd > 0, localize(@"Unable to open newly mounted RootFS."));
            rootfs_vnode = find_vnode_with_fd(myProcAddr, rootfd);
            _assert(KERN_POINTER_VALID(rootfs_vnode), localize(@"Unable to get vnode for newly mounted RootFS."));
            v_mount = ReadKernel64(rootfs_vnode + koffset(KSTRUCT_OFFSET_VNODE_V_MOUNT));
            _assert(KERN_POINTER_VALID(v_mount), localize(@"Unable to get mount info for newly mounted RootFS."));
            snapshots = snapshot_list(rootfd);
            _assert(snapshots != NULL, localize(@"Unable to get snapshots for newly mounted RootFS."));
            LOG("Snapshots on newly mounted RootFS:");
            for (const char **snapshot = snapshots; *snapshot; snapshot++) {
                LOG("\t%s", *snapshot);
                if (strcmp(*snapshot, original_snapshot) == 0) {
                    LOG("Clearing old original system snapshot...");
                    _assert(_kerncred(fs_snapshot_delete(rootfd, original_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to clear old original system snapshot."));
                }
            }
            SafeFreeNULL(snapshots);
            NSString *systemVersionPlist = @"/System/Library/CoreServices/SystemVersion.plist";
            NSString *rootSystemVersionPlist = [@(rootFsMountPoint) stringByAppendingPathComponent:systemVersionPlist];
            NSDictionary *snapshotSystemVersion = [NSDictionary dictionaryWithContentsOfFile:systemVersionPlist];
            _assert(snapshotSystemVersion != nil, localize(@"Unable to get SystemVersion.plist for RootFS."));
            NSDictionary *rootfsSystemVersion = [NSDictionary dictionaryWithContentsOfFile:rootSystemVersionPlist];
            _assert(rootfsSystemVersion != nil, localize(@"Unable to get SystemVersion.plist for newly mounted RootFS."));
            if (![rootfsSystemVersion[@"ProductBuildVersion"] isEqualToString:snapshotSystemVersion[@"ProductBuildVersion"]]) {
                LOG("snapshot VersionPlist: %@", snapshotSystemVersion);
                LOG("rootfs VersionPlist: %@", rootfsSystemVersion);
                _assert("BuildVersions match"==NULL, invalidRootMessage);
            }
            char *test_snapshot = "test-snapshot";
            _assert(_kerncred(fs_snapshot_create(rootfd, test_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to create test snapshot."));
            _assert(_kerncred(fs_snapshot_delete(rootfd, test_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to delete test snapshot."));
            kptr_t vp = ReadKernel64(v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_VNODELIST_TQH_FIRST));
            while (vp != KPTR_NULL) {
                kptr_t ncp = ReadKernel64(vp + koffset(KSTRUCT_OFFSET_VNODE_V_NCCHILDREN_TQH_FIRST));
                while (ncp != KPTR_NULL) {
                    kptr_t nc_vp = ReadKernel64(ncp + koffset(KSTRUCT_OFFSET_NAMECACHE_NC_VP));
                    if (nc_vp != KPTR_NULL) {
                        kptr_t v_data = ReadKernel64(nc_vp + koffset(KSTRUCT_OFFSET_VNODE_V_DATA));
                        if (v_data != KPTR_NULL) {
                            uint32_t flags = ReadKernel32(v_data + 49);
                            if ((flags & 0x40) != 0) {
                                flags &= ~0x40;
                                WriteKernel32(v_data + 49, flags);
                            }
                        }
                    }
                    ncp = ReadKernel64(ncp + koffset(KSTRUCT_OFFSET_NAMECACHE_NC_CHILD_TQE_NEXT));
                }
                vp = ReadKernel64(vp + koffset(KSTRUCT_OFFSET_VNODE_V_MNTVNODES_TQE_NEXT));
            }
            _assert(_kerncred(fs_snapshot_rename(rootfd, systemSnapshot, original_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to rename system snapshot."));
            LOG("Successfully renamed system snapshot.");
            
            // Reboot.
            close(rootfd);
            
            LOG("Rebooting...");
            _notice(localize(@"The system snapshot has been successfully renamed. The device will now be restarted."), true, false);
            _assert(reboot(RB_QUICK) == ERR_SUCCESS, localize(@"Unable to call reboot."));
            _assert(false, localize(@"Unable to reboot device."));
            LOG("Successfully rebooted.");
        } else {
            LOG("APFS Snapshots:");
            for (const char **snapshot = snapshots; *snapshot; snapshot++) {
                if (oldest_snapshot == NULL) {
                    oldest_snapshot = strdup(*snapshot);
                }
                if (strcmp(original_snapshot, *snapshot) == 0) {
                    has_original_snapshot = YES;
                }
                LOG("%s", *snapshot);
            }
        }
        uint32_t v_flag = ReadKernel32(v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_FLAG));
        if ((v_flag & MNT_RDONLY) || (v_flag & MNT_NOSUID)) {
            v_flag &= ~(MNT_RDONLY | MNT_NOSUID);
            WriteKernel32(v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_FLAG), v_flag & ~MNT_ROOTFS);
            char *opts = strdup(thedisk);
            _mem_assert(opts);
            _assert(mount("apfs", "/", MNT_UPDATE, (void *)&opts) == ERR_SUCCESS, localize(@"Unable to remount RootFS."));
            SafeFreeNULL(opts);
            WriteKernel32(v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_FLAG), v_flag);
        }
        _assert(runCommand("/sbin/mount", NULL) == ERR_SUCCESS, localize(@"Unable to print new mount list."));
        NSString *file = [NSString stringWithContentsOfFile:@"/.installed_unc0ver" encoding:NSUTF8StringEncoding error:nil];
        needStrap = file == nil;
        needStrap |= ![file isEqualToString:@""] && ![file isEqualToString:[NSString stringWithFormat:@"%f\n", kCFCoreFoundationVersionNumber]];
        needStrap &= access("/electra", F_OK) != ERR_SUCCESS;
        needStrap &= access("/chimera", F_OK) != ERR_SUCCESS;
        if (needStrap)
            LOG("We need strap.");
        if (!has_original_snapshot) {
            if (oldest_snapshot != NULL) {
                _assert(_kerncred(fs_snapshot_rename(rootfd, oldest_snapshot, original_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to rename oldest snapshot."));
            } else if (needStrap) {
                _assert(_kerncred(fs_snapshot_create(rootfd, original_snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to create stock snapshot."));
            }
        }
        close(rootfd);
        SafeFreeNULL(snapshots);
        SafeFreeNULL(systemSnapshot);
        SafeFreeNULL(oldest_snapshot);
        LOG("Successfully remounted RootFS.");
        _insert_status(localize(@"Remounted RootFS.\n"));
    }
    
    _upstage();
    
    {
        // Write a test file to RootFS.
        
        _progress(localize(@"Writing a test file to RootFS..."));
        const char *testFile = [NSString stringWithFormat:@"/test-%lu.txt", time(NULL)].UTF8String;
        _write_test_file(testFile);
        LOG("Successfully wrote a test file to RootFS.");
    }
    
    _upstage();
    
    {
        NSArray *array = @[@"/var/Keychains/ocspcache.sqlite3",
                                 @"/var/Keychains/ocspcache.sqlite3-shm",
                                 @"/var/Keychains/ocspcache.sqlite3-wal"];
        if (prefs->disable_app_revokes && kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iOS_12_0) {
            // Disable app revokes.
            _progress(localize(@"Disabling app revokes..."));
            blockDomainWithName("ocsp.apple.com");
            for (NSString *path in array) {
                ensure_symlink("/dev/null", path.UTF8String);
            }
            LOG("Successfully disabled app revokes.");
            _insert_status(localize(@"Disabled App Revokes.\n"));
        } else {
            // Enable app revokes.
            _progress(localize(@"Enabling app revokes..."));
            unblockDomainWithName("ocsp.apple.com");
            for (NSString *path in array) {
                if (is_symlink(path.UTF8String)) {
                    clean_file(path.UTF8String);
                }
            }
            LOG("Successfully enabled app revokes.");
            _insert_status(localize(@"Enabled App Revokes.\n"));
        }
    }

install:;
    
    _upstage();
    
    {
        if (prefs->restore_rootfs) {
            _progress(localize(@"Restoring RootFS..."));
            _notice(localize(@"Will restore RootFS. This may take a while. Don't exit the app and don't let the device lock."), 1, 1);
            LOG("Reverting back RootFS remount...");
            int rootfd = open("/", O_RDONLY);
            _assert(rootfd > 0, localize(@"Unable to open RootFS."));
            const char **snapshots = snapshot_list(rootfd);
            _assert(snapshots != NULL, localize(@"Unable to get snapshots for RootFS."));
            _assert(*snapshots != NULL, localize(@"Found no snapshot for RootFS."));
            char *snapshot = strdup(*snapshots);
            _mem_assert(snapshot);
            LOG("%s", snapshot);
            if (!(kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iOS_11_3)) {
                char *systemSnapshot = copySystemSnapshot();
                _assert(systemSnapshot != NULL, localize(@"Unable to copy system snapshot."));
                _assert(_kerncred(fs_snapshot_rename(rootfd, snapshot, systemSnapshot, 0)) == ERR_SUCCESS, localize(@"Unable to rename original snapshot."));
                SafeFreeNULL(snapshot);
                snapshot = strdup(systemSnapshot);
                _assert(snapshot != NULL, localize(@"Unable to duplicate string."));
                SafeFreeNULL(systemSnapshot);
            }
            char *systemSnapshotMountPoint = "/private/var/mnt/jb/mnt2";
            if (is_mountpoint(systemSnapshotMountPoint)) {
                _assert(_kerncred(unmount(systemSnapshotMountPoint, MNT_FORCE)) == ERR_SUCCESS, localize(@"Unable to unmount old snapshot mount point."));
            }
            _assert(clean_file(systemSnapshotMountPoint), localize(@"Unable to clean old snapshot mount point."));
            _assert(ensure_directory(systemSnapshotMountPoint, root_pw->pw_uid, 0755), localize(@"Unable to create snapshot mount point."));
            _assert(_kerncred(fs_snapshot_mount(rootfd, systemSnapshotMountPoint, snapshot, 0)) == ERR_SUCCESS, localize(@"Unable to mount original snapshot."));
            const char *systemSnapshotLaunchdPath = [@(systemSnapshotMountPoint) stringByAppendingPathComponent:@"sbin/launchd"].UTF8String;
            _assert(waitForFile(systemSnapshotLaunchdPath) == ERR_SUCCESS, localize(@"Unable to verify mounted snapshot."));
            _assert(extractDebsForPkg(@"rsync", nil, false, true), localize(@"Unable to extract rsync."));
            _assert(extractDebsForPkg(@"uikittools", nil, false, true), localize(@"Unable to extract uikittools."));
            if (kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iOS_11_3) {
                _assert(runCommand("/usr/bin/rsync", "-vaxcH", "--progress", "--delete-after", "--exclude=/Developer", "--exclude=/usr/bin/uicache", "--exclude=/usr/bin/find", [@(systemSnapshotMountPoint) stringByAppendingPathComponent:@"."].UTF8String, "/", NULL) == 0, localize(@"Unable to sync /."));
            } else {
                _assert(runCommand("/usr/bin/rsync", "-vaxcH", "--progress", "--delete", [@(systemSnapshotMountPoint) stringByAppendingPathComponent:@"Applications/."].UTF8String, "/Applications", NULL) == 0, localize(@"Unable to sync /Applications."));
            }
            _assert(_kerncred(unmount(systemSnapshotMountPoint, MNT_FORCE)) == ERR_SUCCESS, localize(@"Unable to unmount original snapshot mount point."));
            _assert(runCommand("/usr/bin/uicache", NULL) >= 0, localize(@"Unable to refresh icon cache."));
            _assert(clean_file("/usr/bin/uicache"), localize(@"Unable to clean uicache binary."));
            _assert(clean_file("/usr/bin/find"), localize(@"Unable to clean find binary."));
            LOG("Successfully reverted back RootFS remount.");
clean:;
            close(rootfd);
            SafeFreeNULL(snapshot);
            SafeFreeNULL(snapshots);
            // Clean up.
            
            LOG("Cleaning up...");
            NSArray *cleanUpFileList = @[@"/var/cache",
                                        @"/var/lib",
                                        @"/var/stash",
                                        @"/var/db/stash",
                                        @"/var/mobile/Library/Cydia",
                                        @"/var/mobile/Library/Caches/com.saurik.Cydia",
                                        @"/var/jb",
                                        @"/var/containers/Bundle/jb",
                                        @"/var/containers/Bundle/iosbinpack64",
                                        @"/var/dropbear",
                                        @"/var/profile"];
            for (NSString *file in cleanUpFileList) {
                clean_file(file.UTF8String);
            }
            LOG("Successfully cleaned up.");
			
			// Disable RootFS Restore.
            
            LOG("Disabling RootFS Restore...");
            prefs->restore_rootfs = false;
            _sync_prefs();
            LOG("Successfully disabled RootFS Restore.");
            
            _insert_status(localize(@"Restored RootFS.\n"));
			
            // Disallow SpringBoard to show non-default system apps.
            
            LOG("Disallowing SpringBoard to show non-default system apps...");
            _assert(modifyPlist(@"/var/mobile/Library/Preferences/com.apple.springboard.plist", ^(id plist) {
                plist[@"SBShowNonDefaultSystemApps"] = @NO;
            }), localize(@"Unable to update SpringBoard preferences."));
            LOG("Successfully disallowed SpringBoard to show non-default system apps.");
            
            // Reboot.
            
            LOG("Rebooting...");
            _notice(localize(@"RootFS has been successfully restored. The device will now be restarted."), true, false);
            _assert(reboot(RB_QUICK) == ERR_SUCCESS, localize(@"Unable to call reboot."));
            _assert(false, localize(@"Unable to reboot device."));
            LOG("Successfully rebooted.");
        }
    }
    
    _upstage();
    
    {
        // Allow SpringBoard to show non-default system apps.
        
        _progress(localize(@"Allowing SpringBoard to show non-default system apps..."));
        _assert(modifyPlist(@"/var/mobile/Library/Preferences/com.apple.springboard.plist", ^(id plist) {
            plist[@"SBShowNonDefaultSystemApps"] = @YES;
        }), localize(@"Unable to update SpringBoard preferences."));
        LOG("Successfully allowed SpringBoard to show non-default system apps.");
        _insert_status(localize(@"Allowed SpringBoard to show non-default system apps.\n"));
    }
    
    _upstage();
    
    {
        // Create jailbreak directory.
        
        _progress(localize(@"Creating jailbreak directory..."));
        _assert(ensure_directory(jailbreakDirectory, root_pw->pw_uid, 0755), localize(@"Unable to create jailbreak directory."));
        _assert(chdir(jailbreakDirectory) == ERR_SUCCESS, localize(@"Unable to change working directory to jailbreak directory."));
        LOG("Successfully created jailbreak directory.");
        _insert_status(localize(@"Created jailbreak directory.\n"));
    }
	
	clean_file("/jb/offsets.plist");
    
    _upstage();
    
    if (prefs->ssh_only && needStrap) {
        _progress(localize(@"Enabling SSH..."));
        if (!verifySums(pathForResource(@"binpack64-256.md5sums"), HASHTYPE_MD5)) {
            ArchiveFile *binpack64 = [ArchiveFile archiveWithFile:pathForResource(@"binpack64-256.tar.lzma")];
            _assert(binpack64 != nil, localize(@"Unable to open binpack."));
            _assert([binpack64 extractToPath:NSJailbreakDirectory], localize(@"Unable to extract binpack."));
        }
        if (!pidOfProcess(_jailbreak_file("jbid")) && !pidOfProcess(_jailbreak_file("jbinit"))) {
            const char *argv[] = { _jailbreak_file("jbinit"), NULL };
            __block pid_t jbinit_pid = 0;
            _assert(runCommandv(argv[0], 0, argv, ^(pid_t pid) {
                kptr_t procAddr = get_proc_struct_for_pid(pid);
                _assert(KERN_POINTER_VALID(procAddr), localize(@"Unable to find jbinit's process address in kernel memory."));
                kptr_t vnodeAddr = ReadKernel64(procAddr + koffset(KSTRUCT_OFFSET_PROC_TEXTVP));
                _assert(KERN_POINTER_VALID(vnodeAddr), localize(@"Unable to find jbinit's vnode address in kernel memory."));
                kptr_t ubcInfoAddr = ReadKernel64(vnodeAddr + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
                _assert(KERN_POINTER_VALID(ubcInfoAddr), localize(@"Unable to find jbinit's ubcinfo address in kernel memory."));
                kptr_t CSBlobsAddr = ReadKernel64(ubcInfoAddr + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
                _assert(KERN_POINTER_VALID(CSBlobsAddr), localize(@"Unable to find jbinit's csblobs address in kernel memory."));
                while (KERN_POINTER_VALID(CSBlobsAddr)) {
                    wkbuffer(CSBlobsAddr + offsetof(struct cs_blob, csb_cdhash), fake_cdhash, CS_CDHASH_LEN);
                    CSBlobsAddr = ReadKernel64(CSBlobsAddr + offsetof(struct cs_blob, csb_next));
                }
                host_set_amfid_port(myHost, real_amfid_port);
                real_amfid_port = MACH_PORT_NULL;
                mach_port_destroy(mach_task_self(), fake_amfid_port);
                fake_amfid_port = MACH_PORT_NULL;
                jbinit_pid = pid;
                __block_assertion_handler();
            }, false) == 0, localize(@"Unable to initialize jbinit."));
            __check_assertion_failure();
            while (access("/private/var/tmp/jbinit.consumed", F_OK) != 0) {
                continue;
            }
            jbinit_pid++;
            kernel_set_signals_enabled(jbinit_pid, false);
            if (amfidPid != 0) {
                kill(amfidPid, SIGKILL);
                amfidPid = 0;
            }
            unlink("/private/var/tmp/jbinit.consumed");
        }
        _assert(runCommand(_jailbreak_file("usr/bin/uname"), "-a", NULL) == 0, localize(@"Unable to obtain unsigned code execution"));
        NSString *binpackMessage = localize(@"Unable to setup binpack.");
        if (prefs->read_only_rootfs) {
            goto enable_ssh;
        }
        _assert(ensure_symlink(_jailbreak_file("usr/bin/scp"), "/usr/bin/scp"), binpackMessage);
        _assert(ensure_directory("/usr/local/lib", root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_directory("/usr/local/lib/zsh", root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_directory("/usr/local/lib/zsh/5.0.8", root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("/usr/local/lib/zsh/5.0.8/zsh"), "/usr/local/lib/zsh/5.0.8/zsh"), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("bin/zsh"), "/bin/zsh"), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("etc/zshrc"), "/etc/zshrc"), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("usr/share/terminfo"), "/usr/share/terminfo"), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("usr/local/bin"), "/usr/local/bin"), binpackMessage);
        _assert(ensure_symlink(_jailbreak_file("etc/profile"), "/etc/profile"), binpackMessage);
        _assert(ensure_directory("/etc/dropbear", root_pw->pw_uid, 0755), binpackMessage);
enable_ssh:;
        _assert(ensure_symlink(_jailbreak_file("etc/profile"), "/var/profile"), binpackMessage);
        _assert(ensure_directory("/var/dropbear", root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_directory(_jailbreak_file("Library"), root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_directory(_jailbreak_file("Library/LaunchDaemons"), root_pw->pw_uid, 0755), binpackMessage);
        _assert(ensure_directory(_jailbreak_file("etc/rc.d"), root_pw->pw_uid, 0755), binpackMessage);
        _assert(runCommandv(_jailbreak_file("usr/local/bin/dropbear"), 5, (const char *const *)ARRAY(char *, _jailbreak_file("usr/local/bin/dropbear"), "-R", "-E", "--shell", _jailbreak_file("bin/bash"), NULL), ^(pid_t pid) {
            kptr_t proc = get_proc_struct_for_pid(pid);
            set_platform_binary(proc, true);
            set_cs_platform_binary(proc, true);
        }, false) == 0, localize(@"Unable to start dropbear"));
        if (prefs->load_daemons) {
            for (NSString *file in [fileManager contentsOfDirectoryAtPath:_NSJailbreakFile(@"Library/LaunchDaemons") error:nil]) {
                NSString *path = [_NSJailbreakFile(@"Library/LaunchDaemons") stringByAppendingPathComponent:file];
                runCommand(_jailbreak_file("bin/launchctl"), "load", path.UTF8String, NULL);
            }
            for (NSString *file in [fileManager contentsOfDirectoryAtPath:_NSJailbreakFile(@"etc/rc.d") error:nil]) {
                NSString *path = [_NSJailbreakFile(@"etc/rc.d") stringByAppendingPathComponent:file];
                if ([fileManager isExecutableFileAtPath:path]) {
                    runCommand(_jailbreak_file("bin/bash"), "-c", path.UTF8String, NULL);
                }
            }
        }
        if (prefs->run_uicache) {
            _assert(runCommand(_jailbreak_file("usr/bin/uicache"), NULL) == ERR_SUCCESS, localize(@"Unable to refresh icon cache."));
        }
        _assert(runCommand(_jailbreak_file("bin/launchctl"), "stop", "com.apple.cfprefsd.xpc.daemon", NULL) == ERR_SUCCESS, localize(@"Unable to flush preference cache."));
        LOG("Successfully enabled SSH.");
        _insert_status(localize(@"Enabled SSH.\n"));
    }
    
    if (prefs->ssh_only || substitutor == NULL) {
        goto out;
    }
    
    _upstage();
    
    {
        // Copy over resources to RootFS.
        
        _progress(localize(@"Copying over resources to RootFS..."));
        
        _assert(chdir("/") == ERR_SUCCESS, localize(@"Unable to change working directory to RootFS."));
        
        // Uninstall RootLessJB if it is found to prevent conflicts with dpkg.
        _assert(uninstallRootLessJB(), localize(@"Unable to uninstall RootLessJB."));
        
        // Make sure we have an apt packages cache
        _assert(ensureAptPkgLists(), localize(@"Unable to extract apt package lists."));
        
        needSubstitutor = ( needStrap ||
                         (access(substitutor->startup_executable, F_OK) != ERR_SUCCESS) ||
                         !verifySums([NSString stringWithFormat:@"/var/lib/dpkg/info/%s.md5sums", substitutor->package_id], HASHTYPE_MD5)
                         );
        if (needSubstitutor) {
            LOG("We need %s.", substitutor->name);
            NSString *substitutorDeb = debForPkg(@(substitutor->package_id));
            _assert(substitutor != nil, localize(@"Unable to get deb for %s.", substitutor->name));
            if (pidOfProcess(substitutor->server_executable) == 0) {
                _assert(extractDeb(substitutorDeb, doInject), localize(@"Unable to extract %s.", substitutor->name));
            } else {
                skipSubstitutor = YES;
                LOG("%s is running, not extracting again for now.", substitutor->name);
            }
            [debsToInstall addObject:substitutorDeb];
        }
        
        NSMutableArray *resourcesPkgs = [NSMutableArray arrayWithArray:resolveDepsForPkg(@"jailbreak-resources", true)];
        _assert(resourcesPkgs.count != 0, localize(@"Unable to get resource packages."));
		if (strcmp(prefs->code_substitutor, K_SUBSTRATE) == 0) {
			[resourcesPkgs addObject:@"system-memory-reset-fix"];
		}
        
        NSMutableArray *pkgsToRepair = [NSMutableArray new];
        LOG("Resource Pkgs: \"%@\".", resourcesPkgs);
        for (NSString *pkg in resourcesPkgs) {
            // Ignore substitutor because we just handled that separately.
            // TODO: Make a better handling of pure virtual packages
            if ([pkg isEqualToString:@(substitutor->package_id)] || [pkg isEqualToString:@"firmware"] || [pkg isEqualToString:@"cy+cpu.arm64"] || [pkg isEqualToString:@"mobilesubstrate"])
                continue;
            if (verifySums([NSString stringWithFormat:@"/var/lib/dpkg/info/%@.md5sums", pkg], HASHTYPE_MD5)) {
                LOG("Pkg \"%@\" verified.", pkg);
            } else {
                LOG("Need to repair \"%@\".", pkg);
                if ([pkg isEqualToString:@"signing-certificate"]) {
                    // Hack to make sure it catches the Depends: version if it's already installed
                    [debsToInstall addObject:debForPkg(@"jailbreak-resources")];
                }
                [pkgsToRepair addObject:pkg];
            }
        }
        
        if (substitutor->shasums != NULL &&
            !verifySums(pathForResource(@(substitutor->shasums)), HASHTYPE_SHA1)) {
            if (![pkgsToRepair containsObject:@(substitutor->package_id)]) {
                [pkgsToRepair addObject:@(substitutor->package_id)];
            }
        }
		
        if (pkgsToRepair.count > 0) {
            LOG("(Re-)Extracting \"%@\".", pkgsToRepair);
            NSArray <NSString *> *debsToRepair = debsForPkgs(pkgsToRepair);
            _assert(debsToRepair.count == pkgsToRepair.count, localize(@"Unable to get debs for packages to repair."));
            _assert(extractDebs(debsToRepair, doInject), localize(@"Unable to repair packages."));
            [debsToInstall addObjectsFromArray:debsToRepair];
        }
        // Ensure ldid's symlink isn't missing
        // (it's created by update-alternatives which may not have been called yet)
        if (access("/usr/bin/ldid", F_OK) != ERR_SUCCESS) {
            _assert(access("/usr/libexec/ldid", F_OK) == ERR_SUCCESS, localize(@"Unable to access ldid."));
            _assert(ensure_symlink("../libexec/ldid", "/usr/bin/ldid"), localize(@"Unable to create symlink for ldid."));
        }
        
        // These don't need to lay around
        clean_file("/Library/LaunchDaemons/jailbreakd.plist");
        clean_file(_jailbreak_file("jailbreakd.plist"));
        clean_file(_jailbreak_file("amfid_payload.dylib"));
        clean_file(_jailbreak_file("libjailbreak.dylib"));
        
        LOG("Successfully copied over resources to RootFS.");
        _insert_status(localize(@"Copied over resources to RootFS.\n"));
    }
    
    _upstage();
    
    {
        // Repair filesystem.
        
        _progress(localize(@"Repairing filesystem..."));
        
        _assert(ensure_directory("/var/lib", root_pw->pw_uid, 0755), localize(@"Unable to repair state information directory."));
        
        // Make sure dpkg is not corrupted
        if (is_directory("/var/lib/dpkg")) {
            if (is_directory("/Library/dpkg")) {
                LOG("Removing /var/lib/dpkg...");
                _assert(clean_file("/var/lib/dpkg"), localize(@"Unable to clean old dpkg database."));
            } else {
                LOG("Moving /var/lib/dpkg to /Library/dpkg...");
                _assert([fileManager moveItemAtPath:@"/var/lib/dpkg" toPath:@"/Library/dpkg" error:nil], localize(@"Unable to restore dpkg database."));
            }
        }
        
        _assert(ensure_symlink("/Library/dpkg", "/var/lib/dpkg"), localize(@"Unable to symlink dpkg database."));
        _assert(ensure_directory("/Library/dpkg", root_pw->pw_uid, 0755), localize(@"Unable to repair dpkg database."));
        _assert(ensure_file("/var/lib/dpkg/status", root_pw->pw_uid, 0644), localize(@"Unable to repair dpkg status file."));
        _assert(ensure_file("/var/lib/dpkg/available", root_pw->pw_uid, 0644), localize(@"Unable to repair dpkg available file."));
        
        // Make sure firmware-sbin package is not corrupted.
        NSString *file = [NSString stringWithContentsOfFile:@"/var/lib/dpkg/info/firmware-sbin.list" encoding:NSUTF8StringEncoding error:nil];
        if ([file containsString:@"/sbin/fstyp"] || [file containsString:@"\n\n"]) {
            // This is not a stock file for iOS11+
            file = [file stringByReplacingOccurrencesOfString:@"/sbin/fstyp\n" withString:@""];
            file = [file stringByReplacingOccurrencesOfString:@"\n\n" withString:@"\n"];
            [file writeToFile:@"/var/lib/dpkg/info/firmware-sbin.list" atomically:YES encoding:NSUTF8StringEncoding error:nil];
        }
        
        // Make sure this is a symlink - usually handled by ncurses pre-inst
        _assert(ensure_symlink("/usr/lib", "/usr/lib/_ncurses"), localize(@"Unable to repair ncurses."));
        
        // This needs to be there for substitutor to work properly
        _assert(ensure_directory("/Library/Caches", root_pw->pw_uid, S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO), localize(@"Unable to repair caches directory for %s.", substitutor->name));
        _assert(ensure_directory("/Library/Caches/Swap", root_pw->pw_uid, S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO), localize(@"Unable to repair caches directory for %s.", substitutor->name));
        LOG("Successfully repaired filesystem.");
        
        _insert_status(localize(@"Repaired Filesystem.\n"));
		
		ensure_file("/usr/sbin/wirelessproxd", 0, 0755);
		ensure_file("/.mount_rw", 0, 0644);
    }
    
    _upstage();
    
    {
        // Load substitutor
        
        // Configure substitutor.
        _progress(localize(@"Configuring %s...", substitutor->name));
        if (strcmp(substitutor->name, K_SUBSTRATE) == 0) {
            if (prefs->enable_get_task_allow) {
                SETOPT(GET_TASK_ALLOW);
            } else {
                UNSETOPT(GET_TASK_ALLOW);
            }
            if (prefs->set_cs_debugged) {
                SETOPT(CS_DEBUGGED);
            } else {
                UNSETOPT(CS_DEBUGGED);
            }
			SETFEAT(KERNEL_EXECUTION);
        } else if (strcmp(substitutor->name, K_SUBSTITUTE) == 0) {
            SETOPT(GET_TASK_ALLOW);
            SETOPT(CS_DEBUGGED);
            SETOPT(SKIP_SANDBOX);
            SETOPT(SKIP_JETSAM_ENTITLEMENT);
            SETOPT(SKIP_SUBSTRATE);
            SETOPT(SET_CHILDDRAINED);
            SETOPT(SET_HAS_CORPSE_FOOTPRINT);
			SETFEAT(USERSPACE_REBOOT);
        }
        if (prefs->load_tweaks) {
            clean_file(substitutor->loader_killswitch);
        } else {
            _assert(create_file(substitutor->loader_killswitch, root_pw->pw_uid, 644), localize(@"Unable to disable %s's loader.", substitutor->name));
        }
        LOG("Successfully configured %s.", substitutor->name);
        
        // Run substitutor
        _progress(localize(@"Starting %s...", substitutor->name));
        if (access("/Library/substrate", F_OK) == ERR_SUCCESS &&
            is_directory("/Library/substrate") &&
            access(substitutor->bootstrap_tools, F_OK) == ERR_SUCCESS &&
            is_symlink(substitutor->bootstrap_tools)) {
            _assert(clean_file(substitutor->bootstrap_tools), localize(@"Unable to clean old %s bootstrap tools directory.", substitutor->name));
            _assert([fileManager moveItemAtPath:@"/Library/substrate" toPath:@(substitutor->bootstrap_tools) error:nil], localize(@"Unable to move %s bootstrap tools directory.", substitutor->name));
        }
        const char *argv[] = { substitutor->startup_executable, NULL, };
        rv = runCommandv(substitutor->startup_executable, 0, argv, ^(pid_t pid) {
            if (MACH_PORT_VALID(real_amfid_port)) {
                host_set_amfid_port(myHost, real_amfid_port);
                real_amfid_port = MACH_PORT_NULL;
            }
            if (MACH_PORT_VALID(fake_amfid_port)) {
                mach_port_destroy(mach_task_self(), fake_amfid_port);
                fake_amfid_port = MACH_PORT_NULL;
            }
            if (strcmp(substitutor->name, K_SUBSTRATE) != 0 && amfidPid > 1) {
                kill(amfidPid, SIGKILL);
            }
			
        }, true);
        rv = WEXITSTATUS(rv);
        _assert(rv == ERR_SUCCESS, localize(@"Unable to %@ %s.", skipSubstitutor ? @"restart" : @"start", substitutor->name));
        LOG("Successfully started %s.", substitutor->name);
		if (strcmp(substitutor->name, K_SUBSTITUTE) == 0) {
			shouldRebootUserspace = true;
		}
        _insert_status(localize(@"Loaded %s.\n", substitutor->name));
        setenv("_MSSafeMode", "1", 1);
		if (strcmp(substitutor->name, K_SUBSTITUTE) == 0) {
			mach_port_t port = MACH_PORT_NULL;
			rv = host_get_kextd_port(myHost, &port);
			_assert(rv == KERN_SUCCESS, localize(@"Unable to get kextd port: %s", mach_error_string(rv)));
			rv = task_set_special_port(mach_task_self(), TASK_SEATBELT_PORT, port);
			_assert(rv == KERN_SUCCESS || rv == KERN_NO_ACCESS, localize(@"Unable to set our special port: %s", mach_error_string(rv)));
		}
        if (substitutor->inserter != NULL) do {
            void *handle = dlopen(substitutor->inserter, RTLD_LAZY);
            if (handle != NULL) break;
            char *error = dlerror();
            if (error == NULL) break;
            if (strstr(error, "mapping process and mapped file (non-platform) have different Team IDs") == NULL) break;
            int fd = open(substitutor->inserter, O_RDONLY);
            if (fd == -1) break;
            kptr_t vnode = find_vnode_with_fd(myProcAddr, fd);
            close(fd);
            if (!KERN_POINTER_VALID(vnode)) break;
            kptr_t ubcinfo = ReadKernel64(vnode + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
            if (!KERN_POINTER_VALID(ubcinfo)) break;
            kptr_t csblobs = ReadKernel64(ubcinfo + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
            if (!KERN_POINTER_VALID(csblobs)) break;
            uint32_t csb_platform_binary = ReadKernel32(csblobs + offsetof(struct cs_blob, csb_platform_binary));
            kptr_t csb_teamid = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_teamid));
            WriteKernel32(csblobs + offsetof(struct cs_blob, csb_platform_binary), true);
            WriteKernel64(csblobs + offsetof(struct cs_blob, csb_teamid), KPTR_NULL);
            handle = dlopen(substitutor->inserter, RTLD_LAZY);
            _assert(handle != NULL, localize(@"Unable to load %s", substitutor->inserter));
        } while (false);
    }
	
    _upstage();
    
    {
        // Extract bootstrap.
        _progress(localize(@"Extracting bootstrap..."));
        
        if (!pkgIsConfigured("xz")) {
            removePkg("lzma", true);
            extractDebsForPkg(@"lzma", debsToInstall, false, doInject);
        }
        
        if (pkgIsInstalled("openssl") && compareInstalledVersion("openssl", "lt", "1.0.2q")) {
            removePkg("openssl", true);
        }
        
        if (strcmp(substitutor->name, K_SUBSTITUTE) == 0 &&
            pkgIsInstalled("signing-certificate")) {
            removePkg("signing-certificate", true);
        }
        
        // Test dpkg
        if (!pkgIsConfigured("dpkg")) {
            LOG("Extracting dpkg...");
            _assert(extractDebsForPkg(@"dpkg", debsToInstall, false, doInject), localize(@"Unable to extract dpkg."));
            NSString *dpkg_deb = debForPkg(@"dpkg");
            _assert(installDeb(dpkg_deb.UTF8String, true), localize(@"Unable to install deb for dpkg."));
            [debsToInstall removeObject:dpkg_deb];
        }
        
        if (needStrap || !pkgIsConfigured("firmware")) {
            if (access("/usr/libexec/cydia/firmware.sh", F_OK) != ERR_SUCCESS || !pkgIsConfigured("cydia")) {
                LOG("Extracting Cydia...");
                NSArray <NSString *> *fwDebs = debsForPkgs(@[@"cydia", @"cydia-lproj", @"darwintools", @"uikittools", @"system-cmds"]);
                _assert(fwDebs != nil, localize(@"Unable to get firmware debs."));
                _assert(installDebs(fwDebs, true, false), localize(@"Unable to install firmware debs."));
            }
            rv = _system("/usr/libexec/cydia/firmware.sh");
            _assert(WEXITSTATUS(rv) == 0, localize(@"Unable to create virtual dependencies."));
        }
        
        // Dpkg better work now
        
        if (pkgIsInstalled("science.xnu.undecimus.resources")) {
            LOG("Removing old resources...");
            _assert(removePkg("science.xnu.undecimus.resources", true), localize(@"Unable to remove old resources."));
        }
        
        if (pkgIsInstalled("jailbreak-resources-with-cert")) {
            LOG("Removing resources-with-cert...");
            _assert(removePkg("jailbreak-resources-with-cert", true), localize(@"Unable to remove old-development resources."));
        }
        
        if ((pkgIsInstalled("apt7") && compareInstalledVersion("apt7", "lt", "1:0")) ||
            (pkgIsInstalled("apt7-lib") && compareInstalledVersion("apt7-lib", "lt", "1:0")) ||
            (pkgIsInstalled("apt7-key") && compareInstalledVersion("apt7-key", "lt", "1:0"))
            ) {
            LOG("Installing newer version of apt7");
            NSArray <NSString *> *apt7debs = debsForPkgs(@[@"apt7", @"apt7-key", @"apt7-lib"]);
            _assert(apt7debs != nil && apt7debs.count == 3, localize(@"Unable to get debs for apt7."));
            for (NSString *deb in apt7debs) {
                if (![debsToInstall containsObject:deb]) {
                    [debsToInstall addObject:deb];
                }
            }
        }
        
        if (debsToInstall.count > 0) {
            LOG("Installing manually exctracted debs...");
            _assert(installDebs(debsToInstall, true, true), localize(@"Unable to install manually extracted debs."));
        }
        
        _assert(ensure_directory("/etc/apt/undecimus", root_pw->pw_uid, 0755), localize(@"Unable to create local repo."));
        clean_file("/etc/apt/sources.list.d/undecimus.list");
        char const *listPath = "/etc/apt/undecimus/undecimus.list";
        NSString *listContents = @"deb file:///var/lib/undecimus/apt ./\n";
        NSString *existingList = [NSString stringWithContentsOfFile:@(listPath) encoding:NSUTF8StringEncoding error:nil];
        if (![listContents isEqualToString:existingList]) {
            clean_file(listPath);
            [listContents writeToFile:@(listPath) atomically:NO encoding:NSUTF8StringEncoding error:nil];
        }
        init_file(listPath, root_pw->pw_uid, 0644);
        const char *prefsPath = "/etc/apt/undecimus/preferences";
        NSString *prefsContents = @"Package: *\nPin: release o=Undecimus\nPin-Priority: 1001\n";
        NSString *existingPrefs = [NSString stringWithContentsOfFile:@(prefsPath) encoding:NSUTF8StringEncoding error:nil];
        if (![prefsContents isEqualToString:existingPrefs]) {
            clean_file(prefsPath);
            [prefsContents writeToFile:@(prefsPath) atomically:NO encoding:NSUTF8StringEncoding error:nil];
        }
        init_file(prefsPath, root_pw->pw_uid, 0644);
        NSString *repoPath = pathForResource(@"apt");
        _assert(repoPath != nil, localize(@"Unable to get repo path."));
        ensure_directory("/var/lib/undecimus", root_pw->pw_uid, 0755);
        ensure_symlink([repoPath UTF8String], "/var/lib/undecimus/apt");
        if (!pkgIsConfigured("apt1.4") || !aptUpdate()) {
            NSArray *aptNeeded = resolveDepsForPkg(@"apt1.4", false);
            _assert(aptNeeded != nil && aptNeeded.count > 0, localize(@"Unable to resolve dependencies for apt."));
            NSArray <NSString *> *aptDebs = debsForPkgs(aptNeeded);
            _assert(installDebs(aptDebs, true, true), localize(@"Unable to install debs for apt."));
            _assert(aptUpdate(), localize(@"Unable to update apt package index."));
            _assert(aptRepair(), localize(@"Unable to repair system."));
        }
        
        // Workaround for what appears to be an apt bug
        ensure_symlink("/var/lib/undecimus/apt/./Packages", "/var/lib/apt/lists/_var_lib_undecimus_apt_._Packages");
        
        if (!aptInstall(@[@"-f"])) {
            _assert(aptRepair(), localize(@"Unable to repair system."));
        }
        
        // Dpkg and apt both work now
        
        if (needStrap) {
            prefs->run_uicache = true;
            _sync_prefs();
        }
        // Now that things are running, let's install the deb for the files we just extracted
        if (needSubstitutor) {
            if (strcmp(prefs->code_substitutor, K_SUBSTRATE) == 0) {
                if (pkgIsInstalled("com.ex.substitute")) {
                    _assert(removePkg("com.ex.substitute", true), localize(@"Unable to remove Substitute."));
                }
                if (pkgIsInstalled("science.xnu.substituted")) {
                    _assert(removePkg("science.xnu.substituted", true), localize(@"Unable to remove Substitute (Monolithic)."));
                }
            } else if (strcmp(prefs->code_substitutor, K_SUBSTITUTE) == 0) {
                if (pkgIsInstalled("mobilesubstrate")) {
                    _assert(removePkg("mobilesubstrate", true), localize(@"Unable to remove Substrate."));
                }
            }
            _assert(aptInstall(@[@(substitutor->package_id)]), localize(@"Unable to install %s.", substitutor->name));
        }
        
        NSData *file_data = [[NSString stringWithFormat:@"%f\n", kCFCoreFoundationVersionNumber] dataUsingEncoding:NSUTF8StringEncoding];
        if (![[NSData dataWithContentsOfFile:@"/.installed_unc0ver"] isEqual:file_data]) {
            _assert(clean_file("/.installed_unc0ver"), localize(@"Unable to clean old bootstrap marker file."));
            _assert(create_file_data("/.installed_unc0ver", root_pw->pw_uid, 0644, file_data), localize(@"Unable to create bootstrap marker file."));
        }
        
        _assert(ensure_file("/.cydia_no_stash", root_pw->pw_uid, 0644), localize(@"Unable to disable stashing."));
        
        // Make sure everything's at least as new as what we bundled
        rv = system("dpkg --configure -a");
        _assert(WEXITSTATUS(rv) == ERR_SUCCESS, localize(@"Unable to configure installed packages."));
        _assert(aptUpgrade(), localize(@"Unable to upgrade apt packages."));
        
        clean_file(_jailbreak_file("tar"));
        clean_file(_jailbreak_file("lzma"));
        clean_file(_jailbreak_file("substrate.tar.lzma"));
        clean_file("/electra");
        clean_file("/chimera");
        clean_file("/.bootstrapped_electra");
        clean_file([NSString stringWithFormat:@"/etc/.installed-chimera-%@", getECID()].UTF8String);
        clean_file("/usr/lib/libjailbreak.dylib");
        
        LOG("Successfully extracted bootstrap.");
        
        _insert_status(localize(@"Extracted Bootstrap.\n"));
    }
    
    _upstage();
    
    {
        // Fix storage preferences.
        
        _progress(localize(@"Fixing storage preferences..."));
        if (access("/System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/softwareupdated", F_OK) == ERR_SUCCESS) {
            _assert(rename("/System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/softwareupdated", "/System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/Support/softwareupdated") == ERR_SUCCESS, localize(@"Unable to to fix path for softwareupdated."));
        }
        if (access("/System/Library/PrivateFrameworks/SoftwareUpdateServices.framework/softwareupdateservicesd", F_OK) == ERR_SUCCESS) {
            _assert(rename("/System/Library/PrivateFrameworks/SoftwareUpdateServices.framework/softwareupdateservicesd", "/System/Library/PrivateFrameworks/SoftwareUpdateServices.framework/Support/softwareupdateservicesd") == ERR_SUCCESS, localize(@"Unable to fix path for softwareupdateservicesd."));
        }
        if (access("/System/Library/com.apple.mobile.softwareupdated.plist", F_OK) == ERR_SUCCESS) {
            _assert(rename("/System/Library/com.apple.mobile.softwareupdated.plist", "/System/Library/LaunchDaemons/com.apple.mobile.softwareupdated.plist") == ERR_SUCCESS, localize(@"Unable to fix path for softwareupdated launch daemon."));
            _assert(runCommand("/bin/launchctl", "load", "/System/Library/LaunchDaemons/com.apple.mobile.softwareupdated.plist", NULL) == ERR_SUCCESS, localize(@"Unable to load softwareupdated launch daemon."));
        }
        if (access("/System/Library/com.apple.softwareupdateservicesd.plist", F_OK) == ERR_SUCCESS) {
            _assert(rename("/System/Library/com.apple.softwareupdateservicesd.plist", "/System/Library/LaunchDaemons/com.apple.softwareupdateservicesd.plist") == ERR_SUCCESS, localize(@"Unable to fix path for softwareupdateservicesd launch daemon."));
            _assert(runCommand("/bin/launchctl", "load", "/System/Library/LaunchDaemons/com.apple.softwareupdateservicesd.plist", NULL) == ERR_SUCCESS, localize(@"Unable to load softwareupdateservicesd launch daemon."));
        }
        LOG("Successfully fixed storage preferences.");
        _insert_status(localize(@"Fixed Storage Preferences.\n"));
    }
    
    _upstage();
    
    {
        char *targettype = sysctlWithName("hw.targettype");
        _assert(targettype != NULL, localize(@"Unable to get hardware targettype."));
        NSString *jetsamFile = [NSString stringWithFormat:@"/System/Library/LaunchDaemons/com.apple.jetsamproperties.%s.plist", targettype];
        SafeFreeNULL(targettype);
        
        if (prefs->increase_memory_limit) {
            // Increase memory limit.
            
            _progress(localize(@"Increasing memory limit..."));
            _assert(modifyPlist(jetsamFile, ^(id plist) {
                plist[@"Version4"][@"System"][@"Override"][@"Global"][@"UserHighWaterMark"] = @([plist[@"Version4"][@"PListDevice"][@"MemoryCapacity"] integerValue]);
            }), localize(@"Unable to update Jetsam plist to increase memory limit."));
            LOG("Successfully increased memory limit.");
            _insert_status(localize(@"Increased Memory Limit.\n"));
        } else {
            // Restore memory limit.
            
            _progress(localize(@"Restoring memory limit..."));
            _assert(modifyPlist(jetsamFile, ^(id plist) {
				[plist[@"Version4"][@"System"][@"Override"][@"Global"] removeObjectForKey:@"UserHighWaterMark"];
            }), localize(@"Unable to update Jetsam plist to restore memory limit."));
            LOG("Successfully restored memory limit.");
            _insert_status(localize(@"Restored Memory Limit.\n"));
        }
    }
    
    _upstage();
    
    {
        if (prefs->install_openssh) {
            // Install OpenSSH.
            _progress(localize(@"Installing OpenSSH..."));
            _assert(aptInstall(@[@"openssh"]), localize(@"Unable to install OpenSSH."));
            prefs->install_openssh = false;
            _sync_prefs();
            LOG("Successfully installed OpenSSH.");
            
            _insert_status(localize(@"Installed OpenSSH.\n"));
        }
    }
    
    _upstage();
    
    {
        if (pkgIsInstalled("cydia-gui")) {
            // Remove Electra's Cydia.
            _progress(localize(@"Removing Cydia Dummy Package..."));
            _assert(removePkg("cydia-gui", true), localize(@"Unable to remove Cydia Dummy Package."));
            prefs->install_cydia = true;
            prefs->run_uicache = true;
            _sync_prefs();
            LOG("Successfully removed Cydia Dummy Package.");
            
            _insert_status(localize(@"Removed Cydia Dummy Package.\n"));
        }
        deduplicateSillySources();
        if (pkgIsInstalled("cydia-upgrade-helper")) {
            // Remove Electra's Cydia Upgrade Helper.
            _progress(localize(@"Removing Electra's Cydia Upgrade Helper..."));
            _assert(removePkg("cydia-upgrade-helper", true), localize(@"Unable to remove Electra's Cydia Upgrade Helper."));
            prefs->install_cydia = true;
            prefs->run_uicache = true;
            _sync_prefs();
            LOG("Successfully removed Electra's Cydia Upgrade Helper.");
        }
        if (access("/etc/apt/sources.list.d/electra.list", F_OK) == ERR_SUCCESS ||
            access("/etc/apt/sources.list.d/chimera.sources", F_OK) == ERR_SUCCESS) {
            prefs->install_cydia = true;
            prefs->run_uicache = true;
            _sync_prefs();
        }
        // Unblock Saurik's repo if it is blocked.
        unblockDomainWithName("apt.saurik.com");
        if (prefs->install_cydia) {
            // Install Cydia.
            
            _progress(localize(@"Installing Cydia..."));
            NSString *cydiaVer = versionOfPkg(@"cydia");
            _assert(cydiaVer != nil, localize(@"Unable to get Cydia version."));
            _assert(aptInstall(@[@"--reinstall", [@"cydia" stringByAppendingFormat:@"=%@", cydiaVer]]), localize(@"Unable to reinstall Cydia."));
            prefs->install_cydia = false;
            prefs->run_uicache = true;
            _sync_prefs();
            LOG("Successfully installed Cydia.");
            
            _insert_status(localize(@"Installed Cydia.\n"));
        }
    }
    
    _upstage();
    
    {
        if (prefs->load_daemons) {
            // Load Daemons.
            
            _progress(localize(@"Loading Daemons..."));
			if (substitutor->daemon_killswitch == NULL) {
				system("launchctl load /Library/LaunchDaemons");
				// Substitutor is already running, no need to run it again
				systemf("for file in /etc/rc.d/*; do "
						"if [[ -x \"$file\" && \"$file\" != \"%s\" ]]; then "
						"\"$file\";"
						"fi;"
						"done", substitutor->run_command);
			} else {
				clean_file(substitutor->daemon_killswitch);
			}
            LOG("Successfully loaded Daemons.");
            
            _insert_status(localize(@"Loaded Daemons.\n"));
		} else {
			if (substitutor->daemon_killswitch != NULL) {
				ensure_file(substitutor->daemon_killswitch, root_pw->pw_uid, 0644);
			}
		}
    }
    
    _upstage();
    
    {
        if (prefs->reset_cydia_cache) {
            // Reset Cydia cache.
            
            _progress(localize(@"Resetting Cydia cache..."));
            _assert(clean_file("/var/mobile/Library/Cydia"), localize(@"Unable to clean Cydia's directory."));
            _assert(clean_file("/var/mobile/Library/Caches/com.saurik.Cydia"), localize(@"Unable to clean Cydia's cache directory."));
            prefs->reset_cydia_cache = false;
            _sync_prefs();
            LOG("Successfully reset Cydia cache.");
            
            _insert_status(localize(@"Reset Cydia Cache.\n"));
        }
    }
    
    _upstage();
    
    {
        if (prefs->run_uicache || !cydiaIsInstalled()) {
            // Run uicache.
            
            _progress(localize(@"Refreshing icon cache..."));
            _assert(runCommand("/usr/bin/uicache", NULL) >= 0, localize(@"Unable to refresh icon cache."));
            prefs->run_uicache = false;
            _sync_prefs();
            LOG("Successfully ran uicache.");
            _insert_status(localize(@"Ran uicache.\n"));
        }
	}
    
    _upstage();
	
    if (!shouldRebootUserspace) {
		if (prefs->load_tweaks) {
            // Load Tweaks.
            
            _progress(localize(@"Loading Tweaks..."));
            NSMutableString *waitCommand = [NSMutableString new];
            [waitCommand appendFormat:@"while [[ ! -f %s ]]; do :; done;", success_file];
            if (!prefs->auto_respring) {
                [waitCommand appendFormat:@"while ps -p %d; do :; done;", my_pid];
            }
            if (prefs->reload_system_daemons && !needStrap) {
                rv = systemf("nohup bash -c \""
                             "%s"
                             "launchctl unload /System/Library/LaunchDaemons/com.apple.backboardd.plist && "
                             "ldrestart ;"
                             "launchctl load /System/Library/LaunchDaemons/com.apple.backboardd.plist"
                             "\" >/dev/null 2>&1 &", waitCommand.UTF8String);
            } else {
                rv = systemf("nohup bash -c \""
                             "%s"
                             "launchctl stop com.apple.mDNSResponder ;"
                             "sbreload"
                             "\" >/dev/null 2>&1 &", waitCommand.UTF8String);
            }
            _assert(WEXITSTATUS(rv) == ERR_SUCCESS, localize(@"Unable to load tweaks."));
            LOG("Successfully loaded Tweaks.");
            
            _insert_status(localize(@"Loaded Tweaks.\n"));
        }
    }
    
out:;
#undef _sync_prefs
#undef _write_test_file
#undef _inject_trust_cache
    stage = maxStage;
    _update_stage();
    _progress(localize(@"Deinitializing jailbreak..."));
    if (false) {
    fail:;
        __assertion_failure = true;
    } else {
        __assertion_failure = -1;
    }
    if (MACH_PORT_VALID(real_amfid_port)) {
        _assert(host_set_amfid_port(myHost, real_amfid_port) == KERN_SUCCESS, localize(@"Unable to restore amfid port."));
        real_amfid_port = MACH_PORT_NULL;
    }
    if (MACH_PORT_VALID(fake_amfid_port)) {
        _assert(mach_port_destroy(mach_task_self(), fake_amfid_port) == KERN_SUCCESS, localize(@"Unable to destroy fake amfid port."));
        fake_amfid_port = MACH_PORT_NULL;
    }
    if (KERN_POINTER_VALID(myCSBlobsAddr)) {
        kptr_t currentCSBlob = myCSBlobsAddr;
        while (KERN_POINTER_VALID(currentCSBlob)) {
            wkbuffer(currentCSBlob + offsetof(struct cs_blob, csb_cdhash), real_cdhash, CS_CDHASH_LEN);
            currentCSBlob = ReadKernel64(currentCSBlob + offsetof(struct cs_blob, csb_next));
        }
    }
	if (shouldRebootUserspace) {
		// Set userspace to reboot.
		
		_progress(localize(@"Setting userspace to reboot..."));
		NSMutableString *waitCommand = [NSMutableString new];
		[waitCommand appendFormat:@"while [[ ! -f %s ]]; do :; done;", success_file];
		if (!prefs->auto_respring) {
			[waitCommand appendFormat:@"while ps -p %d; do :; done;", my_pid];
		}
		rv = systemf("nohup bash -c \""
					 "%s"
					 "/sbin/launchd ;"
					 "launchctl reboot userspace"
					 "\" >/dev/null 2>&1 &", waitCommand.UTF8String);
		_assert(WEXITSTATUS(rv) == ERR_SUCCESS, localize(@"Unable to set userspace to reboot."));
		LOG("Successfully set userspace to reboot.");
	}
    LOG("Unplatformizing...");
    _assert(set_platform_binary(myProcAddr, false), localize(@"Unable to make my task a non-platform task."));
    _assert(set_cs_platform_binary(myProcAddr, false), localize(@"Unable to make my codesign blob a non-platform blob."));
    LOG("Sandboxing...");
	WriteKernel64(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL), myLabelAddr);
	WriteKernel32(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_UID), mobile_pw->pw_uid);
	WriteKernel32(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_RUID), mobile_pw->pw_uid);
    LOG("Downgrading host port...");
    _assert(setuid(my_uid) == ERR_SUCCESS, localize(@"Unable to set user id."));
    _assert(getuid() == my_uid, localize(@"Unable to verify user id."));
    LOG("Deallocating ports...");
    _assert(mach_port_deallocate(mach_task_self(), myHost) == KERN_SUCCESS, localize(@"Unable to deallocate new host port."));
    myHost = HOST_NULL;
    _assert(mach_port_deallocate(mach_task_self(), myOriginalHost) == KERN_SUCCESS, localize(@"Unable to deallocate my original host port."));
    myOriginalHost = HOST_NULL;
    LOG("Deallocating memory...");
    SafeFreeNULL(real_cdhash);
    SafeFreeNULL(fake_cdhash);
	_assert(create_file(success_file, mobile_pw->pw_uid, 644), localize(@"Unable to create success file."));
#undef FAIL_LABEL
    if (__assertion_failure == true) {
        __assert_terminate();
    }
    _insert_status(([NSString stringWithFormat:@"\nRead %lld bytes from kernel memory\nWrote %lld bytes to kernel memory\n", kreads, kwrites]));
    time_t run_time = time(NULL) - start_time;
    _insert_status(([NSString stringWithFormat:@"\nJailbroke in %ld second%s\n", run_time, run_time!=1?"s":""]));
    _status(localize(@"Jailbroken"), false, false);
    bool forceRespring = (strcmp(prefs->kernel_exploit, K_MACH_SWAP) == 0);
    forceRespring |= (strcmp(prefs->kernel_exploit, K_MACH_SWAP_2) == 0);
    forceRespring &= (!usedPersistedKernelTaskPort);
    forceRespring &= (!prefs->load_tweaks);
    bool willRespring = (forceRespring);
    willRespring |= (prefs->load_tweaks && !prefs->ssh_only);
    release_prefs(&prefs);
	NSString *finalAction;
	if (shouldRebootUserspace) {
		finalAction = @"The device will now reboot jailbroken.";
	} else if (willRespring) {
		finalAction = @"The device will now respring.";
	} else {
		finalAction = @"The app will now exit.";
	}
	
    showAlert(@"Jailbreak Completed", [NSString stringWithFormat:@"%@\n\n%@\n%@", localize(@"Jailbreak Completed with Status:"), status, localize(finalAction)], true, false);
    if (sharedController.canExit) {
        if (forceRespring) {
            WriteKernel64(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL), ReadKernel64(kernelCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL)));
            WriteKernel64(myCredAddr + koffset(KSTRUCT_OFFSET_UCRED_CR_UID), 0);
            _assert(restartSpringBoard(), localize(@"Unable to restart SpringBoard."));
        } else {
            exit(EXIT_SUCCESS);
            _assert(false, localize(@"Unable to exit."));
        }
    }
    sharedController.canExit = YES;
#undef insertstatus
}


// Don't move this - it is at the bottom so that it will list the total number of _upstages
int maxStage = __COUNTER__ - 1;
