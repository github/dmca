@protocol NSXPCListenerDelegate <NSObject>

@optional

-(BOOL)listener:(id)listener shouldAcceptNewConnection:(id)connection;

@end

@interface NSXPCListener : NSObject

+ (instancetype)serviceListener;

- (void)setDelegate:(id)delegate;

- (id)delegate;

- (void)resume;

@end