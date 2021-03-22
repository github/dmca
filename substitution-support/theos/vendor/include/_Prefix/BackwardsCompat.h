#ifndef __has_feature
	#define __has_feature(feature) 0
#endif

// The following block originally from Ryan Petrich, licensed under GNU GPL v3
// https://github.com/rpetrich/theos/blob/master/Prefix.pch
#ifndef __clang__
	#define weak assign
	#define strong retain

	// Fix old GCC versions not identifying the correct response type on
	// [[NSDictionary alloc] initWithContentsOfFile:...] and other similar calls
	#define THEOS_WORKAROUND_ALLOC(type) @interface type() + (type *)alloc; @end
	THEOS_WORKAROUND_ALLOC(NSDictionary);
	THEOS_WORKAROUND_ALLOC(NSMutableDictionary);
	THEOS_WORKAROUND_ALLOC(NSArray);
	THEOS_WORKAROUND_ALLOC(NSMutableArray);
	#undef THEOS_WORKAROUND_ALLOC

	// Workaround NS_AVAILABLE being used in enums, by disabling entirely :(
	#undef NS_AVAILABLE
	#define NS_AVAILABLE(osx, ios)
#endif

#if !__has_feature(nullability)
	#define NS_ASSUME_NONNULL_BEGIN
	#define NS_ASSUME_NONNULL_END
	#define nullable
	#define nonnull
	#define _Nullable
	#define _Nonnull
#endif
