/*
 * Created on Thu Feb 22 2024 11:37:52 PM
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 Aananth C N
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

// TODO: Following enum is a duplicate of tcp_state defined in tcpbase.h
// of LWIP project. For time being this is copied, ideally the code must
// use the original state definition.
typedef enum TcpIp_ConnState {
	TCPIP_CLOSED      = 0,
	TCPIP_LISTEN      = 1,
	TCPIP_SYN_SENT    = 2,
	TCPIP_SYN_RCVD    = 3,
	TCPIP_ESTABLISHED = 4,
	TCPIP_FIN_WAIT_1  = 5,
	TCPIP_FIN_WAIT_2  = 6,
	TCPIP_CLOSE_WAIT  = 7,
	TCPIP_CLOSING     = 8,
	TCPIP_LAST_ACK    = 9,
	TCPIP_TIME_WAIT   = 10
}TcpIp_ConnStateType;

int TcpIp_recv(uint8_t* pdata);
int TcpIp_send(uint8_t* pdata, uint16_t len);
TcpIp_ConnStateType TcpIp_getConnState(uint16 skt_id);


#endif