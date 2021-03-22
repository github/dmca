//
//  prefs.h
//  Undecimus
//
//  Created by Pwn20wnd on 5/3/19.
//  Copyright © 2019 Pwn20wnd. All rights reserved.
//

#ifndef prefs_h
#define prefs_h

#include <stdio.h>
#include <stdbool.h>

#define K_TWEAK_INJECTION          "Tweak Injection"
#define K_LOAD_DAEMONS             "Load Daemons"
#define K_DUMP_APTICKET            "Dump APTicket"
#define K_REFRESH_ICON_CACHE       "Refresh Icon Cache"
#define K_BOOT_NONCE               "Boot Nonce"
#define K_KERNEL_EXPLOIT           "Kernel Exploit"
#define K_DISABLE_AUTO_UPDATES     "Disable Auto Updates"
#define K_DISABLE_APP_REVOKES      "Disable App Revokes"
#define K_OVERWRITE_BOOT_NONCE     "Overwrite Boot Nonce"
#define K_EXPORT_KERNEL_TASK_PORT  "Export Kernel Task Port"
#define K_RESTORE_ROOTFS           "Restore RootFS"
#define K_INCREASE_MEMORY_LIMIT    "Increase Memory Limit"
#define K_ECID                     "ECID"
#define K_INSTALL_OPENSSH          "Install OpenSSH"
#define K_INSTALL_CYDIA            "Install Cydia"
#define K_RELOAD_SYSTEM_DAEMONS    "Reload System Daemons"
#define K_HIDE_LOG_WINDOW          "Hide Log Window"
#define K_RESET_CYDIA_CACHE        "Reset Cydia Cache"
#define K_SSH_ONLY                 "SSH-Only"
#define K_DARK_MODE                "Dark Mode"
#define K_ENABLE_GET_TASK_ALLOW    "Enable get-task-allow"
#define K_SET_CS_DEBUGGED          "Set CS_DEBUGGED"
#define K_AUTO_RESPRING            "Auto Respring"
#define K_CODE_SUBSTITUTOR         "Code Substitutor"
#define K_READ_ONLY_ROOTFS         "Read-Only RootFS"

typedef struct {
    bool load_tweaks;
    bool load_daemons;
    bool dump_apticket;
    bool run_uicache;
    const char *boot_nonce;
    bool disable_auto_updates;
    bool disable_app_revokes;
    bool overwrite_boot_nonce;
    bool export_kernel_task_port;
    bool restore_rootfs;
    bool increase_memory_limit;
    const char *ecid;
    bool install_cydia;
    bool install_openssh;
    bool reload_system_daemons;
    bool reset_cydia_cache;
    bool ssh_only;
    bool enable_get_task_allow;
    bool set_cs_debugged;
    bool hide_log_window;
    bool auto_respring;
    bool dark_mode;
    const char *kernel_exploit;
    const char *code_substitutor;
    bool read_only_rootfs;
} prefs_t;

prefs_t *new_prefs(void);
prefs_t *copy_prefs(void);
void release_prefs(prefs_t **prefs);
bool load_prefs(prefs_t *prefs);
bool set_prefs(prefs_t *prefs);
void register_default_prefs(void);
void repair_prefs(void);
void reset_prefs(void);

#endif /* prefs_h */
