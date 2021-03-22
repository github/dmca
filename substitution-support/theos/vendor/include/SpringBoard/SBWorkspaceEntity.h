@interface SBWorkspaceEntity : NSObject

+ (instancetype)entity;

@property (nonatomic, copy) NSString *identifier;
@property (nonatomic, assign) NSInteger layoutRole;

@end
