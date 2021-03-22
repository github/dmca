//
//  substitutors.h
//  Undecimus
//
//  Created by Pwn20wnd on 7/10/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#ifndef substitutors_h
#define substitutors_h

#include <stdio.h>
#include <stdbool.h>
#include <mach/machine.h>

#define K_SUBSTRATE "Cydia Substrate"
#define K_SUBSTITUTE "Substitute"

#define SUBSTITUTOR_SUPPORT_INJECTION (1<<0)

typedef struct {
    const char *name;
    const char *package_id;
    const char *startup_executable;
    const char *server_executable;
    const char *run_command;
    const char *loader_killswitch;
	const char *daemon_killswitch;
    const char *bootstrap_tools;
    const char *inserter;
    const char *shasums;
    char **resources;
    float reliability;
    const char *min_kernel_version;
    const char *max_kernel_version;
    int requirements;
    int substitutor_support;
} code_substitutor_t;

void iterate_substitutors(void (^handler)(code_substitutor_t *substitutor));
void iterate_evaluated_substitutors(void (^handler)(code_substitutor_t *substitutor));
code_substitutor_t *get_substitutor_by_name(const char *name);
bool evaluate_substitutor(code_substitutor_t *substitutor);
bool evaluate_substitutor_by_name(const char *name);
code_substitutor_t *get_best_substitutor(int support);
const char *get_best_substitutor_by_name(int support);
bool has_substitutor_support(int support);

#endif /* substitutors_h */
