//
//  DetectDebugger.cpp
//  ADVobfuscator
//
//  Created by sebastien on 22/09/14.
//  Copyright (c) 2014 Andrivet. All rights reserved.
//

// Only implemented for Apple platforms (it is just an example)
#ifdef __APPLE__

// https://developer.apple.com/library/mac/qa/qa1361/_index.html

#include <unistd.h>
#include <sys/sysctl.h>

bool AmIBeingDebugged()
// Returns true if the current process is being debugged (either running under the debugger or has a debugger attached post facto).
{
    // Note: It is possible to obfuscate this with ADVobfuscator (like the calls to getpid and sysctl)
    
    kinfo_proc info;
    info.kp_proc.p_flag = 0;
    
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()};
   
    size_t size = sizeof(info);
    sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    
    return (info.kp_proc.p_flag & P_TRACED) != 0;
}

#endif
