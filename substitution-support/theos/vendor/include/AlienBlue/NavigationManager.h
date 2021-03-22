@class Post;

@interface NavigationManager : NSObject

+ (instancetype)shared;

- (void)openRedditThreadUrl:(NSString *)url;
- (void)showPostsForSubreddit:(NSString *)subreddit title:(NSString *)title animated:(BOOL)animated;
- (void)showCommentsForPost:(Post *)post contextId:(NSString *)contextID fromController:(id)controller;

@end
