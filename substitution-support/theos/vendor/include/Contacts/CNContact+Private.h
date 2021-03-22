@class CNEmailAddressContactPredicate, CNPhoneNumberContactPredicate;

@interface CNContact (Private)

+ (CNEmailAddressContactPredicate *)predicateForContactMatchingEmailAddress:(NSString *)emailAddress;
+ (CNPhoneNumberContactPredicate *)predicateForContactMatchingPhoneNumber:(CNPhoneNumber *)phoneNumber;

@end
