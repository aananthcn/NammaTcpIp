/*
 * Created on Thu Feb 22 2024 11:37:52 PM
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 Aananth C N, Krishnaswamy D
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
#ifndef TCP_IP_CAR_OS_EXTENSIONS_H
#define TCP_IP_CAR_OS_EXTENSIONS_H

#include <stdint.h>

// configurable array size based on modelling tools.
#define MAX_TCPIP_SOCKET_HANDLES 512

// TODO: Following enum is a duplicate of tcp_state defined in tcpbase.h
// of LWIP project. For time being this is copied, ideally the code must
// use the original state definition.
typedef enum TcpIp_ConnState {
	TCPIP_CLOSED      = 0,
	TCPIP_BIND        = 1,
	TCPIP_LISTEN      = 2,
	TCPIP_ACCEPT	  = 3,
	TCPIP_SYN_SENT    = 4,
	TCPIP_SYN_RCVD    = 5,
	TCPIP_ESTABLISHED = 6,
	TCPIP_FIN_WAIT_1  = 7,
	TCPIP_FIN_WAIT_2  = 8,
	TCPIP_CLOSE_WAIT  = 9,
	TCPIP_CLOSING     = 10,
	TCPIP_LAST_ACK    = 11,
	TCPIP_TIME_WAIT   = 12
}TcpIp_ConnStateType;

int TcpIp_recv(uint8_t* pdata);
int TcpIp_send(uint8_t* pdata, uint16_t len);
TcpIp_ConnStateType TcpIp_getConnState(uint16 skt_id);

typedef struct TcpIp_Socket
{
	int SocketFd;
	TcpIp_SocketIdType SocketId;
	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;
	OS_WaitEventType WaitEvent;
} TcpIp_SocketType;

// configurable array element based on modelling tools.
TcpIp_SocketType[MAX_TCPIP_SOCKET_HANDLES] TcpIp_Socket_Conn;
int TcpIp_No_Socket_Conn = 0;

Std_ReturnType TcpIp_Socket_Init(int sockfd);
Std_ReturnType TcpIp_Socket_Bind(int sockfd, uint16_t port);
Std_ReturnType TcpIp_Socket_Listen(int sockfd);
Std_ReturnType TcpIp_Socket_Accept_Condition(void* data);
Std_ReturnType TcpIp_Socket_Accept(int sockfd);
int TcpIp_Socket_Recv(int sockfd, uint8_t* data, uint32_t length);
int TcpIp_Socket_Send(int sockfd, uint8_t* data, uint32_t length);
Std_ReturnType TcpIp_Socket_Main_Loop(void);

// TODO: We need support wait API from Zephyr similar like linux wait_queue_head
typedef struct OS_WaitEvent {
	uint8_t state;
} OS_WaitEventType;

typedef Std_ReturnType (*OS_WaitEvent_Condition)(void* socket);
Std_ReturnType OS_WaitEvent_Interruptable(OS_WaitEventType* waitevent, OS_WaitEvent_Condition condition);
Std_ReturnType OS_WaitEvent_Wakeup(OS_WaitEventType* waitevent);

#endif