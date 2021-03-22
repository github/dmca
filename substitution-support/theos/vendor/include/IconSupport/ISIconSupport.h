@interface ISIconSupport : NSObject {
    NSMutableSet *extensions;
}

+ (instancetype)sharedInstance;

- (NSString *)extensionString;
- (BOOL)addExtension:(NSString *)extension;
- (BOOL)isBeingUsedByExtensions;
- (void)repairAndReloadIconState;
- (void)repairAndReloadIconState:(NSDictionary *)iconState;

@end
