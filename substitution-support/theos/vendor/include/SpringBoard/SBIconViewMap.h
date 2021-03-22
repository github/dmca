@class SBIconModel, SBIconView, SBIcon;

@interface SBIconViewMap : NSObject

+ (SBIconViewMap *)homescreenMap NS_DEPRECATED_IOS(5_0, 9_3);

@property (nonatomic, readonly) SBIconModel *iconModel;

- (SBIconView *)iconViewForIcon:(SBIcon *)icon;
- (SBIconView *)_iconViewForIcon:(SBIcon *)icon;

@end
