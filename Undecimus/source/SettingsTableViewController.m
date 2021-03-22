//
//  SettingsTableViewController.m
//  Undecimus
//
//  Created by Pwn20wnd on 9/14/18.
//  Copyright © 2018 - 2019 Pwn20wnd. All rights reserved.
//

#include <sys/utsname.h>
#include <sys/sysctl.h>
#import "SettingsTableViewController.h"
#include <common.h>
#include "hideventsystem.h"
#include "remote_call.h"
#include "JailbreakViewController.h"
#include "utils.h"
#include "voucher_swap-poc.h"
#include "necp.h"
#include "kalloc_crash.h"
#include "prefs.h"
#include "diagnostics.h"
#include "exploits.h"
#include "substitutors.h"
#include <objc/runtime.h>

#define getIvar(type, self, name) ({ \
	id _self = (self); \
	Ivar ivar = class_getInstanceVariable(object_getClass(_self), name); \
	void *pointer = (ivar == NULL ? NULL : (char *)((__bridge void *)_self) + ivar_getOffset(ivar)); \
	(__bridge type)*(void **)pointer; \
})

@interface SettingsTableViewController ()

@end

@implementation SettingsTableViewController

// https://github.com/Matchstic/ReProvision/blob/7b595c699335940f68702bb204c5aa55b8b1896f/Shared/Application%20Database/RPVApplication.m#L102

+ (NSDictionary *)provisioningProfileAtPath:(NSString *)path {
    NSString *stringContent = [NSString stringWithContentsOfFile:path encoding:NSASCIIStringEncoding error:nil];
    NSString *startMarker = @"<plist";
    NSString *endMarker = @"</plist>";
    NSRange startRange = [stringContent rangeOfString:startMarker];
    NSRange endRange = [stringContent rangeOfString:endMarker];
    NSInteger length = (endRange.location + endMarker.length) - startRange.location;
    stringContent = [stringContent substringWithRange:NSMakeRange(startRange.location, length)];
    NSData *stringData = [stringContent dataUsingEncoding:NSUTF8StringEncoding];
    NSPropertyListFormat format;
    id plist = [NSPropertyListSerialization propertyListWithData:stringData options:NSPropertyListImmutable format:&format error:nil];
    return plist;
}

- (void)viewWillAppear:(BOOL)animated {
    init_function();
    [super viewWillAppear:animated];
    [self reloadData];
}

- (void)viewDidLoad {
    init_function();
    [super viewDidLoad];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(darkModeSettings:) name:@"darkModeSettings" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(lightModeSettings:) name:@"lightModeSettings" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(dismissKeyboardFromDoneButton:) name:@"dismissKeyboard" object:nil];
    [self.bootNonceTextField setDelegate:self];
    [self.bootNonceTextField setAutocorrectionType:UITextAutocorrectionTypeNo];
    [self.kernelExploitTextField.inputAssistantItem setLeadingBarButtonGroups:@[]];
    [self.kernelExploitTextField.inputAssistantItem setTrailingBarButtonGroups:@[]];
    [self.kernelExploitTextField setDelegate:self];
    [self.kernelExploitTextField setAutocorrectionType:UITextAutocorrectionTypeNo];
    [self.codeSubstitutorTextField.inputAssistantItem setLeadingBarButtonGroups:@[]];
    [self.codeSubstitutorTextField.inputAssistantItem setTrailingBarButtonGroups:@[]];
    [self.codeSubstitutorTextField setDelegate:self];
    [self.codeSubstitutorTextField setAutocorrectionType:UITextAutocorrectionTypeNo];
    self.tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(userTappedAnyware:)];
    self.tap.cancelsTouchesInView = NO;
    [self.view addGestureRecognizer:self.tap];
    self.exploits = [NSMutableArray new];
    iterate_evaluated_exploits(^(exploit_t *kernel_exploit) {
        if (!(kernel_exploit->exploit_support & EXPLOIT_SUPPORT_JAILBREAK)) return;
        [self.exploits addObject:@(kernel_exploit->name)];
    });
    self.substitutors = [NSMutableArray new];
    iterate_evaluated_substitutors(^(code_substitutor_t *substitutor) {
        if (!(substitutor->substitutor_support & SUBSTITUTOR_SUPPORT_INJECTION)) return;
        [self.substitutors addObject:@(substitutor->name)];
    });
    self.kernelExploitPickerView = [[UIPickerView alloc] init];
    [self.kernelExploitPickerView setDataSource:self];
    [self.kernelExploitPickerView setDelegate:self];
    self.codeSubstitutorPickerView = [[UIPickerView alloc] init];
    [self.codeSubstitutorPickerView setDataSource:self];
    [self.codeSubstitutorPickerView setDelegate:self];
    [self.kernelExploitTextField setInputView:_kernelExploitPickerView];
    [self.codeSubstitutorTextField setInputView:_codeSubstitutorPickerView];
    self.exploitPickerToolbar = [[UIToolbar alloc] initWithFrame:CGRectMake(0, 0, 320, 56)];
    [self.exploitPickerToolbar setBarStyle:UIBarStyleDefault];
    [self.exploitPickerToolbar sizeToFit];
    self.substitutorPickerToolbar = [[UIToolbar alloc] initWithFrame:CGRectMake(0, 0, 320, 56)];
    [self.substitutorPickerToolbar setBarStyle:UIBarStyleDefault];
    [self.substitutorPickerToolbar sizeToFit];
    UIBarButtonItem *exploitPickerAlignRight = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
    UIBarButtonItem *exploitPickerDoneButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(exploitPickerDoneAction)];
    [self.exploitPickerToolbar setItems:[NSArray arrayWithObjects:exploitPickerAlignRight, exploitPickerDoneButtonItem, nil] animated:NO];
    [self.kernelExploitTextField setInputAccessoryView:_exploitPickerToolbar];
    UIBarButtonItem *substitutorPickerAlignRight = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
    UIBarButtonItem *substitutorPickerDoneButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(substitutorPickerDoneAction)];
    [self.substitutorPickerToolbar setItems:[NSArray arrayWithObjects:substitutorPickerAlignRight, substitutorPickerDoneButtonItem, nil] animated:NO];
    [self.codeSubstitutorTextField setInputAccessoryView:_substitutorPickerToolbar];
    self.isPicking = NO;
}

-(void)dismissKeyboardFromDoneButton:(NSNotification *) notification {
    init_function();
    [self.view endEditing:YES];
}

-(void)darkModeSettings:(NSNotification *) notification {
    init_function();
    [self.bootNonceTextField setKeyboardAppearance:UIKeyboardAppearanceDark];
    [self.specialThanksLabel setTextColor:[UIColor whiteColor]];
    [self.tweakInjectionLabel setTextColor:[UIColor whiteColor]];
    [self.loadDaemonsLabel setTextColor:[UIColor whiteColor]];
    [self.dumpAPTicketLabel setTextColor:[UIColor whiteColor]];
    [self.refreshIconCacheLabel setTextColor:[UIColor whiteColor]];
    [self.disableAutoUpdatesLabel setTextColor:[UIColor whiteColor]];
    [self.disableAppRevokesLabel setTextColor:[UIColor whiteColor]];
    [self.overwriteBootNonceLabel setTextColor:[UIColor whiteColor]];
    [self.exportKernelTaskPortLabel setTextColor:[UIColor whiteColor]];
    [self.restoreRootFSLabel setTextColor:[UIColor whiteColor]];
    [self.installCydiaLabel setTextColor:[UIColor whiteColor]];
    [self.installSSHLabel setTextColor:[UIColor whiteColor]];
    [self.increaseMemoryLimitLabel setTextColor:[UIColor whiteColor]];
    [self.reloadSystemDaemonsLabel setTextColor:[UIColor whiteColor]];
    [self.hideLogWindowLabel setTextColor:[UIColor whiteColor]];
    [self.resetCydiaCacheLabel setTextColor:[UIColor whiteColor]];
    [self.sshOnlyLabel setTextColor:[UIColor whiteColor]];
    [self.enableGetTaskAllowLabel setTextColor:[UIColor whiteColor]];
    [self.setCSDebuggedLabel setTextColor:[UIColor whiteColor]];
    [self.autoRespringLabel setTextColor:[UIColor whiteColor]];
    [self.readOnlyRootFSLabel setTextColor:[UIColor whiteColor]];
    [self.kernelExploitLabel setTextColor:[UIColor whiteColor]];
    [self.codeSubstitutorLabel setTextColor:[UIColor whiteColor]];
    [self.bootNonceButton setTitleColor:[UIColor whiteColor] forState:normal];
    [self.bootNonceTextField setTintColor:[UIColor whiteColor]];
    [self.bootNonceTextField setTextColor:[UIColor whiteColor]];
    [self.kernelExploitTextField setTintColor:[UIColor whiteColor]];
    [self.codeSubstitutorTextField setTintColor:[UIColor whiteColor]];
	[getIvar(id, self.bootNonceTextField, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
	[getIvar(id, self.kernelExploitTextField, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
	[getIvar(id, self.codeSubstitutorTextField, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
	[getIvar(id, self.ecidLabel, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
    [self.ecidDarkModeButton setTitleColor:[UIColor darkGrayColor] forState:normal];
    [self.expiryDarkModeLabel setTextColor:[UIColor darkGrayColor]];
	[getIvar(id, self.expiryLabel, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
	[getIvar(id, self.uptimeLabel, "_placeholderLabel") setTextColor:[UIColor darkGrayColor]];
    [self.upTimeLabel setTextColor:[UIColor whiteColor]];
    [self.exploitPickerToolbar setBarTintColor:[UIColor darkTextColor]];
    [self.substitutorPickerToolbar setBarTintColor:[UIColor darkTextColor]];
    [self.kernelExploitPickerView setBackgroundColor:[UIColor blackColor]];
    [self.codeSubstitutorPickerView setBackgroundColor:[UIColor blackColor]];
    [JailbreakViewController.sharedController.navigationController.navigationBar setLargeTitleTextAttributes:@{ NSForegroundColorAttributeName : [UIColor whiteColor] }];
}

-(void)lightModeSettings:(NSNotification *) notification {
    init_function();
    [self.bootNonceTextField setKeyboardAppearance:UIKeyboardAppearanceLight];
    [self.specialThanksLabel setTextColor:[UIColor blackColor]];
    [self.tweakInjectionLabel setTextColor:[UIColor blackColor]];
    [self.loadDaemonsLabel setTextColor:[UIColor blackColor]];
    [self.dumpAPTicketLabel setTextColor:[UIColor blackColor]];
    [self.refreshIconCacheLabel setTextColor:[UIColor blackColor]];
    [self.disableAutoUpdatesLabel setTextColor:[UIColor blackColor]];
    [self.disableAppRevokesLabel setTextColor:[UIColor blackColor]];
    [self.overwriteBootNonceLabel setTextColor:[UIColor blackColor]];
    [self.exportKernelTaskPortLabel setTextColor:[UIColor blackColor]];
    [self.restoreRootFSLabel setTextColor:[UIColor blackColor]];
    [self.installCydiaLabel setTextColor:[UIColor blackColor]];
    [self.installSSHLabel setTextColor:[UIColor blackColor]];
    [self.increaseMemoryLimitLabel setTextColor:[UIColor blackColor]];
    [self.reloadSystemDaemonsLabel setTextColor:[UIColor blackColor]];
    [self.hideLogWindowLabel setTextColor:[UIColor blackColor]];
    [self.resetCydiaCacheLabel setTextColor:[UIColor blackColor]];
    [self.sshOnlyLabel setTextColor:[UIColor blackColor]];
    [self.enableGetTaskAllowLabel setTextColor:[UIColor blackColor]];
    [self.setCSDebuggedLabel setTextColor:[UIColor blackColor]];
    [self.autoRespringLabel setTextColor:[UIColor blackColor]];
    [self.readOnlyRootFSLabel setTextColor:[UIColor blackColor]];
    [self.kernelExploitLabel setTextColor:[UIColor blackColor]];
    [self.codeSubstitutorLabel setTextColor:[UIColor blackColor]];
    [self.bootNonceButton setTitleColor:[UIColor blackColor] forState:normal];
    [self.bootNonceTextField setTintColor:[UIColor blackColor]];
    [self.bootNonceTextField setTextColor:[UIColor blackColor]];
    [self.kernelExploitTextField setTintColor:[UIColor blackColor]];
    [self.codeSubstitutorTextField setTintColor:[UIColor blackColor]];
	[getIvar(id, self.bootNonceTextField, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
	[getIvar(id, self.kernelExploitTextField, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
	[getIvar(id, self.codeSubstitutorTextField, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
	[getIvar(id, self.ecidLabel, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
    [self.ecidDarkModeButton setTitleColor:[UIColor blackColor] forState:normal];
    [self.expiryDarkModeLabel setTextColor:[UIColor blackColor]];
	[getIvar(id, self.expiryLabel, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
	[getIvar(id, self.uptimeLabel, "_placeholderLabel") setTextColor:[UIColor lightGrayColor]];
    [self.upTimeLabel setTextColor:[UIColor blackColor]];
    [self.exploitPickerToolbar setBarTintColor:[UIColor lightTextColor]];
    [self.substitutorPickerToolbar setBarTintColor:[UIColor lightTextColor]];
    [self.kernelExploitPickerView setBackgroundColor:[UIColor whiteColor]];
    [self.codeSubstitutorPickerView setBackgroundColor:[UIColor whiteColor]];
    [JailbreakViewController.sharedController.navigationController.navigationBar setLargeTitleTextAttributes:@{ NSForegroundColorAttributeName : [UIColor blackColor] }];
}

- (void)userTappedAnyware:(UITapGestureRecognizer *) sender
{
    init_function();
    if (!self.isPicking){
        [self.view endEditing:YES];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    init_function();
    [textField resignFirstResponder];
    return YES;
}

- (void)reloadData {
    init_function();
    prefs_t *prefs = copy_prefs();
    [self.tweakInjectionSwitch setOn:(BOOL)prefs->load_tweaks];
    [self.loadDaemonsSwitch setOn:(BOOL)prefs->load_daemons];
    [self.dumpAPTicketSwitch setOn:(BOOL)prefs->dump_apticket];
    [self.bootNonceTextField setPlaceholder:@(prefs->boot_nonce)];
    [self.bootNonceTextField setText:nil];
    [self.refreshIconCacheSwitch setOn:(BOOL)prefs->run_uicache];
    [self.disableAutoUpdatesSwitch setOn:(BOOL)prefs->disable_auto_updates];
    [self.disableAppRevokesSwitch setOn:(BOOL)prefs->disable_app_revokes];
    [self.kernelExploitTextField setText:nil];
    [self.kernelExploitTextField setPlaceholder:prefs->kernel_exploit != NULL ? @(prefs->kernel_exploit) : @"Unavailable"];
    [self.kernelExploitTextField setEnabled:prefs->kernel_exploit != NULL ? YES : NO];
    [self.codeSubstitutorTextField setText:nil];
    [self.codeSubstitutorTextField setPlaceholder:prefs->code_substitutor != NULL ? @(prefs->code_substitutor) : @"Unavailable"];
    [self.codeSubstitutorTextField setEnabled:prefs->code_substitutor != NULL ? YES : NO];
    [self.openCydiaButton setEnabled:(BOOL)cydiaIsInstalled()];
    @try {
        [self.expiryLabel setPlaceholder:[NSString stringWithFormat:@"%d %@", (int)[[SettingsTableViewController provisioningProfileAtPath:[[NSBundle mainBundle] pathForResource:@"embedded" ofType:@"mobileprovision"]][@"ExpirationDate"] timeIntervalSinceDate:[NSDate date]] / 86400, localize(@"Days")]];
    } @catch (NSException *exception) {
        (void)exception;
        [self.expiryLabel setPlaceholder:@"Unavailable"];
    }
    [self.overwriteBootNonceSwitch setOn:(BOOL)prefs->overwrite_boot_nonce];
    [self.exportKernelTaskPortSwitch setOn:(BOOL)prefs->export_kernel_task_port];
    [self.restoreRootFSSwitch setOn:(BOOL)prefs->restore_rootfs];
    [self.uptimeLabel setPlaceholder:[NSString stringWithFormat:@"%d %@", (int)getUptime() / 86400, localize(@"Days")]];
    [self.increaseMemoryLimitSwitch setOn:(BOOL)prefs->increase_memory_limit];
    [self.installSSHSwitch setOn:(BOOL)prefs->install_openssh];
    [self.installCydiaSwitch setOn:(BOOL)prefs->install_cydia];
    if (prefs->ecid) [self.ecidLabel setPlaceholder:hexFromInt([@(prefs->ecid) integerValue])];
    [self.reloadSystemDaemonsSwitch setOn:(BOOL)prefs->reload_system_daemons];
    [self.hideLogWindowSwitch setOn:(BOOL)prefs->hide_log_window];
    [self.resetCydiaCacheSwitch setOn:(BOOL)prefs->reset_cydia_cache];
    [self.sshOnlySwitch setOn:(BOOL)prefs->ssh_only];
    [self.enableGetTaskAllowSwitch setOn:(BOOL)prefs->enable_get_task_allow];
    [self.setCSDebuggedSwitch setOn:(BOOL)prefs->set_cs_debugged];
    [self.autoRespringSwitch setOn:(BOOL)prefs->auto_respring];
    [self.readOnlyRootFSSwitch setOn:(BOOL)prefs->read_only_rootfs];
    [self.restartSpringBoardButton setEnabled:has_exploit_support(EXPLOIT_SUPPORT_RESPRING)];
    [self.restartButton setEnabled:has_exploit_support(EXPLOIT_SUPPORT_PANIC)];
    release_prefs(&prefs);
    [JailbreakViewController.sharedController updateStatus];
    [self.tableView reloadData];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    init_function();
    if (indexPath.row == 0) {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"showSpecialThanks" object:self];
    }
    
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (IBAction)selectedSpecialThanks:(id)sender {
    init_function();
    [[NSNotificationCenter defaultCenter] postNotificationName:@"showSpecialThanks" object:self];
}

- (IBAction)tweakInjectionSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->load_tweaks = (bool)self.tweakInjectionSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)loadDaemonsSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->load_daemons = (bool)self.loadDaemonsSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)dumpAPTicketSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->dump_apticket = (bool)self.dumpAPTicketSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)bootNonceTextFieldEditingDidEnd:(id)sender {
    init_function();
    uint64_t val = 0;
    if ([[NSScanner scannerWithString:[self.bootNonceTextField text]] scanHexLongLong:&val] && val != HUGE_VAL && val != -HUGE_VAL) {
        prefs_t *prefs = copy_prefs();
        prefs->boot_nonce = [NSString stringWithFormat:@ADDR, val].UTF8String;
        set_prefs(prefs);
        release_prefs(&prefs);
    } else {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:localize(@"Invalid Entry") message:localize(@"The boot nonce entered could not be parsed") preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *OK = [UIAlertAction actionWithTitle:localize(@"OK") style:UIAlertActionStyleDefault handler:nil];
        [alertController addAction:OK];
        [self presentViewController:alertController animated:YES completion:nil];
    }
    [self reloadData];
}

- (IBAction)refreshIconCacheSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->run_uicache = (bool)self.refreshIconCacheSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    init_function();
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    init_function();
    NSInteger count = 0;
    if (pickerView == _kernelExploitPickerView) {
        count = [self.exploits count];
    } else if (pickerView == _codeSubstitutorPickerView) {
        count = [self.substitutors count];
    }
    return count;
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    init_function();
    NSString *title = nil;
    if (pickerView == _kernelExploitPickerView) {
        title = [self.exploits objectAtIndex:row];
    } else if (pickerView == _codeSubstitutorPickerView) {
        title = [self.substitutors objectAtIndex:row];
    }
    return title;
}

- (NSAttributedString *)pickerView:(UIPickerView *)pickerView attributedTitleForRow:(NSInteger)row forComponent:(NSInteger)component {
    init_function();
    NSString *title = nil;
    if (pickerView == _kernelExploitPickerView) {
        title = [self.exploits objectAtIndex:row];
    } else if (pickerView == _codeSubstitutorPickerView) {
        title = [self.substitutors objectAtIndex:row];
    }
    if (title == nil) {
        return nil;
    }
    prefs_t *prefs = copy_prefs();
    NSDictionary *attributes = @{NSForegroundColorAttributeName : prefs->dark_mode ? [UIColor whiteColor] : [UIColor blackColor] };
    release_prefs(&prefs);
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithString:title attributes:attributes];
    return attributedString;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    init_function();
    self.isPicking = YES;
}

- (void)exploitPickerDoneAction {
    init_function();
    self.isPicking = NO;
    prefs_t *prefs = copy_prefs();
    prefs->kernel_exploit = [[_exploits objectAtIndex:[[self kernelExploitPickerView] selectedRowInComponent:0]] UTF8String];
    set_prefs(prefs);
    release_prefs(&prefs);
    [[self kernelExploitTextField] resignFirstResponder];
    [self reloadData];
}

- (void)substitutorPickerDoneAction {
    init_function();
    self.isPicking = NO;
    prefs_t *prefs = copy_prefs();
    prefs->code_substitutor = [[_substitutors objectAtIndex:[[self codeSubstitutorPickerView] selectedRowInComponent:0]] UTF8String];
    set_prefs(prefs);
    release_prefs(&prefs);
    [[self codeSubstitutorTextField] resignFirstResponder];
    [self reloadData];
}

- (IBAction)disableAppRevokesSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->disable_app_revokes = (bool)self.disableAppRevokesSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedOnRestart:(id)sender {
    init_function();
    void (^block)(void) = ^(void) {
        init_function();
        _notice(localize(@"The device will be restarted."), true, false);
        const char *support = get_best_exploit_by_name(EXPLOIT_SUPPORT_PANIC);
        if (strcmp(support, K_NECP) == 0) {
            necp_die();
        } else if (strcmp(support, K_VOUCHER_SWAP) == 0) {
            voucher_swap_poc();
        } else if (strcmp(support, K_KALLOC_CRASH) == 0) {
            do_kalloc_crash();
        }
        exit(EXIT_FAILURE);
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (IBAction)disableAutoUpdatesSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->disable_auto_updates = (bool)self.disableAutoUpdatesSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedOnShareDiagnosticsData:(id)sender {
    init_function();
    NSURL *URL = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/Documents/diagnostics.plist", NSHomeDirectory()]];
    [getDiagnostics() writeToURL:URL error:nil];
    UIActivityViewController *activityViewController = [[UIActivityViewController alloc] initWithActivityItems:@[URL] applicationActivities:nil];
    if ([activityViewController respondsToSelector:@selector(popoverPresentationController)]) {
        [[activityViewController popoverPresentationController] setSourceView:self.shareDiagnosticsDataButton];
    }
    [self presentViewController:activityViewController animated:YES completion:nil];
}

- (IBAction)tappedOnOpenCydia:(id)sender {
    init_function();
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"cydia://"] options:@{} completionHandler:nil];
}

- (IBAction)tappedOnOpenGithub:(id)sender {
    init_function();
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://github.com/pwn20wndstuff/Undecimus"] options:@{} completionHandler:nil];
}

- (IBAction)overwriteBootNonceSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->overwrite_boot_nonce = (bool)self.overwriteBootNonceSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedOnCopyNonce:(id)sender{
    init_function();
    UIAlertController *copyBootNonceAlert = [UIAlertController alertControllerWithTitle:localize(@"Copy boot nonce?") message:localize(@"Would you like to copy nonce generator to clipboard?") preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *copyAction = [UIAlertAction actionWithTitle:localize(@"Yes") style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        prefs_t *prefs = copy_prefs();
        [[UIPasteboard generalPasteboard] setString:@(prefs->boot_nonce)];
        release_prefs(&prefs);
    }];
    UIAlertAction *noAction = [UIAlertAction actionWithTitle:localize(@"No") style:UIAlertActionStyleCancel handler:nil];
    [copyBootNonceAlert addAction:copyAction];
    [copyBootNonceAlert addAction:noAction];
    [self presentViewController:copyBootNonceAlert animated:TRUE completion:nil];
}

- (IBAction)tappedOnCopyECID:(id)sender {
    init_function();
    UIAlertController *copyBootNonceAlert = [UIAlertController alertControllerWithTitle:localize(@"Copy ECID?") message:localize(@"Would you like to copy ECID to clipboard?") preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *copyAction = [UIAlertAction actionWithTitle:localize(@"Yes") style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        prefs_t *prefs = copy_prefs();
        [[UIPasteboard generalPasteboard] setString:hexFromInt(@(prefs->ecid).integerValue)];
        release_prefs(&prefs);
    }];
    UIAlertAction *noAction = [UIAlertAction actionWithTitle:localize(@"No") style:UIAlertActionStyleCancel handler:nil];
    [copyBootNonceAlert addAction:copyAction];
    [copyBootNonceAlert addAction:noAction];
    [self presentViewController:copyBootNonceAlert animated:TRUE completion:nil];
}

- (IBAction)tappedOnCheckForUpdate:(id)sender {
    init_function();
    void (^block)(void) = ^(void) {
        NSString *update = [NSString stringWithContentsOfURL:[NSURL URLWithString:@"https://github.com/pwn20wndstuff/Undecimus/raw/master/Update.txt"] encoding:NSUTF8StringEncoding error:nil];
        if (update == nil) {
            _notice(localize(@"Failed to check for update."), true, false);
        } else if ([update compare:appVersion() options:NSNumericSearch] == NSOrderedDescending) {
            _notice(localize(@"An update is available."), true, false);
        } else {
            _notice(localize(@"Already up to date."), true, false);
        }
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (IBAction)exportKernelTaskPortSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->export_kernel_task_port = (bool)self.exportKernelTaskPortSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)restoreRootFSSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->restore_rootfs = (bool)self.restoreRootFSSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)installCydiaSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->install_cydia = (bool)self.installCydiaSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)installSSHSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->install_openssh = (bool)self.installSSHSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (void)tableView:(UITableView *)tableView willDisplayFooterView:(UITableViewHeaderFooterView *)footerView forSection:(NSInteger)section {
    init_function();
    footerView.textLabel.text = [@"unc0ver " stringByAppendingString:appVersion()];
    footerView.textLabel.textAlignment = NSTextAlignmentCenter;
}

- (IBAction)increaseMemoryLimitSwitch:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->increase_memory_limit = (bool)self.increaseMemoryLimitSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedOnAutomaticallySelectExploit:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->kernel_exploit = get_best_exploit_by_name(EXPLOIT_SUPPORT_JAILBREAK);
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)reloadSystemDaemonsSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->reload_system_daemons = (bool)self.reloadSystemDaemonsSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedRestartSpringBoard:(id)sender {
    init_function();
    void (^block)(void) = ^(void) {
        _notice(localize(@"SpringBoard will be restarted."), true, false);
        const char *support = get_best_exploit_by_name(EXPLOIT_SUPPORT_RESPRING);
        if (strcmp(support, K_DEJA_XNU) == 0) {
            mach_port_t const bb_tp = hid_event_queue_exploit();
            _assert(MACH_PORT_VALID(bb_tp), localize(@"Unable to get task port for backboardd."));
            _assert(thread_call_remote(bb_tp, exit, 1, REMOTE_LITERAL(EXIT_SUCCESS)) == ERR_SUCCESS, localize(@"Unable to make backboardd exit."));
        }
        exit(EXIT_FAILURE);
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (IBAction)tappedOnCleanDiagnosticsData:(id)sender {
    init_function();
    cleanLogs();
    _notice(localize(@"Cleaned diagnostics data."), false, false);
}

- (IBAction)hideLogWindowSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->hide_log_window = (bool)self.hideLogWindowSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
    void (^block)(void) = ^(void) {
        _notice(localize(@"Preference was changed. The app will now exit."), true, false);
        exit(EXIT_SUCCESS);
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (IBAction)resetCydiaCacheSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->reset_cydia_cache = (bool)self.resetCydiaCacheSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)sshOnlySwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->ssh_only = (bool)self.sshOnlySwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)enableGetTaskAllowSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->enable_get_task_allow = (bool)self.enableGetTaskAllowSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)setCSDebugged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->set_cs_debugged = (bool)self.setCSDebuggedSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)setAutoRespring:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->auto_respring = (bool)self.autoRespringSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)readOnlyRootFSSwitchValueChanged:(id)sender {
    init_function();
    prefs_t *prefs = copy_prefs();
    prefs->read_only_rootfs = (bool)self.readOnlyRootFSSwitch.isOn;
    set_prefs(prefs);
    release_prefs(&prefs);
    [self reloadData];
}

- (IBAction)tappedOnResetAppPreferences:(id)sender {
    init_function();
    void (^block)(void) = ^(void) {
        init_function();
        reset_prefs();
        _notice(localize(@"Preferences were reset. The app will now exit."), true, false);
        exit(EXIT_SUCCESS);
    };
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul), block);
}

- (IBAction)tappedOnLoadTweaksInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Load Tweaks"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes Substrate load extensions that are commonly referred to as tweaks in newly started processes."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnLoadDaemonsInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Load Daemons"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak load the launch daemons located at /Library/LaunchDaemons and execute files located at /etc/rc.d."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnDumpAPTicketInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Dump APTicket"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak create a copy of the system APTicket located at /System/Library/Caches/apticket.der at its Documents directory which is accessible via iTunes File Sharing."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnRefreshIconCacheInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Refresh Icon Cache"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak regenerate SpringBoard's system application installation cache to cause newly installed .app bundles to appear on the icon list."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnDisableAutoUpdatesInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Disable Updates"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak effectively disable the system's software update mechanism to prevent the system from automatically upgrading to the latest available firmware which may not be supported by the jailbreak at that time."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnDisableAppRevokesInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Disable Revokes"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak effectively disable the system's online certificate status protocol system to prevent enterprise certificates which the jailbreak may be signed with from getting revoked."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-11.4.1 on arm64 SoCs (A7-A11)."),
              false,
              false);
}

- (IBAction)tappedOnOverwriteBootNonceInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Set Boot Nonce"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak set the persistent com.apple.System.boot-nonce variable in non-volatile random-access memory (NVRAM) which may be required to downgrade to an unsigned iOS firmware by using SHSH files."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnExportKernelTaskPortInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Export TFP0"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak modify the host-port to grant any process access to the host-priv-port."
                       "\n"
                       "This option effectively grants any process access to the kernel task port (TFP0) and allows re-jailbreaking without exploiting again."
                       "\n"
                       "This option is considered unsafe as the privilege this option effectively grants to processes can be used for bad purposes by malicious apps."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnRestoreRootFSInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Restore RootFS"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak restore the root filesystem (RootFS) to the snapshot which is created by the system when the device is restored."
                       "\n"
                       "This option effectively allows uninstalling the jailbreak without losing any user data."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnIncreaseMemoryLimitInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Max Memory Limit"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak modify the Jetsam configuration file to increase the memory limit that is enforced upon processes by Jetsam to the maximum value to effectively bypass that mechanism."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnInstallSSHInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"(Re)Install OpenSSH"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak (re)install the openssh package."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnInstallCydiaInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Reinstall Cydia"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes jailbreak reinstall the cydiainstaller package."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnReloadSystemDaemonsInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Reload Daemons"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak reload all of the running system daemons to make the Substrate extensions (tweaks) load in them."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnHideLogWindowInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Hide Log Window"),
              localize(@"Description:"
                       "\n\n"
                       "This option hides the log window or console in the jailbreak app for a more clean look."),
              false,
              false);
}

- (IBAction)tappedOnResetCydiaCacheInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Reset Cydia Cache"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak reset Cydia's cache."
                       "\n"
                       "This option will cause Cydia to regenerate the repo lists and its cache."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnSSHOnlyInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"SSH Only"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak skip installing Cydia and Substrate."
                       "\n"
                       "This option starts SSH on 127.0.0.1 (localhost) on port 22 via dropbear."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-12.1.2 on arm64/arm64e SoCs (A7-A12X)."),
              false,
              false);
}

- (IBAction)tappedOnEnableGetTaskAllowInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Set get-task-allow"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak dynamically enable the get-task-allow entitlement for every new process."
                       "\n"
                       "This option makes dyld treat the processes unrestricted."
                       "\n"
                       "This option enables dyld environment variables such as DYLD_INSERT_LIBRARIES."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-12.1.2 on arm64 SoCs (A7-A11)."),
              false,
              false);
}
- (IBAction)tappedOnCSDebuggedInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Set CS_DEBUGGED"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak dynamically set the CS_DEBUGGED codesign flag for every new process."
                       "\n"
                       "This option makes the kernel allow processes to run with invalid executable pages."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-12.1.2 on arm64 SoCs (A7-A11)."),
              false,
              false);
}
- (IBAction)tappedOnAutoRespringInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Auto Respring"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak automatically restart the SpringBoard as soon as the jailbreak process is completed without the confirmation."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-13.3 on arm64/arm64e SoCs (A7-A13)."),
              false,
              false);
}

- (IBAction)tappedOnReadOnlyRootFSInfoButton:(id)sender {
    init_function();
    showAlert(localize(@"Read-Only RootFS"),
              localize(@"Description:"
                       "\n\n"
                       "This option makes the jailbreak skip remounting the root filesystem as read-write when combined with the SSH-Only option."
                       "\n\n"
                       "Compatibility:"
                       "\n\n"
                       "iOS 11.0-12.4.1 on arm64/arm64e SoCs (A7-A12X)."),
              false,
              false);
}


- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    init_function();
    return 44;
}

- (void)didReceiveMemoryWarning {
    init_function();
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
