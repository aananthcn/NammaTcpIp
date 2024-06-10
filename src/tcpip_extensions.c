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

#include <TcpIp.h>

#include "tcpip_extensions.h"

// initialize socket connection
Std_ReturnType TcpIp_Socket_Init(int sockfd) {
	Std_ReturnType retval = E_OK;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		TcpIp_Socket_Conn[sockfd].SocketFd   = sockfd;
		TcpIp_Socket_Conn[sockfd].SocketId   = sockfd;
		TcpIp_Socket_Conn[sockfd].TcpState   = TCPIP_CLOSED;
		TcpIp_No_Socket_Conn++;
	}

	return retval;
}

// bind socket connection
Std_ReturnType TcpIp_Socket_Bind(int sockfd, uint16_t port) {
	Std_ReturnType retval = E_NOK;

	TcpIp_SocketIdType SocketId = 0;
	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		SocketId = TcpIp_Socket_Conn[sockfd].SocketId;
		TcpState = TcpIp_Socket_Conn[sockfd].TcpState;

		// bind socket to ip address and port
		if (TcpState == TCPIP_CLOSED) {
			retval = TcpIp_Bind(SocketId, 0, &port);
			if (retval == E_OK) {
				TcpState = TCPIP_BIND;
			}
		}
	}

	return retval;
}

// listen to socket connection
Std_ReturnType TcpIp_Socket_Listen(int sockfd) {
	Std_ReturnType retval = E_NOK;

	TcpIp_SocketIdType SocketId = 0;
	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		SocketId = TcpIp_Socket_Conn[sockfd].SocketId;
		TcpState = TcpIp_Socket_Conn[sockfd].TcpState;

		// listen to incoming connections; accept will happen within TcpIp module
		if ((TcpState == TCPIP_BIND) || (TcpState == TCPIP_LISTEN)) {
			retval = TcpIp_TcpListen(SocketId, 0); // call once, this will return immediately, but an accept call back will be called.
			if (retval == E_OK)  {
				TcpIp_Socket_Conn[sockfd].TcpState = TCPIP_ACCEPT;
			}
			else {
				TcpIp_Socket_Conn[sockfd].TcpState = TCPIP_LISTEN;
			}
		}
	}

	return retval;
}

// callback function to poll the state of socket connection whether accept is completed or not
Std_ReturnType TcpIp_Socket_Accept_Condition(void* data) {
	Std_ReturnType retval = E_NOK;

	TcpIp_SocketType* socket = (TcpIp_SocketType*)data;
	if(NULL != socket) {
		while(TCPIP_ACCEPT != socket->TcpState) {
		}
		retval = E_OK;
	}

	return retval;
}

// accept new incoming connection
Std_ReturnType TcpIp_Socket_Accept(int sockfd) {
	Std_ReturnType retval = E_NOK;

	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		TcpState = TcpIp_Socket_Conn[sockfd].TcpState;
		if(TCPIP_LISTEN == TcpState) {

			// wait until new connection is established by the client.
			OS_WaitEventType waitevent = TcpIp_Socket_Conn[sockfd].WaitEvent;
			OS_WaitEvent_Interruptable(waitevent, TcpIp_Socket_Accept_Condition);

			// check if initialization is complete
			if (TCPIP_ACCEPT == TcpIp_Socket_Conn[sockfd].TcpState) {
				retval = E_OK; // initialization is complete
			}
		} else if (TCPIP_ACCEPT == TcpState) {
			retval = E_OK; // initialization is complete
		} else {
			retval = E_NOK;
		}
	}

	return retval;
}

// receiver data through socket connection
int TcpIp_Socket_Recv(int sockfd, uint8_t* data, uint32_t length) {
	int RecvSize = 0;

	TcpIp_SocketIdType SocketId = 0;
	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		SocketId = TcpIp_Socket_Conn[sockfd].SocketId;
		TcpState = TcpIp_Socket_Conn[sockfd].TcpState;

		if (TcpState >= TCPIP_ACCEPT) {
			// TODO: need to change the design. 
			// 1) TcpIp_recv uses global buffer. So we should not entertain parallel receive call. some lock is needed here.
			// 2) TcpIp_recv shall length from upper layer to avoid accessing the buffer location which is out of range.
			RecvSize = TcpIp_recv(data);
		}
	}
	
	return RecvSize;
}

// send data through socket connection
int TcpIp_Socket_Send(int sockfd, uint8_t* data, uint32_t length) {
	int SendSize = 0;

	TcpIp_SocketIdType SocketId = 0;
	TcpIp_ConnStateType TcpState = TCPIP_CLOSED;

	if(0 > sockfd) {
		retval = E_NOK;
	} else if (MAX_TCPIP_SOCKET_HANDLES < sockfd) {
		retval = E_NOK;
	} else {
		SocketId = TcpIp_Socket_Conn[sockfd].SocketId;
		TcpState = TcpIp_Socket_Conn[sockfd].TcpState;		

		if (TcpState >= TCPIP_ACCEPT) {
			// TODO: need to change the design. TcpIp_recv uses global buffer. So we should 
			// not entertain parallel receive call. some lock is needed here.
			if(length > 0) {
				SendSize = TcpIp_send(data, length);
			}
		}
	}

	return SendSize;
}

// Trigger cyclically in the context of OS Task (Zephyr)
Std_ReturnType TcpIp_Socket_Main_Loop(void) { 
	Std_ReturnType retval = E_OK;

	int con = 0;
	TcpIp_SocketIdType SocketId = 0;

	for(con = 0; i < TcpIp_No_Socket_Conn; con++) {
		SocketId = TcpIp_Socket_Conn[con].SocketId;
		// TODO: Poll the state of connection with TcpIP. We need to find which API it is. currently TcpIp_TcpState is used
		TcpIp_Socket_Conn[con].TcpState = TcpIp_TcpState(TcpIp_Socket_Conn[con].SocketId);
		if(TCPIP_ACCEPT == TcpIp_Socket_Conn[con].TcpState) {
			// if state is accepted, wakeup the thread which is waiting through OS_WaitEvent_Interruptable.
			OS_WaitEvent_Wakeup(&TcpIp_Socket_Conn[con].WaitEvent);
		}
	}

	return retval;
}

// TODO: We need support wait API from Zephyr similar like linux wait_queue_head
Std_ReturnType OS_WaitEvent_Interruptable(OS_WaitEventType waitevent, OS_WaitEvent_Condition condition)
{
	Std_ReturnType retval = E_OK;

	(void)waitevent;
	(void)condition;

	return retval;
}

Std_ReturnType OS_WaitEvent_Wakeup(OS_WaitEventType* waitevent)
{
	Std_ReturnType retval = E_OK;

	(void)waitevent;

	return retval;
}