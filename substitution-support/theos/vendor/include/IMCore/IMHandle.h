@class IMPerson;

@interface IMHandle : NSObject

+ (NSArray <IMHandle *> *)imHandlesForIMPerson:(IMPerson *)person;

@property (nonatomic, retain, readonly) NSString *ID;

@property (nonatomic, retain) IMPerson *person;

@property (nonatomic, retain, readonly) NSString *name;
@property (nonatomic, retain, readonly) NSString *_displayNameWithAbbreviation;

@end
