/**
 * Name: TechSupport
 * Type: iOS framework
 * Desc: iOS framework to assist in providing support for and receiving issue
 *       reports and feedback from users.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import <UIKit/UIKit.h>

@interface TSHTMLViewController : UIViewController
@property(nonatomic, readonly) UIWebView *webView;
- (id)initWithHTMLContent:(NSString *)content;
- (id)initWithHTMLContent:(NSString *)content dataDetector:(UIDataDetectorTypes)dataDetectors;
- (id)initWithURL:(NSURL *)url;
- (id)initWithURL:(NSURL *)url dataDetector:(UIDataDetectorTypes)dataDetectors;
- (void)setContent:(NSString *)content;
- (void)setURL:(NSURL *)url;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
