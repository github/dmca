enum {
NSXPCConnectionPrivileged = (1 << 12UL )
};
typedef NSUInteger NSXPCConnectionOptions;

@interface NSXPCConnection : NSObject

@property (retain) id exportedObject;

@property (retain) id exportedInterface;

@property (retain) id remoteObjectInterface;

- (id)initWithServiceName:(id)serviceName;

- (void)resume;

- (id)remoteObjectProxyWithErrorHandler:(id)errorHandler;

- (instancetype)initWithMachServiceName:(NSString *)name options:(NSXPCConnectionOptions)options;

@end