@interface BiometricKitIdentity : NSObject <NSSecureCoding, NSCopying>

@property (nonatomic, retain) NSUUID * uuid;
@property (nonatomic, assign) NSUInteger userID;
@property (nonatomic, assign) NSInteger type;
@property (nonatomic, assign) NSInteger attribute;
@property (nonatomic, assign) NSInteger entity;
@property (nonatomic, copy) NSString * name;

@end
