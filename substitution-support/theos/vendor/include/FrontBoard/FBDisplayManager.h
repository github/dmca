@class FBSDisplay;

@interface FBDisplayManager : NSObject

+ (instancetype)sharedInstance;

+ (FBSDisplay *)mainDisplay;

@property (nonatomic, retain) FBSDisplay *mainDisplay;

- (NSArray *)displays;

@end
