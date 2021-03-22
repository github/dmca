//
//  diagnostics.c
//  Undecimus
//
//  Created by Pwn20wnd on 5/3/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#include "diagnostics.h"
#include <common.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include "utils.h"
#include "prefs.h"

#if 0
Credits:
- https://github.com/lechium/nitoTV/blob/53cca06514e79279fa89639ad05b562f7d730079/Classes/packageManagement.m#L1138
- https://github.com/lechium/nitoTV/blob/53cca06514e79279fa89639ad05b562f7d730079/Classes/packageManagement.m#L1163
- https://github.com/lechium/nitoTV/blob/53cca06514e79279fa89639ad05b562f7d730079/Classes/packageManagement.m#L854
- https://github.com/lechium/nitoTV/blob/53cca06514e79279fa89639ad05b562f7d730079/Classes/packageManagement.m#L869
#endif

NSArray *dependencyArrayFromString(NSString *depends) {
    init_function();
    NSMutableArray *cleanArray = [NSMutableArray new];
    NSArray *dependsArray = [depends componentsSeparatedByString:@","];
    for (NSString *depend in dependsArray) {
        NSArray *spaceDelimitedArray = [depend componentsSeparatedByString:@" "];
        NSString *isolatedDependency = [[spaceDelimitedArray objectAtIndex:0] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        if ([isolatedDependency length] == 0) {
            isolatedDependency = [[spaceDelimitedArray objectAtIndex:1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        }
        [cleanArray addObject:isolatedDependency];
    }
    return cleanArray;
}

NSArray *parsedPackageArray() {
    init_function();
    NSString *packageString = [NSString stringWithContentsOfFile:STATUS_FILE encoding:NSUTF8StringEncoding error:nil];
    NSArray *lineArray = [packageString componentsSeparatedByString:@"\n\n"];
    NSMutableArray *mutableList = [[NSMutableArray alloc] init];
    for (NSString *currentItem in lineArray) {
        NSArray *packageArray = [currentItem componentsSeparatedByString:@"\n"];
        NSMutableDictionary *currentPackage = [[NSMutableDictionary alloc] init];
        for (NSString *currentLine in packageArray) {
            NSArray *itemArray = [currentLine componentsSeparatedByString:@": "];
            if ([itemArray count] >= 2) {
                NSString *key = [itemArray objectAtIndex:0];
                NSString *object = [itemArray objectAtIndex:1];
                if ([key isEqualToString:@"Depends"]) {
                    NSArray *dependsObject = dependencyArrayFromString(object);
                    [currentPackage setObject:dependsObject forKey:key];
                } else {
                    [currentPackage setObject:object forKey:key];
                }
            }
        }
        if ([[currentPackage allKeys] count] > 4) {
            [mutableList addObject:currentPackage];
        }
        currentPackage = nil;
    }
    NSSortDescriptor *nameDescriptor = [[NSSortDescriptor alloc] initWithKey:@"Name" ascending:YES selector:@selector(localizedCaseInsensitiveCompare:)];
    NSSortDescriptor *packageDescriptor = [[NSSortDescriptor alloc] initWithKey:@"Package" ascending:YES selector:@selector(localizedCaseInsensitiveCompare:)];
    NSArray *descriptors = [NSArray arrayWithObjects:nameDescriptor, packageDescriptor, nil];
    NSArray *sortedArray = [mutableList sortedArrayUsingDescriptors:descriptors];
    mutableList = nil;
    return sortedArray;
}

NSString *domainFromRepoObject(NSString *repoObject) {
    init_function();
    if ([repoObject length] == 0) return nil;
    NSArray *sourceObjectArray = [repoObject componentsSeparatedByString:@" "];
    NSString *url = [sourceObjectArray objectAtIndex:1];
    if ([url length] > 7) {
        NSString *urlClean = [url substringFromIndex:7];
        NSArray *secondArray = [urlClean componentsSeparatedByString:@"/"];
        return [secondArray objectAtIndex:0];
    }
    return nil;
}

NSArray *sourcesFromFile(NSString *theSourceFile) {
    init_function();
    NSMutableArray *finalArray = [NSMutableArray new];
    NSString *sourceString = [[NSString stringWithContentsOfFile:theSourceFile encoding:NSASCIIStringEncoding error:nil] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    NSArray *sourceFullArray =  [sourceString componentsSeparatedByString:@"\n"];
    NSEnumerator *sourceEnum = [sourceFullArray objectEnumerator];
    NSString *currentSource = nil;
    while (currentSource = [sourceEnum nextObject]) {
        NSString *theObject = domainFromRepoObject(currentSource);
        if (theObject != nil) {
            if (![finalArray containsObject:theObject])
                [finalArray addObject:theObject];
        }
    }
    return finalArray;
}

NSDictionary *getDiagnostics() {
    init_function();
    NSMutableDictionary *diagnostics = [NSMutableDictionary new];
    char *OSVersion = getOSVersion();
    assert(OSVersion != NULL);
    char *OSProductVersion = getOSProductVersion();
    assert(OSProductVersion != NULL);
    char *kernelVersion = getKernelVersion();
    assert(kernelVersion != NULL);
    char *machineName = getMachineName();
    assert(machineName != NULL);
    prefs_t *prefs = copy_prefs();
    diagnostics[@"OSVersion"] = @(OSVersion);
    diagnostics[@"OSProductVersion"] = @(OSProductVersion);
    diagnostics[@"KernelVersion"] = @(kernelVersion);
    diagnostics[@"MachineName"] = @(machineName);
    diagnostics[@"Preferences"] = [NSMutableDictionary new];
    diagnostics[@"Preferences"][@(K_TWEAK_INJECTION)] = @(prefs->load_tweaks);
    diagnostics[@"Preferences"][@(K_LOAD_DAEMONS)] = @(prefs->load_daemons);
    diagnostics[@"Preferences"][@(K_DUMP_APTICKET)] = @(prefs->dump_apticket);
    diagnostics[@"Preferences"][@(K_REFRESH_ICON_CACHE)] = @(prefs->run_uicache);
    diagnostics[@"Preferences"][@(K_BOOT_NONCE)] = @(prefs->boot_nonce);
    diagnostics[@"Preferences"][@(K_DISABLE_AUTO_UPDATES)] = @(prefs->disable_auto_updates);
    diagnostics[@"Preferences"][@(K_DISABLE_APP_REVOKES)] = @(prefs->disable_app_revokes);
    diagnostics[@"Preferences"][@(K_OVERWRITE_BOOT_NONCE)] = @(prefs->overwrite_boot_nonce);
    diagnostics[@"Preferences"][@(K_EXPORT_KERNEL_TASK_PORT)] = @(prefs->export_kernel_task_port);
    diagnostics[@"Preferences"][@(K_RESTORE_ROOTFS)] = @(prefs->restore_rootfs);
    diagnostics[@"Preferences"][@(K_INCREASE_MEMORY_LIMIT)] = @(prefs->increase_memory_limit);
    diagnostics[@"Preferences"][@(K_ECID)] = @(prefs->ecid);
    diagnostics[@"Preferences"][@(K_INSTALL_CYDIA)] = @(prefs->install_cydia);
    diagnostics[@"Preferences"][@(K_INSTALL_OPENSSH)] = @(prefs->install_openssh);
    diagnostics[@"Preferences"][@(K_RELOAD_SYSTEM_DAEMONS)] = @(prefs->reload_system_daemons);
    diagnostics[@"Preferences"][@(K_RESET_CYDIA_CACHE)] = @(prefs->reset_cydia_cache);
    diagnostics[@"Preferences"][@(K_SSH_ONLY)] = @(prefs->ssh_only);
    diagnostics[@"Preferences"][@(K_ENABLE_GET_TASK_ALLOW)] = @(prefs->enable_get_task_allow);
    diagnostics[@"Preferences"][@(K_SET_CS_DEBUGGED)] = @(prefs->set_cs_debugged);
    diagnostics[@"Preferences"][@(K_HIDE_LOG_WINDOW)] = @(prefs->hide_log_window);
    diagnostics[@"AppVersion"] = appVersion();
    diagnostics[@"LogFile"] = [NSString stringWithContentsOfFile:getLogFile() encoding:NSUTF8StringEncoding error:nil];
    diagnostics[@"Sources"] = sourcesFromFile(CYDIA_LIST);
    diagnostics[@"Packages"] = parsedPackageArray();
    diagnostics[@"Uptime"] = @(getUptime());
    SafeFreeNULL(OSVersion);
    SafeFreeNULL(OSProductVersion);
    SafeFreeNULL(kernelVersion);
    SafeFreeNULL(machineName);
    release_prefs(&prefs);
    return diagnostics;
}
