@class MKPlacemark;

typedef void (^MKMapItemItemsFromHandleCompletion)(NSArray <NSURL *> *items);

@interface MKMapItem (Private)

+ (NSArray *)mapItemsFromURL:(NSURL *)url options:(id *)options;
+ (NSURL *)urlForMapItems:(NSArray *)items options:(id)options;
+ (void)_mapItemsFromHandleURL:(NSURL *)url completionHandler:(MKMapItemItemsFromHandleCompletion)completion;

@property (nonatomic, retain) MKPlacemark *placemark;
@property BOOL isCurrentLocation;

@end
