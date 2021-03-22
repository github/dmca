@protocol FBSceneClientProvider <NSObject>

@required

- (void)beginTransaction;

- (void)endTransaction;

@end
