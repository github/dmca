@interface FBWorkspaceEventQueue : NSObject

+ (instancetype)sharedInstance;

- (void)executeOrPrependEvents:(NSArray *)events;
- (void)executeOrAppendEvent:(id)event;
- (void)executeOrPrependEvent:(id)event;

@end
