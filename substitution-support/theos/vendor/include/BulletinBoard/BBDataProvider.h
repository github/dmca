@class BBActionResponse, BBBulletinRequestParameters, BBDataProviderIdentity, BBSectionIcon, BBSectionInfo, BBSectionParameters, BBThumbnailSizeConstraints;

@protocol BBSectionIdentity <NSObject>

@required

- (NSString *)sectionIdentifier;

@optional

- (NSString *)parentSectionIdentifier;
- (NSString *)universalSectionIdentifier;

- (NSString *)sectionDisplayName;
- (NSData *)sectionIconData;
- (BBSectionIcon *)sectionIcon;

- (BBSectionInfo *)defaultSectionInfo;

@end

@protocol BBDataProvider <BBSectionIdentity>

- (NSArray *)sortDescriptors;

@optional

- (void)dataProviderDidLoad;

- (BBSectionParameters *)sectionParameters;
- (void)receiveMessageWithName:(NSString *)name userInfo:(NSDictionary *)userInfo;
- (BOOL)migrateSectionInfo:(BBSectionInfo *)sectionInfo oldSectionInfo:(BBSectionInfo *)sectionInfo;
- (BOOL)syncsBulletinDismissal;

- (NSArray <BBSectionInfo *> *)defaultSubsectionInfos;
- (NSString *)displayNameForSubsectionID:(NSString *)subsectionID;

- (void)noteSectionInfoDidChange:(BBSectionInfo *)sectionInfo;

- (NSSet *)bulletinsFilteredBy:(NSUInteger)filter count:(NSUInteger)count lastCleared:(NSDate *)lsatCleared;
- (NSSet *)bulletinsFilteredBy:(NSUInteger)filter enabledSectionIDs:(NSSet *)sectionIDs count:(NSUInteger)count lastCleared:(NSDate *)lsatCleared;
- (NSString *)displayNameForFilterID:(NSString *)filterID;

- (NSSet *)bulletinsWithRequestParameters:(BBBulletinRequestParameters *)requestParameters lastCleared:(id)lastClearedInfo;

- (CGFloat)attachmentAspectRatioForRecordID:(NSString *)recordID;
- (NSData *)attachmentPNGDataForRecordID:(NSString *)recordID sizeConstraints:(BBThumbnailSizeConstraints *)sizeConstraints;
- (NSData *)primaryAttachmentDataForRecordID:(NSString *)recordID;

- (void)getAspectRatioForAttachmentUUID:(NSUUID *)attachmentUUID recordID:(NSString *)recordID withCompletionHandler:(void (^)(CGFloat))completion;
- (void)getPNGDataForAttachmentUUID:(NSUUID *)attachmentUUID recordID:(NSString *)recordID sizeConstraints:(BBThumbnailSizeConstraints *)sizeConstraints withCompletionHandler:(void (^)(NSData *))completion;
- (void)getDataForAttachmentUUID:(NSUUID *)attachmentUUID recordID:(NSString *)recordID withCompletionHandler:(void (^)(NSData *))completion;

- (void)handleBulletinActionResponse:(BBActionResponse *)actionResponse withCompletion:(void (^)(BOOL))completion;
- (void)handleBulletinActionResponse:(BBActionResponse *)actionResponse;

- (id)clearedInfoForBulletins:(NSSet *)bulletins;
- (id)clearedInfoForBulletins:(NSSet *)bulletins lastClearedInfo:(id)lastClearedInfo;
- (id)clearedInfoForClearingBulletinsFromDate:(NSDate *)fromDate toDate:(NSDate *)toDate lastClearedInfo:(id)lastClearedInfo;
- (id)clearedInfoForClearingAllBulletinsWithLastClearedInfo:(id)lastClearedInfo;

@end

@interface BBDataProvider : NSObject <BBDataProvider>

@property (nonatomic, retain) BBDataProviderIdentity *identity;

@end
