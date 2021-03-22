@interface APSMessage : NSObject <NSCoding>

- (id)initWithDictionary:(NSDictionary *)dictionary;
- (id)initWithDictionary:(NSDictionary *)dictionary xpcMessage:(id)xpcMessage;
- (id)initWithTopic:(NSString *)topic userInfo:(id)userInfo;

@property (nonatomic) NSUInteger identifier;
@property (nonatomic, retain) NSString *topic;
@property (nonatomic, retain) NSDictionary *userInfo;

- (NSDictionary <NSString *, id> *)dictionaryRepresentation;
- (id <NSCoding>)objectForKey:(NSString *)key;
- (void)setObject:(id <NSCoding>)object forKey:(NSString *)key;

@end
