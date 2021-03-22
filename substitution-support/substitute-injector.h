#ifndef substitute_injector_h
#define substitute_injector_h

#include <common.h>

bool SubHookProcess(pid_t pid, const char *library);
bool MSHookProcess(pid_t pid, const char *library);

#endif
