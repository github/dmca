#ifndef HB_LOG_USE_COLOR
	#if defined(__DEBUG__) || defined(__IPHONE_10_0)
		#define HB_LOG_USE_COLOR 0
	#else
		#define HB_LOG_USE_COLOR 1
	#endif
#endif

#if defined(__IPHONE_10_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_10_0 && __has_include(<os/log.h>)
	#import <os/log.h>

	#define HB_LOG_INTERNAL(level, type, ...) os_log_with_type(OS_LOG_DEFAULT, level, "[%{public}s:%{public}d] %{public}@", __BASE_FILE__, __LINE__, [NSString stringWithFormat:__VA_ARGS__])

	#define HBLogDebug(...) HB_LOG_INTERNAL(OS_LOG_TYPE_DEBUG, "DEBUG", __VA_ARGS__)
	#define HBLogInfo(...) HB_LOG_INTERNAL(OS_LOG_TYPE_INFO, "INFO", __VA_ARGS__)
	#define HBLogWarn(...) HB_LOG_INTERNAL(OS_LOG_TYPE_DEFAULT, "WARN", __VA_ARGS__)
	#define HBLogError(...) HB_LOG_INTERNAL(OS_LOG_TYPE_ERROR, "ERROR", __VA_ARGS__)
#else
	#include <CoreFoundation/CFLogUtilities.h>

	#ifdef HB_LOG_USE_COLOR
		#define HB_LOG_FORMAT(color) CFSTR("\e[1;3" #color "m[%s] \e[m\e[0;3" #color "m%s:%d\e[m \e[0;30;4" #color "m%s:\e[m %@")
	#else
		#define HB_LOG_FORMAT(color) CFSTR("[%s: %s:%d] %s: %@")
	#endif

	#define HB_LOG_INTERNAL(color, level, type, ...) CFLog(level, HB_LOG_FORMAT(color), THEOS_INSTANCE_NAME, __BASE_FILE__, __LINE__, type, (__bridge CFStringRef)[NSString stringWithFormat:__VA_ARGS__])

	#ifdef __DEBUG__
		#define HBLogDebug(...) HB_LOG_INTERNAL(6, kCFLogLevelNotice, "DEBUG", __VA_ARGS__)
	#else
		#define HBLogDebug(...)
	#endif

	#define HBLogInfo(...) HB_LOG_INTERNAL(2, kCFLogLevelNotice, "INFO", __VA_ARGS__)
	#define HBLogWarn(...) HB_LOG_INTERNAL(3, kCFLogLevelWarning, "WARN", __VA_ARGS__)
	#define HBLogError(...) HB_LOG_INTERNAL(1, kCFLogLevelError, "ERROR", __VA_ARGS__)
#endif
