@interface FBSSceneSettings : NSObject <NSCopying, NSMutableCopying>

@property (nonatomic, readonly, getter=isBackgrounded) BOOL backgrounded;

@property (nonatomic, readonly, getter=isEffectivelyBackgrounded) BOOL effectivelyBackgrounded;

- (instancetype)initWithSettings:(FBSSceneSettings *)settings;

@end
