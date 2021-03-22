#import "SBSApplicationShortcutIcon.h"

@interface SBSApplicationShortcutSystemIcon : SBSApplicationShortcutIcon

@property (nonatomic, readonly) NSInteger type;

- (instancetype)initWithType:(NSInteger)type;

@end
