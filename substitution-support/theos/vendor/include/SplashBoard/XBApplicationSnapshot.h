@interface XBApplicationSnapshot : NSObject

@property (nonatomic, copy) NSString * variantID;
@property (nonatomic, copy) NSString * launchInterfaceIdentifier;
@property (nonatomic, assign) CGFloat imageScale;
@property (nonatomic, assign, getter=isImageOpaque) BOOL imageOpaque;
@property (nonatomic, assign) CGAffineTransform imageTransform;
@property (nonatomic, assign) NSInteger imageOrientation;
@property (nonatomic, copy, readonly) NSString * identifier;
@property (nonatomic, copy, readonly) NSString * groupID;
@property (nonatomic, copy) NSString * name;
@property (nonatomic, copy) NSString * scheme;
@property (nonatomic, copy) NSString * requiredOSVersion;
@property (nonatomic, copy, readonly) NSString * path;
@property (nonatomic, copy, readonly) NSString * filename;
@property (nonatomic, readonly, getter=isExpired) BOOL expired;
@property (nonatomic, assign) NSInteger contentType;
@property (nonatomic, assign, getter=isFullScreen) BOOL fullScreen;
@property (nonatomic, assign) CGSize referenceSize;
@property (nonatomic, readonly) CGSize naturalSize;
@property (nonatomic, assign) CGRect contentFrame;
@property (nonatomic, readonly) BOOL hasFullSizedContent;
@property (nonatomic, assign) NSInteger interfaceOrientation;
@property (nonatomic, assign) NSInteger classicMode;
@property (nonatomic, assign) NSInteger compatibilityMode;
@property (nonatomic, assign) NSInteger backgroundStyle;

@end
