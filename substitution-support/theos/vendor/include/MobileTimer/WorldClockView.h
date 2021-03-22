// app

@class WorldClockCity;

@interface WorldClockView : UIView

@property (readonly, assign, nonatomic) UILabel *nameLabel;
@property (nonatomic, retain) WorldClockCity *city;

@end
