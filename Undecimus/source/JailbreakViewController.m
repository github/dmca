//
//  JailbreakViewController.m
//  Undecimus
//
//  Created by pwn20wnd on 8/29/18.
//  Copyright © 2018 - 2019 Pwn20wnd. All rights reserved.
//

#include <common.h>
#include <sys/time.h>
#import "JailbreakViewController.h"
#import "SettingsTableViewController.h"
#import "CreditsTableViewController.h"
#include "jailbreak.h"
#include "prefs.h"
#include "utils.h"
#include "exploits.h"
#include <patchfinder64.h>
#include <pthread.h>
#include <mach/mach.h>
#include <sandbox.h>
#include <dlfcn.h>

@interface JailbreakViewController ()

@end

@implementation JailbreakViewController
static JailbreakViewController *sharedController = nil;
static NSMutableString *output = nil;
static NSString *bundledResources = nil;
static NSOperationQueue *updateQueue = nil;
extern int maxStage;

- (IBAction)tappedOnJailbreak:(id)sender
{
    init_function();
    [self.exploitMessageLabel setAlpha:1];
    [self.exploitProgressLabel setAlpha:1];
    [self.jailbreakProgressBar setAlpha:1];
    
    void (^block)(void) = ^(void) {
        _assert(bundledResources != nil, localize(@"Bundled Resources version missing."));
        if (!has_exploit_support(EXPLOIT_SUPPORT_JAILBREAK)) {
            _status(localize(@"Unsupported"), false, true);
            return;
        }
        jailbreak();
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (void)updateStatus {
    init_function();
    prefs_t *prefs = copy_prefs();
    
    if (!has_exploit_support(EXPLOIT_SUPPORT_JAILBREAK)) {
        _status(localize(@"Unsupported"), false, true);
        _progress(localize(@"Unsupported"));
    } else if (prefs->restore_rootfs) {
        _status(localize(@"Restore RootFS"), true, true);
        _progress(localize(@"Ready to restore RootFS"));
    } else if (jailbreakEnabled()) {
        _status(localize(@"Re-Jailbreak"), true, true);
        _progress(localize(@"Ready to re-jailbreak"));
    } else {
        _status(localize(@"Jailbreak"), true, true);
        _progress(localize(@"Ready to jailbreak"));
    }
    release_prefs(&prefs);
}

- (void)viewWillAppear:(BOOL)animated {
    init_function();
    [super viewWillAppear:animated];
    
    [self.jailbreakProgressBar setProgress:0];
    [self.jailbreakProgressBar setTransform:CGAffineTransformScale(CGAffineTransformIdentity, 1, 2)];
    
    [self.settingsView setTransform:CGAffineTransformScale(CGAffineTransformIdentity, 0.7, 0.7)];
    [self.settingsView setAlpha:0];
    [self.mainDevView setTransform:CGAffineTransformScale(CGAffineTransformIdentity, 0.7, 0.7)];
    [self.mainDevView setAlpha:0];
    [self.creditsView setTransform:CGAffineTransformScale(CGAffineTransformIdentity, 0.7, 0.7)];
    [self.creditsView setAlpha:0];
}

void sandbox_extension_issue_file_to_process_by_pid(void);

- (void)viewDidLoad {
    init_function();
    [super viewDidLoad];
    _canExit = YES;
    // Do any additional setup after loading the view, typically from a nib.
    prefs_t *prefs = copy_prefs();
    
    if (prefs->hide_log_window) {
        _outputView.hidden = YES;
        _outputView = nil;
    }
    
    if (prefs->dark_mode) {
        [self darkMode];
    } else {
        [self lightMode];
    }
    //
    release_prefs(&prefs);
    
    [self.settingsNavBar setBackgroundImage:[UIImage new] forBarMetrics:UIBarMetricsDefault];
    [self.settingsNavBar setShadowImage:[UIImage new]];
    [self.creditsNavBar setBackgroundImage:[UIImage new] forBarMetrics:UIBarMetricsDefault];
    [self.creditsNavBar setShadowImage:[UIImage new]];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showSpeicalThanks:) name:@"showSpecialThanks" object:nil];
    [self.exploitProgressLabel setText:[NSString stringWithFormat:@"%d/%d", 0, maxStage]];
    [self.uOVersionLabel setText:[NSString stringWithFormat:@"unc0ver Version: %@", appVersion()]];
    
    sharedController = self;
    bundledResources = bundledResourcesVersion();
    LOG("unc0ver Version: %@", appVersion());
    printOSDetails();
    LOG("Bundled Resources Version: %@", bundledResources);
    if (bundledResources == nil) {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), ^{
            init_function();
            showAlert(localize(@"Error"), localize(@"Bundled Resources version is missing. This build is invalid."), false, false);
        });
    }

}

- (void)darkMode {
    init_function();
    [[NSNotificationCenter defaultCenter] postNotificationName:@"darkModeSettings" object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"darkModeCredits" object:self];
    
    [self.darkModeButton setImage:[UIImage imageNamed:@"DarkMode-Dark"] forState:UIControlStateNormal];
    [self.settingsButton setImage:[UIImage imageNamed:@"Settings-Dark"] forState:UIControlStateNormal];
    [self.exploitProgressLabel setTextColor:[UIColor whiteColor]];
    [self.exploitMessageLabel setTextColor:[UIColor whiteColor]];
    [self.u0Label setTextColor:[UIColor whiteColor]];
    [self.uOVersionLabel setTextColor:[UIColor whiteColor]];
    [self.jailbreakLabel setTextColor:[UIColor whiteColor]];
    [self.byLabel setTextColor:[UIColor whiteColor]];
    [self.UIByLabel setTextColor:[UIColor whiteColor]];
    [self.firstAndLabel setTextColor:[UIColor whiteColor]];
    [self.uncoverLabel setTextColor:[UIColor whiteColor]];
    [self.supportedOSLabel setTextColor:[UIColor whiteColor]];
    [self.fourthAndLabel setTextColor:[UIColor whiteColor]];
    [self.outputView setTextColor:[UIColor whiteColor]];
    [self.backgroundView setBackgroundColor:[UIColor colorWithRed:10.0f/255.0f green:13.0f/255.0f blue:17.0f/255.0f alpha:0.97f]];
    [self.mainDevsButton setTitleColor:[UIColor whiteColor] forState:normal];
    [self.settingsNavBar setTintColor:[UIColor whiteColor]];
    [self.settingsNavBar setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [self.settingsNavBar setLargeTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [self.creditsNavBar setTintColor:[UIColor whiteColor]];
    [self.creditsNavBar setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [self.creditsNavBar setLargeTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    self.jailbreakProgressBar.trackTintColor = [UIColor blackColor];
    [self setNeedsStatusBarAppearanceUpdate];
}

- (void)lightMode {
    init_function();
    [[NSNotificationCenter defaultCenter] postNotificationName:@"lightModeSettings" object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"lightModeCredits" object:self];
    
    [self.darkModeButton setImage:[UIImage imageNamed:@"DarkMode-Light"] forState:UIControlStateNormal];
    [self.settingsButton setImage:[UIImage imageNamed:@"Settings-Light"] forState:UIControlStateNormal];
    [self.exploitProgressLabel setTextColor:[UIColor blackColor]];
    [self.exploitMessageLabel setTextColor:[UIColor blackColor]];
    [self.u0Label setTextColor:[UIColor blackColor]];
    [self.jailbreakLabel setTextColor:[UIColor blackColor]];
    [self.byLabel setTextColor:[UIColor blackColor]];
    [self.UIByLabel setTextColor:[UIColor blackColor]];
    [self.firstAndLabel setTextColor:[UIColor blackColor]];
    [self.fourthAndLabel setTextColor:[UIColor blackColor]];
    [self.uncoverLabel setTextColor:[UIColor blackColor]];
    [self.supportedOSLabel setTextColor:[UIColor blackColor]];
    [self.uOVersionLabel setTextColor:[UIColor blackColor]];
    [self.outputView setTextColor:[UIColor blackColor]];
    [self.backgroundView setBackgroundColor:[UIColor.whiteColor colorWithAlphaComponent:0.84]];
    [self.settingsNavBar setTintColor:[UIColor blackColor]];
    [self.settingsNavBar setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor blackColor]}];
    [self.settingsNavBar setLargeTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor blackColor]}];
    [self.creditsNavBar setTintColor:[UIColor blackColor]];
    [self.creditsNavBar setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor blackColor]}];
    [self.creditsNavBar setLargeTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor blackColor]}];
    self.jailbreakProgressBar.trackTintColor = [UIColor lightGrayColor];
    [self setNeedsStatusBarAppearanceUpdate];
}

- (IBAction)enableDarkMode:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->dark_mode = !prefs->dark_mode;
    set_prefs(prefs);
    [UIView animateWithDuration:0.5 delay:0 usingSpringWithDamping:1 initialSpringVelocity:1 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        init_function();
        if (prefs->dark_mode) {
            [self darkMode];
        } else {
            [self lightMode];
        }
    } completion:nil];
    release_prefs(&prefs);
}

- (void)didReceiveMemoryWarning {
    init_function();
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (UIStatusBarStyle)preferredStatusBarStyle {
    init_function();
    prefs_t *prefs = copy_prefs();
    UIStatusBarStyle statusBarStyle = prefs->dark_mode ? UIStatusBarStyleLightContent : UIStatusBarStyleDefault;
    release_prefs(&prefs);
    return statusBarStyle;
}

- (IBAction)openSettings:(id)sender {
    init_function();
    [UIView animateWithDuration:0.5 delay:0 usingSpringWithDamping:1 initialSpringVelocity:1 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        init_function();
        self.settingsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
        self.settingsView.alpha = 1;
        self.mainView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.3, 1.3);
        self.mainView.alpha = 0;
    } completion:nil];
}

- (void) showSpeicalThanks:(NSNotification *) notification {
    init_function();
    [UIView animateWithDuration:0.5 delay:0 usingSpringWithDamping:1 initialSpringVelocity:1 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        init_function();
        self.creditsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
        self.creditsView.alpha = 1;
        self.settingsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.3, 1.3);
        self.settingsView.alpha = 0;
    } completion:nil];
}

- (IBAction)dismissSpeicalThanks:(id)sender{
    init_function();
    [UIView animateWithDuration:0.5 delay:0 usingSpringWithDamping:1 initialSpringVelocity:1 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        init_function();
        self.settingsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
        self.settingsView.alpha = 1;
        self.creditsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.7, 0.7);
        self.creditsView.alpha = 0;
    } completion:nil];
}

- (IBAction)closeSettings:(id)sender{
    init_function();
    [UIView animateWithDuration:0.5 delay:0 usingSpringWithDamping:1 initialSpringVelocity:1 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        init_function();
        self.mainView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
        self.mainView.alpha = 1;
        self.settingsView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.7, 0.7);
        self.settingsView.alpha = 0;
    } completion:nil];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"dismissKeyboard" object:self];

}

- (IBAction)tappedOnPwn:(id)sender{
    init_function();
    [[UIApplication sharedApplication] openURL:[CreditsTableViewController getURLForUserName:@"Pwn20wnd"] options:@{} completionHandler:nil];
}

- (IBAction)tappedOnSamB:(id)sender{
    init_function();
    [[UIApplication sharedApplication] openURL:[CreditsTableViewController getURLForUserName:@"sbingner"] options:@{} completionHandler:nil];
}

- (IBAction)tappendOnJoonwoo:(id)sender{
    init_function();
    [[UIApplication sharedApplication] openURL:[CreditsTableViewController getURLForUserName:@"iOS_App_Dev"] options:@{} completionHandler:nil];
}
- (IBAction)tappendOnUbik:(id)sender{
    init_function();
    [[UIApplication sharedApplication] openURL:[CreditsTableViewController getURLForUserName:@"HiMyNameIsUbik"] options:@{} completionHandler:nil];
}


// This intentionally returns nil if called before it's been created by a proper init
+(JailbreakViewController *)sharedController {
    init_function();
    return sharedController;
}

-(void)updateOutputView {
    init_function();
    [self updateOutputViewFromQueue:@NO];
}

-(void)updateOutputViewFromQueue:(NSNumber*)fromQueue {
    init_function();
    static BOOL updateQueued = NO;
    static struct timeval last = {0,0};
    
    if (updateQueue == nil) {
        updateQueue = [NSOperationQueue new];
        updateQueue.qualityOfService = NSQualityOfServiceUserInteractive;
		updateQueue.maxConcurrentOperationCount = 1;
    }
    
    [updateQueue addOperationWithBlock:^{
        init_function();
        struct timeval now;

        if (fromQueue.boolValue) {
            updateQueued = NO;
        }
        
        if (updateQueued) {
            return;
        }
        
        if (gettimeofday(&now, NULL)) {
            LOG("gettimeofday failed");
            return;
        }
        
        uint64_t elapsed = (now.tv_sec - last.tv_sec) * 1000000 + now.tv_usec - last.tv_usec;
        // 30 FPS
        if (elapsed > 1000000/30) {
            updateQueued = NO;
            gettimeofday(&last, NULL);
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                init_function();
                self.outputView.text = output;
                [self.outputView scrollRangeToVisible:NSMakeRange(self.outputView.text.length, 0)];
            }];
        } else {
            NSTimeInterval waitTime = ((1000000/30) - elapsed) / 1000000.0;
            updateQueued = YES;
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                init_function();
                [self performSelector:@selector(updateOutputViewFromQueue:) withObject:@YES afterDelay:waitTime];
            }];
        }
    }];
}

-(void)appendTextToOutput:(NSString *)text {
    init_function();
    if (_outputView == nil || text == nil) {
        return;
    }
    static NSRegularExpression *remove = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        remove = [NSRegularExpression regularExpressionWithPattern:@"^\\d{4}-\\d{2}-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}\\.\\d+[-+\\d\\s]+\\S+\\[\\d+:\\d+\\]\\s+"
                                                           options:NSRegularExpressionAnchorsMatchLines error:nil];
        output = [NSMutableString new];
    });
    
    text = [remove stringByReplacingMatchesInString:text options:0 range:NSMakeRange(0, text.length) withTemplate:@""];

    @synchronized (output) {
        [output appendString:text];
    }
    [self updateOutputView];
}

- (id)initWithCoder:(NSCoder *)aDecoder {
    init_function();
    @synchronized(sharedController) {
        if (sharedController == nil) {
            sharedController = [super initWithCoder:aDecoder];
        }
    }
    self = sharedController;
    return self;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    init_function();
    @synchronized(sharedController) {
        if (sharedController == nil) {
            sharedController = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
        }
    }
    self = sharedController;
    return self;
}

- (id)init {
    init_function();
    @synchronized(sharedController) {
        if (sharedController == nil) {
            sharedController = [super init];
        }
    }
    self = sharedController;
    return self;
}

@end
