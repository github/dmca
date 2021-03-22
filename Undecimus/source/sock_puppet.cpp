//
//  sock_puppet.c
//  Undecimus
//
//  Created by Pwn20wnd on 7/12/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#include "sock_puppet.h"
extern "C" {
#include "iosurface.h"
#include "kernel_memory.h"
#include "parameters.h"
#include "KernelMemory.h"
}

#include "SockPuppet.hpp"

extern "C" bool sock_puppet(void)
{
    int ret = false;
    mach_port_t task_port = MACH_PORT_NULL;
    Azad::Exploits::SockPuppet exploit;
    if (!parameters_init())
        goto out;
    for (int i = 0; i < 5; i++) {
        try {
            if (exploit.run()) {
                break;
            }
            continue;
        } catch (const std:: exception&) {
            continue;
        }
    }
    task_port = exploit.getFakeKernelTaskPort();
    if (!MACH_PORT_VALID(task_port))
        goto out;
    prepare_rwk_via_tfp0(task_port);
    ret = true;
out:;
    return ret;
}
