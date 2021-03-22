@class BBDataProvider, BBSectionInfo, BBSectionParameters;

@interface BBDataProviderIdentity : NSObject

+ (instancetype)identityForDataProvider:(BBDataProvider *)dataProvider;

@property (nonatomic, retain) NSString *sectionIdentifier;
@property (nonatomic, retain) NSString *sectionDisplayName;

@property (nonatomic, retain) BBSectionInfo *defaultSectionInfo;
@property (nonatomic, retain) NSArray <BBSectionInfo *> *defaultSubsectionInfos;

@property (nonatomic, retain) BBSectionParameters *sectionParameters;

@end
