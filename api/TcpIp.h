/*
 * Created on Sat Jan 28 2023 12:29:17 PM
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
#ifndef NAMMA_AUTOSAR_TCPIP_H
#define NAMMA_AUTOSAR_TCPIP_H

#include <stddef.h>

#include <Platform_Types.h>
#include <Std_Types.h>
#include <Eth_GeneralTypes.h>

#include <TcpIp_cfg.h>


#define TCPIP_AF_INET		0x02
#define TCPIP_AF_INET6		0x1c
typedef uint16 TcpIp_DomainType;


typedef enum {
	TCPIP_IPPROTO_TCP = 0x06,
	TCPIP_IPPROTO_UDP = 0x11
} TcpIp_ProtocolType;


#define IPv4_2_U32(a,b,c,d) (((u32)((a) & 0xff) << 24) | ((u32)((b) & 0xff) << 16) | \
                             ((u32)((c) & 0xff) << 8)  |  (u32)((d) & 0xff))
typedef struct {
	TcpIp_DomainType domain;
	uint16 port;
	uint32 addr[1];
} TcpIp_SockAddrInetType;


typedef struct {
	TcpIp_DomainType domain;
	uint16 port;
	uint32 addr[4];
} TcpIp_SockAddrInet6Type;


union TcpIp_SockAddrUnion {
	TcpIp_SockAddrInetType inet4;
	TcpIp_SockAddrInet6Type inet6;
};
typedef union TcpIp_SockAddrUnion TcpIp_SockAddrType;


typedef uint8 TcpIp_LocalAddrIdType;
typedef uint16 TcpIp_SocketIdType;


typedef enum {
	TCPIP_STATE_ONLINE,
	TCPIP_STATE_ONHOLD,
	TCPIP_STATE_OFFLINE,
	TCPIP_STATE_STARTUP,
	TCPIP_STATE_SHUTDOWN,
	TCPIP_STATE_MAX
} TcpIp_StateType;


typedef enum {
	TCPIP_IPADDR_STATE_ASSIGNED,
	TCPIP_IPADDR_STATE_ONHOLD,
	TCPIP_IPADDR_STATE_UNASSIGNED,
	TCPIP_IPADDR_STATE_MAX
} TcpIp_IpAddrStateType;


typedef enum {
	TCPIP_TCP_RESET,
	TCPIP_TCP_CLOSED,
	TCPIP_TCP_FIN_RECEIVED,
	TCPIP_UDP_CLOSED,
	TCPIP_TLS_HANDSHAKE_SUCCEEDED
} TcpIp_EventType;


typedef enum {
	TCPIP_IPADDR_ASSIGNMENT_STATIC,
	TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP,
	TCPIP_IPADDR_ASSIGNMENT_DHCP,
	TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL,
	TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER,
	TCPIP_IPADDR_ASSIGNMENT_ALL 
} TcpIp_IpAddrAssignmentType;


typedef enum {
	TCPIP_E_OK,
	TCPIP_E_NOT_OK,
	TCPIP_E_PHYS_ADDR_MISS,
	TCPIP_E_PENDING
} TcpIp_ReturnType;


typedef enum {
	TCPIP_PARAMID_TCP_RXWND_MAX,
	TCPIP_PARAMID_FRAMEPRIO,
	TCPIP_PARAMID_TCP_NAGLE,
	TCPIP_PARAMID_TCP_KEEPALIVE,
	TCPIP_PARAMID_TTL,
	TCPIP_PARAMID_TCP_KEEPALIVE_TIME,
	TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX,
	TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL,
	TCPIP_PARAMID_TCP_OPTIONFILTER,
	TCPIP_PARAMID_PATHMTU_ENABLE,
	TCPIP_PARAMID_FLOWLABEL,
	TCPIP_PARAMID_DSCP,
	TCPIP_PARAMID_UDP_CHECKSUM,
	TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT,
	TCPIP_PARAMID_TCP_RETRANSMIT_TIMEOUT,
	TCPIP_PARAMID_VENDOR_SPECIFIC
} TcpIp_ParamIdType;


typedef uint32 TcpIpIpAddrWildcardType;
typedef uint16 TcpIpPortWildcardType;
typedef TcpIp_LocalAddrIdType TcpIpLocalAddrIdWildcardType;


typedef struct {
	uint32 InetAddr[1]; 	/* IPv4 */
	uint8 PhysAddr[6]; 	/* MAC addr */
	uint8 State; /* TCPIP_ARP_ENTRY_STATIC, TCPIP_ARP_ENTRY_VALID, TCPIP_ARP_ENTRY_STALE */
} TcpIp_ArpCacheEntryType;

typedef struct {
	uint32 Inet6Addr[4]; 	/* IPv6 */
	uint8 PhysAddr[6]; 	/* MAC addr */
	uint8 State; /* TCPIP_NDP_ENTRY_STATIC, TCPIP_NDP_ENTRY_VALID, TCPIP_NDP_ENTRY_STALE */
} TcpIp_NdpCacheEntryType;


#define	TCPIP_MEAS_DROP_TCP 	0x01 
#define	TCPIP_MEAS_DROP_UDP 	0x02 
#define	TCPIP_MEAS_DROP_IPV4 	0x03 
#define	TCPIP_MEAS_DROP_IPV6 	0x04 
#define	TCPIP_MEAS_RESERVED_1	0x05/*-0x7F*/
#define	TCPIP_MEAS_RESERVED_2	0x80/*-0xEF*/
#define	TCPIP_MEAS_RESERVED_3	0xF0/*-0xFE*/
#define	TCPIP_MEAS_ALL 		0xFF 
typedef uint8 TcpIp_MeasurementIdxType;


typedef uint16 TcpIp_TlsConnectionIdType;



/////////////////////////////////////////////////
// Functions

void TcpIp_Init(const TcpIp_ConfigType* ConfigPtr);

Std_ReturnType TcpIp_Close(TcpIp_SocketIdType SocketId, boolean Abort);
Std_ReturnType TcpIp_Bind(TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr);
Std_ReturnType TcpIp_TcpListen(TcpIp_SocketIdType SocketId, uint16 MaxChannels);
Std_ReturnType TcpIp_TcpConnect(TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr);
Std_ReturnType TcpIp_TcpReceived(TcpIp_SocketIdType SocketId, uint32 Length);
Std_ReturnType TcpIp_RequestComMode(uint8 CtrlIdx, TcpIp_StateType State);

Std_ReturnType TcpIp_TcpTransmit(TcpIp_SocketIdType SocketId, const uint8* DataPtr,
                      uint32 AvailableLength, boolean ForceRetrieve);
Std_ReturnType TcpIp_UdpTransmit(TcpIp_SocketIdType SocketId, const uint8* DataPtr,
                const TcpIp_SockAddrType* RemoteAddrPtr, uint16 TotalLength);
void TcpIp_RxIndication(uint8 CtrlIdx, Eth_FrameType FrameType, boolean IsBroadcast,
                        const uint8* PhysAddrPtr, const uint8* DataPtr, uint16 LenByte);

void TcpIp_MainFunction(void);


#endif
