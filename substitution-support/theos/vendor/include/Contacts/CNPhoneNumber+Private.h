@interface CNPhoneNumber (Private)

@property (nonatomic, readonly, copy) NSString *countryCode;
@property (nonatomic, readonly, copy) NSString *digits;
@property (nonatomic, readonly, copy) NSString *formattedInternationalStringValue;
@property (nonatomic, readonly, copy) NSString *formattedStringValue;
@property (nonatomic, readonly, copy) NSString *normalizedStringValue;
@property (nonatomic, readonly, copy) NSString *unformattedInternationalStringValue;

@end
