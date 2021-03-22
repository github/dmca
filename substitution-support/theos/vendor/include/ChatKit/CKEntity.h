@class IMHandle;

@interface CKEntity : NSObject

+ (instancetype)copyEntityForAddressString:(NSString *)address;

@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) IMHandle *handle;

@property BOOL foundABNameForSender;

@end
