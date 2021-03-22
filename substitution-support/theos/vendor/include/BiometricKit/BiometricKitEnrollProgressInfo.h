@interface BiometricKitEnrollProgressInfo : NSObject

@property (nonatomic, assign) NSInteger progress;
@property (nonatomic, retain) NSDictionary *captureImage;
@property (nonatomic, retain) NSDictionary *renderedImage;
@property (nonatomic, assign) NSInteger message;

@end
