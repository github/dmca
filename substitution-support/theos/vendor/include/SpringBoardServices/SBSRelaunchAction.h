#import <BaseBoard/BSAction.h>

typedef NS_OPTIONS(NSUInteger, SBSRelaunchActionOptions) {
	SBSRelaunchActionOptionsNone,
	SBSRelaunchActionOptionsRestartRenderServer = 1 << 0,
	SBSRelaunchActionOptionsSnapshotTransition = 1 << 1,
	SBSRelaunchActionOptionsFadeToBlackTransition = 1 << 2
};

@interface SBSRelaunchAction : BSAction

@property (nonatomic, readonly, copy) NSString *reason;
@property (nonatomic, readonly, retain) NSURL *targetURL;
@property (nonatomic, readonly) SBSRelaunchActionOptions options;

+ (instancetype)actionWithReason:(NSString *)reason options:(SBSRelaunchActionOptions)options targetURL:(NSURL *)targetURL;

- (instancetype)initWithReason:(NSString *)reason options:(SBSRelaunchActionOptions)options targetURL:(NSURL *)targetURL;

@end
