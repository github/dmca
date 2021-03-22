@interface CPDistributedMessagingCenter : NSObject

+ (instancetype)centerNamed:(NSString *)name;

- (void)runServer;
- (void)runServerOnCurrentThread;
- (void)stopServer;

- (void)registerForMessageName:(NSString *)messageName target:(id)target selector:(SEL)selector;

- (BOOL)sendMessageName:(NSString *)messageName userInfo:(NSDictionary *)userInfo;

- (NSDictionary *)sendMessageAndReceiveReplyName:(NSString *)messageName userInfo:(NSDictionary *)userInfo;
- (NSDictionary *)sendMessageAndReceiveReplyName:(NSString *)messageName userInfo:(NSDictionary *)userInfo error:(NSError **)error;

@end
