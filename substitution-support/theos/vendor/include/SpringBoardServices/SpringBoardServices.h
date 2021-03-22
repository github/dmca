#include <sys/cdefs.h>

__BEGIN_DECLS

mach_port_t SBSSpringBoardServerPort();

void SBFrontmostApplicationDisplayIdentifier(mach_port_t port, char *result);
NSString *SBSCopyFrontmostApplicationDisplayIdentifier();
void SBGetScreenLockStatus(mach_port_t port, BOOL *lockStatus, BOOL *passcodeEnabled);
void SBSUndimScreen();

int SBSLaunchApplicationWithIdentifierAndURLAndLaunchOptions(NSString *bundleIdentifier, NSURL *url, NSDictionary *appOptions, NSDictionary *launchOptions, BOOL suspended);
int SBSLaunchApplicationWithIdentifierAndLaunchOptions(NSString *bundleIdentifier, NSDictionary *appOptions, NSDictionary *launchOptions, BOOL suspended);
bool SBSOpenSensitiveURLAndUnlock(CFURLRef url, char flags);

extern NSString *const SBSApplicationLaunchOptionUnlockDeviceKey;

__END_DECLS
