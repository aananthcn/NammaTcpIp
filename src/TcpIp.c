/*
 * Created on Sat Jan 28 2023 2:45:48 PM
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
#include <TcpIp.h>
#include <Eth.h>

#include <lwip/tcp.h>
#include "lwip_int.h"


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(TcpIp, LOG_LEVEL_DBG);



///////////////////////////////////////////////////////////////////////////////
// local static datastructures, imported from tcpecho_raw.c of lwip
enum tcpip_raw_states
{
        ES_NONE = 0,
        ES_ACCEPTED,
        ES_RECEIVED,
        ES_CLOSING
};

struct tcpip_raw_state
{
        u8_t state;
        u8_t retries;
        struct tcp_pcb *pcb;
        struct pbuf *p; /* pbuf (chain) to recycle */
};


static struct tcp_pcb *tcpip_raw_pcb = NULL;
static struct tcpip_raw_state *client_es;
static struct tcpip_raw_state *server_es;



///////////////////////////////////////////////////////////////////////////////
// local static functions, imported from tcpecho_raw.c of lwip
static void tcpip_raw_free(struct tcpip_raw_state *es)
{
        if (es != NULL) {
                if (es->p) {
                        /* free the buffer chain if present */
                        pbuf_free(es->p);
                }

                mem_free(es);
        }
}



static void tcpip_raw_error(void *arg, err_t err) {
        struct tcpip_raw_state *es;

        LWIP_UNUSED_ARG(err);
        es = (struct tcpip_raw_state *)arg;
        tcpip_raw_free(es);
        LOG_DBG("tcp raw error");
}



static void tcpip_raw_send(struct tcp_pcb *tpcb, struct tcpip_raw_state *es)
{
        struct pbuf *ptr;
        err_t wr_err = ERR_OK;

        while ((wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb)))
        {
                ptr = es->p;

                /* enqueue data for transmission */
                wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
                if (wr_err == ERR_OK) {
                        u16_t plen;

                        plen = ptr->len;
                        /* continue with next pbuf in chain (if any) */
                        es->p = ptr->next;
                        if (es->p != NULL) {
                                /* new reference! */
                                pbuf_ref(es->p);
                        }
                        /* chop first pbuf from chain */
                        pbuf_free(ptr);
                        /* we can read more data now */
                        tcp_recved(tpcb, plen);
                }
                else if (wr_err == ERR_MEM) {
                        /* we are low on memory, try later / harder, defer to poll */
                        es->p = ptr;
                        LOG_DBG("tcp memory error");
                }
                else {
                        /* other problem ?? */
                }
        }
}



static void tcpip_raw_close(struct tcp_pcb *tpcb, struct tcpip_raw_state *es)
{
        tcp_arg(tpcb, NULL);
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_err(tpcb, NULL);
        tcp_poll(tpcb, NULL, 0);

        tcpip_raw_free(es);

        tcp_close(tpcb);
}



static err_t tcpip_raw_poll(void *arg, struct tcp_pcb *tpcb) {
        err_t ret_err;
        struct tcpip_raw_state *es;

        es = (struct tcpip_raw_state *) arg;
        if (es != NULL) {
                if (es->p != NULL) {
                        /* there is a remaining pbuf (chain) */
                        tcpip_raw_send(tpcb, es);
                }
                else {
                        /* no remaining pbuf (chain)  */
                        if (es->state == ES_CLOSING) {
                                tcpip_raw_close(tpcb, es);
                        }
                }
                ret_err = ERR_OK;
        }
        else {
                /* nothing to be done */
                tcp_abort(tpcb);
                ret_err = ERR_ABRT;
        }

        return ret_err;
}



static err_t tcpip_raw_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
        struct tcpip_raw_state *es;

        LWIP_UNUSED_ARG(len);

        es = (struct tcpip_raw_state *)arg;
        es->retries = 0;

        if (es->p != NULL) {
                /* still got pbufs to send */
                tcp_sent(tpcb, tcpip_raw_sent);
                tcpip_raw_send(tpcb, es);
        }
        else {
                /* no more pbufs to send */
                if (es->state == ES_CLOSING) {
                        LOG_DBG("tcp socket close!");
                        tcpip_raw_close(tpcb, es);
                }
        }

        return ERR_OK;
}



static err_t tcpip_raw_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
        struct tcpip_raw_state *es;
        err_t ret_err;

        LWIP_ASSERT("arg != NULL", arg != NULL);
        es = (struct tcpip_raw_state *) arg;
        if (p == NULL) {
                /* remote host closed connection */
                es->state = ES_CLOSING;
                if (es->p == NULL) {
                        /* we're done sending, close it */
                        tcpip_raw_close(tpcb, es);
                }
                else {
                        /* we're not done yet */
                        tcpip_raw_send(tpcb, es);
                }
                ret_err = ERR_OK;
        }
        else if (err != ERR_OK) {
                /* cleanup, for unknown reason */
                LWIP_ASSERT("no pbuf expected here", p == NULL);
                LOG_DBG("TCP recv error: %d", err);
                ret_err = err;
        }
        else if (es->state == ES_ACCEPTED) {
                /* first data chunk in p->payload */
                es->state = ES_RECEIVED;
                /* store reference to incoming pbuf (chain) */
                es->p = p;
                tcpip_raw_send(tpcb, es);
                ret_err = ERR_OK;
        }
        else if (es->state == ES_RECEIVED) {
                /* read some more data */
                if (es->p == NULL) {
                        es->p = p;
                        tcpip_raw_send(tpcb, es);
                }
                else
                {
                        struct pbuf *ptr;

                        /* chain pbufs to the end of what we recv'ed previously  */
                        ptr = es->p;
                        pbuf_cat(ptr, p);
                }
                ret_err = ERR_OK;
        }
        else
        {
                /* unknown es->state, trash data  */
                tcp_recved(tpcb, p->tot_len);
                pbuf_free(p);
                ret_err = ERR_OK;
        }
        return ret_err;
}



static err_t tcpip_raw_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
        err_t ret_err;

        LWIP_UNUSED_ARG(arg);
        if ((err != ERR_OK) || (newpcb == NULL)) {
                LOG_DBG("Argument validation failure!");
                return ERR_VAL;
        }

        /* Unless this pcb should have NORMAL priority, set its priority now.
           When running out of pcbs, low priority pcbs can be aborted to create
           new pcbs of higher priority. */
        tcp_setprio(newpcb, TCP_PRIO_MIN);

        server_es = (struct tcpip_raw_state *) mem_malloc(sizeof(struct tcpip_raw_state));
        if (server_es != NULL) {
                server_es->state = ES_ACCEPTED;
                server_es->pcb = newpcb;
                server_es->retries = 0;
                server_es->p = NULL;
                /* pass newly allocated es to our callbacks */
                tcp_arg(newpcb, server_es);
                tcp_recv(newpcb, tcpip_raw_recv);
                tcp_err(newpcb, tcpip_raw_error);
                tcp_poll(newpcb, tcpip_raw_poll, 0);
                tcp_sent(newpcb, tcpip_raw_sent);
                ret_err = ERR_OK;
        }
        else {
                LOG_DBG("Memory allocation failure!");
                ret_err = ERR_MEM;
        }

        return ret_err;
}



static err_t tcpip_raw_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
        LWIP_UNUSED_ARG(arg);
        LWIP_UNUSED_ARG(tpcb);

        LOG_DBG("TcpIp connect status: %d", err);

        return ERR_OK;
}


///////////////////////////////////////////////////////////////////////////////
// Core Communication Control APIs

// By this API service the TCP/IP stack is requested to close the socket and
// release all related resources.
Std_ReturnType TcpIp_Close(TcpIp_SocketIdType SocketId, boolean Abort) {
        Std_ReturnType retval = E_OK;

        if (tcpip_raw_pcb == NULL) {
                retval = E_NOT_OK;
        }
        else {
                tcpip_raw_close(tcpip_raw_pcb, server_es); // TODO: implement TcpIp close for client socket later
        }

        return retval;
}



// By this API service the TCP/IP stack is requested to bind a UDP or TCP socket
// to a local resource. The Server API.
Std_ReturnType TcpIp_Bind(TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr) {
        Std_ReturnType retval = E_OK;
        err_t err;

        if (PortPtr == NULL && tcpip_raw_pcb != NULL) {
                LOG_DBG("Argument validation failure!");
                return E_NOT_OK;
        }

        tcpip_raw_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
        if (tcpip_raw_pcb != NULL) {
                err = tcp_bind(tcpip_raw_pcb, IP_ANY_TYPE, *PortPtr);
                if (err != ERR_OK) {
                        retval = E_NOT_OK;
                }
        }

        return retval;
}



#define TCP_LISTEN_STATE        1
// By this API service the TCP/IP stack is requested to listen on the TCP socket
// specified by the socket identifier. The Server API.
Std_ReturnType TcpIp_TcpListen(TcpIp_SocketIdType SocketId, uint16 MaxChannels) {
        Std_ReturnType retval = E_OK;

        if (tcpip_raw_pcb == NULL) {
                LOG_DBG("Argument validation failure!");
                return E_NOT_OK; // TcpIp_Bind is not called!!
        }


        if (tcpip_raw_pcb->state < TCP_LISTEN_STATE) {
                tcpip_raw_pcb = tcp_listen(tcpip_raw_pcb);
                tcp_accept(tcpip_raw_pcb, tcpip_raw_accept);
        }

        // check if accept call back is received by TcpIp module
        if (server_es == NULL) {
                retval = E_NOT_OK;
        }
        else if ((server_es->state == ES_NONE) || (server_es->state == ES_CLOSING)) {
                retval = E_NOT_OK;
        }

        return retval;
}



// By this API service the TCP/IP stack is requested to establish a TCP connection
// to the configured peer. The client API. Reference: test_tcp.c of lwip project.
Std_ReturnType TcpIp_TcpConnect(TcpIp_SocketIdType SocketId, const TcpIp_SockAddrType* RemoteAddrPtr) {
        Std_ReturnType retval = E_OK;
        ip_addr_t ipv4;
        err_t err;
        u16_t port;

        struct tcp_pcb *newpcb = tcp_new();
        if (newpcb == NULL) {
                LOG_DBG("tcp_pcp allocation failure!");
                return E_NOT_OK;
        }

        client_es = (struct tcpip_raw_state *) mem_malloc(sizeof(struct tcpip_raw_state));
        if (client_es != NULL) {
                client_es->state = ES_ACCEPTED;
                client_es->pcb = newpcb;
                client_es->retries = 0;
                client_es->p = NULL;
                /* pass newly allocated es to our callbacks */
                tcp_arg(newpcb, client_es);
                tcp_recv(newpcb, tcpip_raw_recv);
                tcp_err(newpcb, tcpip_raw_error);
                tcp_poll(newpcb, tcpip_raw_poll, 0);
                tcp_sent(newpcb, tcpip_raw_sent);
        }
        else {
                LOG_DBG("Memory allocation failure!");
                // tcp_free(newpcb);
                memp_free(MEMP_TCP_PCB, newpcb);
                return E_NOT_OK;
        }

        ipv4.addr = RemoteAddrPtr->inet4.addr[0];
        port = RemoteAddrPtr->inet4.port;
        err = tcp_connect(newpcb, &ipv4, port, tcpip_raw_connected);
        if (err != ERR_OK) {
                retval = E_NOT_OK;
        }

        return retval;
}



// By this API service the reception of socket data is confirmed to the TCP/IP stack.
Std_ReturnType TcpIp_TcpReceived(TcpIp_SocketIdType SocketId, uint32 Length) {
        Std_ReturnType retval = E_OK;

        // Increase receive window -- The receive window represents the number of bytes
        // that are available in the receive buffer. If the receive buffer is full, the
        // receiving system advertises a receive window size of zero, and the sending
        // system must wait to send more data.

        return retval;
}



// By this API service the TCP/IP stack is requested to change the TcpIp state
// of the communication network identified by EthIf controller index.
Std_ReturnType TcpIp_RequestComMode(uint8 CtrlIdx, TcpIp_StateType State) {
        Std_ReturnType retval = E_OK;

        // Call EthIf to change the state of controller

        return retval;
}


///////////////////////////////////////////////////////////////////////////////
// Transmission functions

// This service requests transmission of data via TCP to a remote node. The 
// transmission of the data is decoupled.
// Note: The TCP segment(s) are sent dependent on runtime factors (e.g. receive
// window) and configuration parameter (e.g. Nagle algorithm) .
Std_ReturnType TcpIp_TcpTransmit(TcpIp_SocketIdType SocketId, const uint8* DataPtr,
                      uint32 AvailableLength, boolean ForceRetrieve) {
        Std_ReturnType retval = E_OK;


        return retval;
}



// This service transmits data via UDP to a remote node. The transmission of the data
// is immediately performed with this function call by forwarding it to EthIf.
Std_ReturnType TcpIp_UdpTransmit(TcpIp_SocketIdType SocketId, const uint8* DataPtr,
                const TcpIp_SockAddrType* RemoteAddrPtr, uint16 TotalLength) {
        Std_ReturnType retval = E_OK;


        return retval;
}



///////////////////////////////////////////////////////////////////////////////
// Reception Function
// By this API service the TCP/IP stack gets an indication and the data of a received frame.
void TcpIp_RxIndication(uint8 CtrlIdx, Eth_FrameType FrameType, boolean IsBroadcast, 
                        const uint8* PhysAddrPtr, const uint8* DataPtr, uint16 LenByte) {

}


///////////////////////////////////////////////////////////////////////////////



// Function: TcpIp_Init will be called by EcuM
void TcpIp_Init(const TcpIp_ConfigType* ConfigPtr) {
        /* light weight IP stack init */
        lwip_int_init();

        LOG_INF("Init complete!");
}



// Function: TcpIp_MainFunction will be called by Os
void TcpIp_MainFunction(void) {
        /* lwip periodic functions */
        lwip_int_main();
}