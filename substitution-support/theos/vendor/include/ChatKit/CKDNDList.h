@interface CKDNDList : NSObject

+ (instancetype)sharedList;

- (BOOL)isMutedChatIdentifier:(NSString *)identifier;
- (NSDate *)unmuteDateForIdentifier:(NSString *)identifier;

@end
