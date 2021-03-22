
@class IMDHandle;

@interface IMDChat : NSObject

@property (nonatomic, retain) NSDictionary <NSString *, id> *properties;

- (void)updateProperties:(NSDictionary <NSString *, id> *)properties;

@property (nonatomic, retain) NSArray <IMDHandle *> *participants;

@end
