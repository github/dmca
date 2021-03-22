/**
 * CoreFoundation Version Header
 *
 * by HASHBANG Productions
 * Public Domain
 *
 * 2.0		478.23
 * 2.1		478.26
 * 2.2		478.29
 * 3.0		478.47
 * 3.1		478.52
 * 3.2		478.61
 * 4.0		550.32
 * 4.1		550.38
 * 4.2		550.52
 * 4.3		550.58
 * 5.0		675.00
 * 5.1		690.10
 * 6.x		793.00
 * 7.0		847.20
 * 7.0.3	847.21
 * 7.1		847.26
 * 8.0		1140.10
 * 8.1		1141.14
 * 8.2		1142.16
 * 8.3		1144.17
 * 8.4		1145.15
 * 9.0		1240.1
 * 9.1		1241.11
 * 9.2		1242.13
 * 9.3		1280.30
 * 10.0		1348.00
 * 10.1		1348.00
 * 10.2		1348.22
 * 10.3		1349.56
 * 11.0		1443.00
 * 11.1		1445.32
 * 11.2		1450.14
 *
 * Reference: http://iphonedevwiki.net/index.php/CoreFoundation.framework#Versions
 */

// iOS 2.0 – 4.2 are defined in <CoreFoundation/CFBase.h>. The format prior to 4.0 is
// kCFCoreFoundationVersionNumber_iPhoneOS_X_Y. 4.0 and newer have the format
// kCFCoreFoundationVersionNumber_iOS_X_Y.

#import <CoreFoundation/CFBase.h>

// Newer version number #defines may or may not be defined. For instance, the iOS 5, 6, 7 SDKs
// didn’t define any newer version than iOS 4.2. 9.3 SDK defines versions up to iOS 8.4. 10.1 SDK
// defines versions up to 9.4(!) but not 10.0

#ifndef kCFCoreFoundationVersionNumber_iOS_4_3
#define kCFCoreFoundationVersionNumber_iOS_4_3 550.58
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_0
#define kCFCoreFoundationVersionNumber_iOS_5_0 675.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_5_1
#define kCFCoreFoundationVersionNumber_iOS_5_1 690.10
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_0
#define kCFCoreFoundationVersionNumber_iOS_6_0 793.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_6_1
#define kCFCoreFoundationVersionNumber_iOS_6_1 793.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_0
#define kCFCoreFoundationVersionNumber_iOS_7_0 847.20
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_0_3
#define kCFCoreFoundationVersionNumber_iOS_7_0_3 847.21
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_7_1
#define kCFCoreFoundationVersionNumber_iOS_7_1 847.26
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_0
#define kCFCoreFoundationVersionNumber_iOS_8_0 1140.10
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_1
#define kCFCoreFoundationVersionNumber_iOS_8_1 1141.14
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_2
#define kCFCoreFoundationVersionNumber_iOS_8_2 1142.16
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_3
#define kCFCoreFoundationVersionNumber_iOS_8_3 1144.17
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_8_4
#define kCFCoreFoundationVersionNumber_iOS_8_4 1145.15
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_0
#define kCFCoreFoundationVersionNumber_iOS_9_0 1240.1
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_1
#define kCFCoreFoundationVersionNumber_iOS_9_1 1241.11
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_2
#define kCFCoreFoundationVersionNumber_iOS_9_2 1242.13
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_9_3
#define kCFCoreFoundationVersionNumber_iOS_9_3 1280.30
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_0
#define kCFCoreFoundationVersionNumber_iOS_10_0 1348.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_1
#define kCFCoreFoundationVersionNumber_iOS_10_1 1348.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_2
#define kCFCoreFoundationVersionNumber_iOS_10_2 1348.22
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_10_3
#define kCFCoreFoundationVersionNumber_iOS_10_3 1349.56
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_11_0
#define kCFCoreFoundationVersionNumber_iOS_11_0 1443.00
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_11_1
#define kCFCoreFoundationVersionNumber_iOS_11_1 1445.32
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_11_2
#define kCFCoreFoundationVersionNumber_iOS_11_2 1450.14
#endif

#ifndef kCFCoreFoundationVersionNumber10_10
#define kCFCoreFoundationVersionNumber10_10 1151.16
#endif

// Let’s also define some useful functions to check which firmware the user is on. (Note that
// feature detection is highly recommended where possible)

#define IS_IOS_OR_OLDER(version) (kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber_##version)
#define IS_IOS_OR_NEWER(version) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_##version)
#define IS_IOS_BETWEEN(start, end) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_##start && kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber_##end)

// And let’s make equivalent macros for macOS so it doesn’t feel lonely.

#define IS_MACOS_OR_OLDER(version) (kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber##version)
#define IS_MACOS_OR_NEWER(version) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber##version)
#define IS_MACOS_BETWEEN(start, end) (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber##start && kCFCoreFoundationVersionNumber <= kCFCoreFoundationVersionNumber##end)

#define IS_OSX_OR_OLDER(version) IS_MACOS_OR_OLDER(version)
#define IS_OSX_OR_NEWER(version) IS_MACOS_OR_NEWER(version)
#define IS_OSX_BETWEEN(version) IS_MACOS_BETWEEN(version)
