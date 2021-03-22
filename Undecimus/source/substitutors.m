//
//  substitutors.c
//  Undecimus
//
//  Created by Pwn20wnd on 7/10/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#include "substitutors.h"
#include <common.h>
#include "utils.h"
#include <sys/sysctl.h>

#define REQ_NO_PAC (1<<0)
#define REQ_PAC (1<<1)

#define IS_NO_PAC (get_cpu_subtype() != CPU_SUBTYPE_ARM64E)

code_substitutor_t *substitutors[] = {
    &(code_substitutor_t)
    {
        .name = K_SUBSTRATE,
        .package_id = "mobilesubstrate",
        .startup_executable = "/usr/libexec/substrate",
        .server_executable = "/usr/libexec/substrated",
        .run_command = "/etc/rc.d/substrate",
        .loader_killswitch = "/var/tmp/.substrated_disable_loader",
		.daemon_killswitch = NULL,
        .bootstrap_tools = "/usr/lib/substrate",
        .inserter = NULL,
        .shasums = NULL,
        .resources = ARRAY(char *, "/usr/libexec/substrate", "/usr/libexec/substrated", NULL),
        .reliability = 95.00,
        .min_kernel_version = "4397.0.0.2.4~1",
        .max_kernel_version = "4999.00",
        .requirements = REQ_NO_PAC,
        .substitutor_support = SUBSTITUTOR_SUPPORT_INJECTION,
    },
    &(code_substitutor_t)
    {
        .name = K_SUBSTITUTE,
        .package_id = "science.xnu.substituted",
        .startup_executable = "/usr/libexec/substitute",
        .server_executable = "/usr/libexec/substituted",
        .run_command = "/etc/rc.d/substitute",
        .loader_killswitch = "/var/tmp/.substitute_disable_loader",
		.daemon_killswitch = "/var/tmp/.substitute_disable_daemons",
        .bootstrap_tools = "/usr/lib/substitute",
        .inserter = "/usr/lib/substitute-inserter.dylib",
        .shasums = "substitute-resources.txt",
        .resources = ARRAY(char *, "/usr/libexec/substitute", NULL),
        .reliability = 90.00,
        .min_kernel_version = "4903.200.199.13.1~1",
        .max_kernel_version = "6153.60.66~39",
		.requirements = 0,
        .substitutor_support = SUBSTITUTOR_SUPPORT_INJECTION,
    },
    NULL
};

void iterate_substitutors(void (^handler)(code_substitutor_t *substitutor)) {
    init_function();
    assert(handler != NULL);
    for (code_substitutor_t **substitutor = substitutors; *substitutor; substitutor++) {
        handler(*substitutor);
    }
}

void iterate_evaluated_substitutors(void (^handler)(code_substitutor_t *substitutor)) {
    init_function();
    assert(handler != NULL);
    iterate_substitutors(^(code_substitutor_t *substitutor) {
        if (!evaluate_substitutor(substitutor)) {
            return;
        }
        handler(substitutor);
    });
}

code_substitutor_t *get_substitutor_by_name(const char *name) {
    init_function();
    if (name == NULL) return NULL;
    __block code_substitutor_t *found_substitutor = NULL;
    iterate_substitutors(^(code_substitutor_t *substitutor) {
        if (strcmp(substitutor->name, name) == 0) {
            found_substitutor = substitutor;
        }
    });
    return found_substitutor;
}

bool evaluate_substitutor(code_substitutor_t *substitutor) {
    init_function();
    if (substitutor == NULL) return false;
    NSString *kernelBuildVersion = getKernelBuildVersion();
    assert(kernelBuildVersion != nil);
    if (substitutor->requirements & REQ_NO_PAC && !IS_NO_PAC) return false;
    if (substitutor->requirements & REQ_PAC && IS_NO_PAC) return false;
    if ([kernelBuildVersion compare:@(substitutor->min_kernel_version) options:NSNumericSearch] == NSOrderedAscending) return false;
    if ([kernelBuildVersion compare:@(substitutor->max_kernel_version) options:NSNumericSearch] == NSOrderedDescending) return false;
    return true;
}

bool evaluate_substitutor_by_name(const char *name) {
    init_function();
    code_substitutor_t *substitutor = get_substitutor_by_name(name);
    return evaluate_substitutor(substitutor);
}

code_substitutor_t *get_best_substitutor(int support) {
    init_function();
    __block code_substitutor_t *best_substitutor = NULL;
    iterate_evaluated_substitutors(^(code_substitutor_t *substitutor) {
        if (!(substitutor->substitutor_support & support)) {
            return;
        }
        if (best_substitutor == NULL || substitutor->reliability > best_substitutor->reliability) {
            best_substitutor = substitutor;
        }
    });
    return best_substitutor;
}

const char *get_best_substitutor_by_name(int support) {
    init_function();
    code_substitutor_t *substitutor = get_best_substitutor(support);
    return substitutor != NULL ? substitutor->name : NULL;
}

bool has_substitutor_support(int support) {
    init_function();
    __block bool supported = false;
    iterate_evaluated_substitutors(^(code_substitutor_t *substitutor) {
        if (!(substitutor->substitutor_support & support)) return;
        supported = true;
    });
    return supported;
}
