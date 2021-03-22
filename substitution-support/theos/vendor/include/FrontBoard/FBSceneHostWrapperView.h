@class FBScene;

@interface FBSceneHostWrapperView : UIView

@property (nonatomic, retain, readonly) FBScene *scene;
@property (nonatomic, copy, readonly) NSString *requester;

@end
