@class BBBulletin;

@interface SBBulletinListSection : NSObject

@property (copy, nonatomic) NSString *sectionID;
@property (nonatomic, retain) NSArray <BBBulletin *> *bulletins;

@end
