NS_ASSUME_NONNULL_BEGIN

/**
 * HBLOHandlerDelegate is the protocol that all handlers must conform to.
 * HBLOHandler conforms to this protocol for you, but handler classes do not
 * necessarily need to subclass HBLOHandler.
 *
 * For more information on the methods and properties of this protocol, refer to
 * the documentation for HBLOHandler.
 */
@protocol HBLOHandlerDelegate <NSObject>

@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *identifier;

@property (nonatomic, retain, nullable) NSBundle *preferencesBundle;
@property (nonatomic, retain, nullable) NSString *preferencesClass;

- (nullable id)openURL:(NSURL *)url sender:(nullable NSString *)sender;

@end

NS_ASSUME_NONNULL_END
