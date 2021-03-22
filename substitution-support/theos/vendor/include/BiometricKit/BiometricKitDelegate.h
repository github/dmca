@class BiometricKitEnrollProgressInfo, BiometricKitIdentity;

@protocol BiometricKitDelegate <NSObject>
@optional

- (void)enrollProgress:(BiometricKitEnrollProgressInfo *)progress;
- (void)enrollResult:(BiometricKitIdentity *)result;
- (void)homeButtonPressed;
- (void)matchResult:(BiometricKitIdentity *)result;
- (void)matchResult:(BiometricKitIdentity *)result withDetails:(NSDictionary *)details;
- (void)statusMessage:(NSUInteger)message;
- (void)taskResumeStatus:(NSInteger)status;
- (void)templateUpdate:(BiometricKitIdentity *)update withDetails:(NSDictionary *)details;
- (void)touchIDButtonPressed:(BOOL)pressed;

@end
