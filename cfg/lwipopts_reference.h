/*
 * Created on Fri Jan 27 2023 8:43:15 PM
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Aananth C N
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _NASR_LWIPOPTS_H__
#define _NASR_LWIPOPTS_H__

#include <arch/sys_arch.h>

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          1
#define MEM_ALIGNMENT                   4
#define SYS_LIGHTWEIGHT_PROT            1
#define LWIP_SINGLE_NETIF               0

#define MEMP_MEM_MALLOC                 0
#define MEM_USE_POOLS                   1
#define MEMP_USE_CUSTOM_POOLS           1
#define MEM_LIBC_MALLOC                 0



/* Minimal changes to opt.h required for tcp unit tests: */
#define MEM_SIZE                        (2*1024)
#define PBUF_POOL_SIZE                  40 /* pbuf tests need 400 == ~200KByte */
#define LWIP_SNMP                       1
#define LWIP_MDNS_RESPONDER             1
#define LWIP_RAND()                     ((u32_t)sys_rand())

#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_RAW_PCB                8
#define MEMP_NUM_UDP_PCB                6
#define MEMP_NUM_TCP_PCB                7
#define MEMP_NUM_TCP_PCB_LISTEN         8
//#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_ARP_QUEUE              30

// ARP
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_MAXAGE                      300
#define ARP_QUEUEING                    0
#define ARP_QUEUE_LEN                   3
#define ETHARP_SUPPORT_VLAN             0
#define LWIP_ETHERNET                   LWIP_ARP
#define ETH_PAD_SIZE                    0
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#define ETHARP_TABLE_MATCH_NETIF        !LWIP_SINGLE_NETIF

// DNS
#define LWIP_DNS                        1
#define DNS_TABLE_SIZE                  4
#define DNS_MAX_NAME_LENGTH             256
#define DNS_MAX_SERVERS                 2
#define DNS_MAX_RETRIES                 4
#define DNS_DOES_NAME_CHECK             1
#define LWIP_DNS_SECURE (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)
#define DNS_LOCAL_HOSTLIST              0
#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   0

// IPv4
#define LWIP_IPV4                       1
#define IP_FORWARD                      0
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define IP_OPTIONS_ALLOWED              1
#define IP_REASS_MAXAGE                 15
#define IP_REASS_MAX_PBUFS              10
#define IP_DEFAULT_TTL                  255
#define IP_SOF_BROADCAST                0
#define IP_SOF_BROADCAST_RECV           0
#define IP_FORWARD_ALLOW_TX_ON_RX_NETIF 0
#define LWIP_ICMP                       1
#define ICMP_TTL                        (IP_DEFAULT_TTL)
#define LWIP_BROADCAST_PING             0
#define LWIP_MULTICAST_PING             0
#define LWIP_RAW                        1
#define RAW_TTL                         (IP_DEFAULT_TTL)

// IPv6
#define LWIP_IPV6                       1
#define IPV6_REASS_MAXAGE               60
#define LWIP_IPV6_SCOPES                (LWIP_IPV6 && !LWIP_SINGLE_NETIF)
#define LWIP_IPV6_SCOPES_DEBUG          0
#define LWIP_IPV6_NUM_ADDRESSES         3
#define LWIP_IPV6_FORWARD               0
#define LWIP_IPV6_FRAG                  1
#define LWIP_IPV6_REASS                 (LWIP_IPV6)
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   1
#define LWIP_IPV6_AUTOCONFIG            (LWIP_IPV6)
#define LWIP_IPV6_ADDRESS_LIFETIMES     (LWIP_IPV6_AUTOCONFIG)
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0
#define LWIP_ICMP6                      (LWIP_IPV6)
#define LWIP_ICMP6_DATASIZE             8
#define LWIP_ICMP6_HL                   255
#define LWIP_IPV6_MLD                   (LWIP_IPV6)
#define MEMP_NUM_MLD6_GROUP             4
#define LWIP_ND6_QUEUEING               (LWIP_IPV6)
#define MEMP_NUM_ND6_QUEUE              20
#define LWIP_ND6_NUM_NEIGHBORS          10
#define LWIP_ND6_NUM_DESTINATIONS       10
#define LWIP_ND6_NUM_PREFIXES           5
#define LWIP_ND6_NUM_ROUTERS            3
#define LWIP_ND6_MAX_MULTICAST_SOLICIT  3
#define LWIP_ND6_MAX_UNICAST_SOLICIT    3
#define LWIP_ND6_MAX_ANYCAST_DELAY_TIME 1000
#define LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT  3
#define LWIP_ND6_REACHABLE_TIME         30000
#define LWIP_ND6_RETRANS_TIMER          1000
#define LWIP_ND6_DELAY_FIRST_PROBE_TIME 5000
#define LWIP_ND6_ALLOW_RA_UPDATES       1
#define LWIP_ND6_TCP_REACHABILITY_HINTS 1
#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS  0
#define LWIP_IPV6_DHCP6                 0
#define LWIP_IPV6_DHCP6_STATEFUL        0
#define LWIP_IPV6_DHCP6_STATELESS       LWIP_IPV6_DHCP6
#define LWIP_DHCP6_GET_NTP_SRV          0
#define LWIP_DHCP6_MAX_NTP_SERVERS      1
#define LWIP_DHCP6_MAX_DNS_SERVERS      DNS_MAX_SERVERS
#define IPV6_FRAG_COPYHEADER            1

// IGMP
#define LWIP_IGMP                       1

// DHCP
#define LWIP_DHCP                       1
#define LWIP_DHCP_CHECK_LINK_UP         0
#define LWIP_DHCP_BOOTP_FILE            0
#define LWIP_DHCP_GET_NTP_SRV           0
#define LWIP_DHCP_MAX_NTP_SERVERS       1
#define LWIP_DHCP_MAX_DNS_SERVERS       DNS_MAX_SERVERS
#define LWIP_AUTOIP                     1
#define LWIP_DHCP_AUTOIP_COOP           0
#define LWIP_DHCP_AUTOIP_COOP_TRIES     9
#define LWIP_MIB2_CALLBACKS             0

// UDP
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    0
#define UDP_TTL                         (IP_DEFAULT_TTL)
#define LWIP_NETBUF_RECVINFO            0
#define LWIP_MULTICAST_TX_OPTIONS       ((LWIP_IGMP || LWIP_IPV6_MLD) && (LWIP_UDP || LWIP_RAW))

// TCP
#define LWIP_TCP                        1
#define TCP_TTL                         (IP_DEFAULT_TTL)
#define TCP_MSS                         536
#define TCP_WND                         (10 * TCP_MSS)
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   6
#define TCP_QUEUE_OOSEQ                 (LWIP_TCP)
#define LWIP_TCP_SACK_OUT               0
#define LWIP_TCP_MAX_SACK_NUM           4
#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (2 * TCP_MSS)
#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
#define TCP_SNDLOWAT                    LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1)
#define TCP_SNDQUEUELOWAT               LWIP_MAX(((TCP_SND_QUEUELEN)/2), 5)
#define TCP_OOSEQ_MAX_BYTES             0
#define TCP_OOSEQ_BYTES_LIMIT(pcb)      TCP_OOSEQ_MAX_BYTES
#define TCP_OOSEQ_MAX_PBUFS             0
#define TCP_OOSEQ_PBUFS_LIMIT(pcb)      TCP_OOSEQ_MAX_PBUFS
#define TCP_LISTEN_BACKLOG              0
#define TCP_DEFAULT_LISTEN_BACKLOG      0xff
#define TCP_OVERSIZE                    TCP_MSS
#define LWIP_TCP_TIMESTAMPS             0
#define TCP_WND_UPDATE_THRESHOLD        LWIP_MIN((TCP_WND / 4), (TCP_MSS * 4))
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   2
#define LWIP_TCP_PCB_NUM_EXT_ARGS       0
#define LWIP_ALTCP                      1
#define LWIP_ALTCP_TLS                  0
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN


// PBUF
#define PBUF_LINK_HLEN                  (14 + ETH_PAD_SIZE)
#define PBUF_LINK_ENCAPSULATION_HLEN    0
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)
#define LWIP_PBUF_REF_T                 u8_t

// NETIF
#define LWIP_NETIF_HOSTNAME             0
#define LWIP_NETIF_API                  0
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  0
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_REMOVE_CALLBACK      0
#define LWIP_NETIF_HWADDRHINT           0
#define LWIP_NETIF_TX_SINGLE_PBUF       0
#define LWIP_NUM_NETIF_CLIENT_DATA      1
#define LWIP_NETIF_LOOPBACK             0
#define LWIP_HAVE_LOOPIF                (LWIP_NETIF_LOOPBACK && !LWIP_SINGLE_NETIF)
#define LWIP_LOOPIF_MULTICAST           0
#define LWIP_LOOPBACK_MAX_PBUFS         0
#define LWIP_NETIF_LOOPBACK_MULTITHREADING    (!NO_SYS)

// NETCONN
#define LWIP_NETCONN                    0
#define LWIP_TCPIP_TIMEOUT              0
#define LWIP_NETCONN_SEM_PER_THREAD     0
#define LWIP_NETCONN_FULLDUPLEX         0

// SOCKET
#define LWIP_SOCKET                     0
#define LWIP_COMPAT_SOCKETS             1 /* 0..2 */
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#define LWIP_SOCKET_OFFSET              0
#define LWIP_TCP_KEEPALIVE              0
#define LWIP_SO_SNDTIMEO                0
#define LWIP_SO_RCVTIMEO                0
#define LWIP_SO_SNDRCVTIMEO_NONSTANDARD 0
#define LWIP_SO_RCVBUF                  0
#define LWIP_SO_LINGER                  0
//#define RECV_BUFSIZE_DEFAULT            INT_MAX
#define RECV_BUFSIZE_DEFAULT            8192
#define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 20000
#define SO_REUSE                        0
#define SO_REUSE_RXTOALL                0
#define LWIP_FIONREAD_LINUXMODE         0
#define LWIP_SOCKET_SELECT              1
#define LWIP_SOCKET_POLL                1

// STATS
#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              0
#define LINK_STATS                      1
#define ETHARP_STATS                    (LWIP_ARP)
#define IP_STATS                        1
#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
#define ICMP_STATS                      1
#define IGMP_STATS                      (LWIP_IGMP)
#define UDP_STATS                       (LWIP_UDP)
#define TCP_STATS                       (LWIP_TCP)
#define MEM_STATS                       ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
#define MEMP_STATS                      (MEMP_MEM_MALLOC == 0)
#define SYS_STATS                       (NO_SYS == 0)
#define IP6_STATS                       (LWIP_IPV6)
#define ICMP6_STATS                     (LWIP_IPV6 && LWIP_ICMP6)
#define IP6_FRAG_STATS                  (LWIP_IPV6 && (LWIP_IPV6_FRAG || LWIP_IPV6_REASS))
#define MLD6_STATS                      (LWIP_IPV6 && LWIP_IPV6_MLD)
#define ND6_STATS                       (LWIP_IPV6)
#define MIB2_STATS                      1

// CHECKSUM
#define LWIP_CHECKSUM_CTRL_PER_NETIF    0
#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_TCP                1
#define CHECKSUM_GEN_ICMP               1
#define CHECKSUM_GEN_ICMP6              1
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_ICMP             0
#define CHECKSUM_CHECK_ICMP6            0
#define LWIP_CHECKSUM_ON_COPY           0




#define LWIP_DEBUG 1
#if LWIP_DEBUG == 1
#define TCP_DEBUG                       LWIP_DBG_ON
#define ETHARP_DEBUG                    LWIP_DBG_ON
#define PBUF_DEBUG                      LWIP_DBG_ON
#define IP_DEBUG                        LWIP_DBG_ON
#define TCPIP_DEBUG                     LWIP_DBG_ON
#define DHCP_DEBUG                      LWIP_DBG_ON
#define UDP_DEBUG                       LWIP_DBG_ON
#endif



#endif /* _NASR_LWIPOPTS_H__ */