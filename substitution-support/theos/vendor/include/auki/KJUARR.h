@class BBBulletin;

@interface KJUARR : NSObject

+ (void)doUrThing:(BBBulletin *)bulletin;
+ (void)doUrThing:(BBBulletin *)bulletin withImages:(NSArray *)images;
+ (void)doUrThing:(BBBulletin *)bulletin withImages:(NSArray *)images recipients:(NSArray *)recipients;
+ (void)doUrThing:(BBBulletin *)bulletin withRecipients:(NSArray *)recipients;

@end
