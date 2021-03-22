@interface MPUNowPlayingArtworkView : UIView

@property (nonatomic, retain) UIImage *artworkImage;

@property (nonatomic) BOOL activated;

- (void)setActivated:(BOOL)activated animated:(BOOL)animated;

- (void)addTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)events;
- (void)removeTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)events;

@end
