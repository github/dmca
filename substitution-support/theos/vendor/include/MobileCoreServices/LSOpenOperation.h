@interface LSOpenOperation : NSOperation

- (instancetype)initForOpeningResource:(NSURL *)resource usingApplication:(NSString *)application uniqueDocumentIdentifier:(id)uniqueDocumentIdentifier sourceIsManaged:(BOOL)sourceIsManaged userInfo:(NSDictionary *)userInfo options:(NSDictionary *)options delegate:(id)delegate;

- (void)main;

@property BOOL didSucceed;

@end
