#ifndef COMMON_H
#define COMMON_H

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#else
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <mach-o/loader.h>
#include <mach/error.h>
#include <sys/sysctl.h>
#include <sys/syscall.h>
#include <codesign.h>
#include <syslog.h>

#if 0

#ifdef __cplusplus
extern "C"
#endif
char ***_NSGetArgv(void);

//#define common_log(fmt, ...) syslog(LOG_EMERG, "[Substitution] " fmt, ##__VA_ARGS__)

#define common_log(fmt, ...) do { \
	if (access("/var/mobile/Library/Preferences/substitute-persistence.log", F_OK) == 0 && \
            access("/var/mobile/Library/Preferences/substitute-persistence.log", W_OK) != 0) break; \
	FILE *log_file = fopen("/var/mobile/Library/Preferences/substitute-persistence.log", "a"); \
	if (log_file == NULL) break; \
	time_t curtime = time(NULL); \
	time(&curtime); \
	char **procName = *_NSGetArgv(); \
	if (procName) fprintf(log_file, "%ld %s: " fmt "\n", curtime, *procName, ##__VA_ARGS__); \
	else fprintf(log_file, "%ld %d: " fmt "\n", curtime, getpid(), ##__VA_ARGS__); \
	fclose(log_file); \
} while (false)

#else
#define common_log(fmt, ...) do { } while (false)
#endif
#define error_log(fmt, ...) common_log("ERROR: " fmt, ##__VA_ARGS__)
#define info_log(fmt, ...) common_log("INFO: " fmt, ##__VA_ARGS__)
#define debug_log(fmt, ...) common_log("DEBUG: " fmt, ##__VA_ARGS__)
#define panic_log(fmt, ...) do { \
	common_log("PANIC: " fmt, ##__VA_ARGS__); \
	abort(); \
} while (false)

#define kCFCoreFoundationVersionNumber_iOS_13_0 1656.00
#define kCFCoreFoundationVersionNumber_iOS_13_3 1674.00

#endif

