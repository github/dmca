#define IS_IPAD ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
#define IN_SPRINGBOARD ([[NSBundle mainBundle].bundleIdentifier isEqualToString:@"com.apple.springboard"])
#define IN_BUNDLE(bundleID) ([[NSBundle mainBundle].bundleIdentifier isEqualToString:bundleID])

#undef NS_AVAILABLE_MAC
#undef NS_CLASS_AVAILABLE_MAC
#define NS_AVAILABLE_MAC(...)
#define NS_CLASS_AVAILABLE_MAC(...)

#define NSRect CGRect
#define NSPoint CGPoint
#define NSSize CGSize
