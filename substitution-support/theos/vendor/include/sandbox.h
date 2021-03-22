#include_next <sandbox.h>

// courtesy of clang
// https://github.com/applesrc/clang/blob/bb8f644/src/projects/compiler-rt/lib/sanitizer_common/sanitizer_mac_spi.cc

enum sandbox_filter_type {
	SANDBOX_FILTER_NONE,
	SANDBOX_FILTER_PATH,
	SANDBOX_FILTER_GLOBAL_NAME,
	SANDBOX_FILTER_LOCAL_NAME,
	SANDBOX_FILTER_APPLEEVENT_DESTINATION,
	SANDBOX_FILTER_RIGHT_NAME,
	SANDBOX_FILTER_PREFERENCE_DOMAIN,
	SANDBOX_FILTER_KEXT_BUNDLE_ID,
	SANDBOX_FILTER_INFO_TYPE,
	SANDBOX_FILTER_NOTIFICATION
};

extern const enum sandbox_filter_type SANDBOX_CHECK_NO_REPORT;
extern const enum sandbox_filter_type SANDBOX_CHECK_CANONICAL;

int sandbox_check(pid_t pid, const char *operation, enum sandbox_filter_type type, ...);
