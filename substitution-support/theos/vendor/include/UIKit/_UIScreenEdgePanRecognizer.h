typedef NS_ENUM(NSInteger, UIScreenEdgePanRecognizerType) {
    UIScreenEdgePanRecognizerTypeMultitasking,
    UIScreenEdgePanRecognizerTypeNavigation,
    UIScreenEdgePanRecognizerTypeOther
};

@interface _UIScreenEdgePanRecognizer : NSObject

- (instancetype)initWithType:(UIScreenEdgePanRecognizerType)type;

@end