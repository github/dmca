//
//  JailbreakViewController.h
//  Undecimus
//
//  Created by pwn20wnd on 8/29/18.
//  Copyright © 2018 - 2019 Pwn20wnd. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <UIProgressHUD.h>
#import "common.h"

#define __assert_terminate() do { \
    if (!JailbreakViewController.sharedController.canExit) break; \
    exit(EXIT_FAILURE); \
} while(false)
#ifdef FAIL_LABEL
#define __assert_failure() do { \
    if (__assertion_failure != false) break; \
    __assertion_failure = true; \
    goto fail; \
} while(false)
#else
#define __assert_failure() do { \
    __assert_terminate(); \
} while(false)
#endif
#define _assert(test, message) do {\
    if ((test)) break; \
    int saved_errno = errno; \
    NSString *msg = (message); \
    LOG("%@", msg); \
    showAlert(@"Error", msg, true, false); \
    errno = saved_errno; \
    __assert_failure(); \
} while(false)

#define _notice(msg, wait, destructive) showAlert(@"Notice", msg, wait, destructive)

#define _status(msg, btnenbld, nvbenbld) do { \
    dispatch_async(dispatch_get_main_queue(), ^{ \
        init_function(); \
        if ([[[[[JailbreakViewController sharedController] goButton] titleLabel] text] isEqualToString:msg]) return; \
        LOG("Status: %@", msg); \
        [UIView performWithoutAnimation:^{ \
            [[[JailbreakViewController sharedController] goButton] setEnabled:btnenbld]; \
            [[[JailbreakViewController sharedController] settingsButton] setUserInteractionEnabled:nvbenbld]; \
            [[[JailbreakViewController sharedController] goButton] setTitle:msg forState: btnenbld ? UIControlStateNormal : UIControlStateDisabled]; \
            [[[JailbreakViewController sharedController] goButton] layoutIfNeeded]; \
        }]; \
    }); \
} while (false)

#define _progress(x) do { \
    dispatch_async(dispatch_get_main_queue(), ^{ \
        init_function(); \
        if ([[[[JailbreakViewController sharedController] exploitMessageLabel] text] isEqualToString:x]) return; \
        LOG("Progress: %@", x); \
        [[[JailbreakViewController sharedController] exploitMessageLabel] setText:x]; \
    }); \
} while (false)

@interface JailbreakViewController : UIViewController
@property (weak, nonatomic) IBOutlet UIButton *goButton;
@property (weak, nonatomic) IBOutlet UITextView *outputView;
@property (weak, nonatomic) IBOutlet UIButton *darkModeButton;
@property (weak, nonatomic) IBOutlet UIButton *settingsButton;
@property (weak, nonatomic) IBOutlet UIButton *mainDevsButton;

@property (weak, nonatomic) IBOutlet UILabel *exploitProgressLabel;
@property (weak, nonatomic) IBOutlet UILabel *exploitMessageLabel;
@property (weak, nonatomic) IBOutlet UILabel *u0Label;
@property (weak, nonatomic) IBOutlet UILabel *uOVersionLabel;

@property (weak, nonatomic) IBOutlet UIProgressView *jailbreakProgressBar;

@property (weak, nonatomic) IBOutlet UIView *mainView;
@property (weak, nonatomic) IBOutlet UIView *creditsView;
@property (weak, nonatomic) IBOutlet UIView *settingsView;
@property (weak, nonatomic) IBOutlet UIView *mainDevView;
@property (weak, nonatomic) IBOutlet UIView *backgroundView;

@property (weak, nonatomic) IBOutlet UINavigationBar *settingsNavBar;
@property (weak, nonatomic) IBOutlet UINavigationBar *creditsNavBar;

@property (weak, nonatomic) IBOutlet UILabel *jailbreakLabel;
@property (weak, nonatomic) IBOutlet UILabel *byLabel;
@property (weak, nonatomic) IBOutlet UILabel *uncoverLabel;
@property (weak, nonatomic) IBOutlet UILabel *supportedOSLabel;
@property (weak, nonatomic) IBOutlet UILabel *UIByLabel;
@property (weak, nonatomic) IBOutlet UILabel *firstAndLabel;
@property (weak, nonatomic) IBOutlet UILabel *fourthAndLabel;


@property (readonly) JailbreakViewController *sharedController;
@property (assign) BOOL canExit;

double uptime(void);

NSString *hexFromInt(NSInteger val);

- (IBAction)tappedOnJailbreak:(id)sender;
+(JailbreakViewController*)sharedController;
- (void)appendTextToOutput:(NSString*)text;
- (void)updateStatus;

@end

static inline UIProgressHUD *addProgressHUD() {
    __block UIProgressHUD *hud = nil;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    dispatch_async(dispatch_get_main_queue(), ^{
        init_function();
        hud = [[UIProgressHUD alloc] init];
        [hud setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
        UIView *view = [[JailbreakViewController sharedController] view];
        [hud showInView:view];
        dispatch_semaphore_signal(semaphore);
    });
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    return hud;
}

static inline void removeProgressHUD(UIProgressHUD *hud) {
    if (hud == nil) {
        return;
    }
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    dispatch_async(dispatch_get_main_queue(), ^{
        init_function();
        [hud hide];
        [hud done];
        dispatch_semaphore_signal(semaphore);
    });
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

static inline void updateProgressHUD(UIProgressHUD *hud, NSString *msg) {
    if (hud == nil) {
        return;
    }
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    dispatch_async(dispatch_get_main_queue(), ^{
        init_function();
        [hud setText:msg];
        dispatch_semaphore_signal(semaphore);
    });
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

static inline void showAlertWithCancel(NSString *title, NSString *message, Boolean wait, Boolean destructive, NSString *cancel) {
    dispatch_semaphore_t semaphore;
    if (wait)
        semaphore = dispatch_semaphore_create(0);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        init_function();
        JailbreakViewController *controller = [JailbreakViewController sharedController];
        [controller dismissViewControllerAnimated:YES completion:nil];
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *OK = [UIAlertAction actionWithTitle:@"OK" style:destructive ? UIAlertActionStyleDestructive : UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            controller.canExit = YES;
            if (wait)
                dispatch_semaphore_signal(semaphore);
        }];
        [alertController addAction:OK];
        [alertController setPreferredAction:OK];
        if (cancel) {
            UIAlertAction *abort = [UIAlertAction actionWithTitle:cancel style:destructive ? UIAlertActionStyleDestructive : UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                controller.canExit = NO;
                if (wait)
                    dispatch_semaphore_signal(semaphore);
            }];
            [alertController addAction:abort];
            [alertController setPreferredAction:abort];
        }
        [controller presentViewController:alertController animated:YES completion:nil];
    });
    if (wait)
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

static inline void showAlert(NSString *title, NSString *message, Boolean wait, Boolean destructive) {
    __block bool outputIsHidden;
    dispatch_block_t checkOutput = ^{
        init_function();
        outputIsHidden = [[[JailbreakViewController sharedController] outputView] isHidden];
    };

    if ([[NSThread currentThread] isMainThread]) {
        checkOutput();
    } else {
        dispatch_sync(dispatch_get_main_queue(), checkOutput);
    }
    showAlertWithCancel(title, message, wait, destructive, outputIsHidden?nil:@"View Log");
}
