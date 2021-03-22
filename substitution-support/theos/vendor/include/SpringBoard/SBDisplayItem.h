@interface SBDisplayItem : NSObject

@property (nonatomic, retain, readonly) NSString *type;
@property (nonatomic, retain, readonly) NSString *displayIdentifier;

+ (instancetype)displayItemWithType:(NSString *)type displayIdentifier:(NSString *)identifier;

@end
