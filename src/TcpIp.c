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

#include "tcpip_mpool.h"


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(TcpIp, LOG_LEVEL_DBG);



///////////////////////////////////////////////////////////////////////////////
// local static datastructures, imported from tcpecho_raw.c of lwip
struct rx_ctrl_blk {
        uint16_t rx_buf_cnt;
        tcpip_mpool_t *rx_bufhead;
};


static struct tcp_pcb *tcpip_raw_pcb = NULL;



///////////////////////////////////////////////////////////////////////////////
// Rx control block and functions
static struct rx_ctrl_blk TcpIp_Rx_CtrlBlk = {
        .rx_buf_cnt = 0,
        .rx_bufhead = NULL
};


// This function will be called by tcpip_raw_recv(), upon receiving a new frame
static void push_to_rx_ctrl_blk(struct pbuf *pbuf) {
        tcpip_mpool_t *p_rxbuf;
        tcpip_mpool_t *p_last_node = TcpIp_Rx_CtrlBlk.rx_bufhead;

        // allocate memory to store the overhead (not data) for the incoming bytes
        p_rxbuf = get_new_tcpip_mpool();
        if (p_rxbuf == NULL) {
                uint16_t plen = pbuf->len;

                /* drop the packet */
                LOG_ERR("Unable to get memory pool for Eth Rx message. Dropping it!");
                pbuf_free(pbuf);

                /* we can read more data now */
                tcp_recved(tcpip_raw_pcb, plen);
                return;
        }

        // increment count which acts similar to spin lock
        TcpIp_Rx_CtrlBlk.rx_buf_cnt++;
        if (TcpIp_Rx_CtrlBlk.rx_buf_cnt >= TCPIP_MEM_POOL_SIZE-1) {
                LOG_WRN("TcpIp Rx message queue has reach its limit (%d)", TCPIP_MEM_POOL_SIZE);
        }

        // store data
        p_rxbuf->pbuf = pbuf;

        // if the list is empty, make the new node to the head
        if (p_last_node == NULL) {
                TcpIp_Rx_CtrlBlk.rx_bufhead = p_rxbuf;
                return;
        }
        else {
                // push the current rxbuf node to the tail
                do {
                        p_last_node = p_last_node->next;
                } while (p_last_node != NULL);
                p_last_node = p_rxbuf;
        }
}



// This function is expected to be called by TcpIp_recv() in a loop
static struct pbuf* pop_pbuf_from_rx_ctrl_blk(void) {
        tcpip_mpool_t *p_head;
        struct pbuf *pbuf = NULL;

        if (TcpIp_Rx_CtrlBlk.rx_buf_cnt) {
                p_head = TcpIp_Rx_CtrlBlk.rx_bufhead; // head is at rx ctrl block

                // if any message exists in the rx buffer
                if (p_head != NULL) {
                        // pop pbuf from head & remove the popped one
                        pbuf = p_head->pbuf;
                        TcpIp_Rx_CtrlBlk.rx_bufhead = p_head->next;

                        // free the head's memory, alloc'ed in push_to_rx_ctrl_blk
                        free_tcpip_mpool(p_head);
                        p_head = NULL;
                }


                // decrement Counter, because we popped the data out
                TcpIp_Rx_CtrlBlk.rx_buf_cnt--; // call tcp_recved(tpcb, plen); only after pbuf is consumed
        }

        return pbuf;
}



///////////////////////////////////////////////////////////////////////////////
// non-AUTOSAR APIs for testing
int TcpIp_send(uint8_t* pdata, uint16_t len) {
        int retval;
        err_t wr_err = ERR_OK;

        if (pdata == NULL) {
                LOG_ERR("Input validation failure: pdata = %p, len = %d", pdata, len);
                return -1;
        }

        wr_err = tcp_write(tcpip_raw_pcb, pdata, len, 1);
        if (wr_err == ERR_OK) {
                retval = 0;
        }
        else if (wr_err == ERR_MEM) {
                /* we are low on memory, try later / harder, defer to poll */
                LOG_ERR("tcp memory error (code: %d, tcp_state = 0x%X)", wr_err, tcpip_raw_pcb->state);
                retval = -2;
        }
        else {
                /* other problem ?? */
                LOG_ERR("other error (code: %d), tcp_state = 0x%X)", wr_err, tcpip_raw_pcb->state);
                retval = -3;
        }

        return retval;
}


int TcpIp_recv(uint8_t* pdata) {
        struct pbuf* pbuf;
        int len, i;

        if (pdata == NULL) {
                LOG_DBG("Input pdata is null (%p). Data cannot be copied!", pdata);
                return 0;
        }

        pbuf = pop_pbuf_from_rx_ctrl_blk();
        if (pbuf) {
                len = pbuf->len;
                for (i = 0; i < len; i++) {
                        pdata[i] = ((uint8_t*)pbuf->payload)[i];
                }
        }
        else {
                len = 0;
        }

        return len;
}


///////////////////////////////////////////////////////////////////////////////
// local static functions, imported from tcpecho_raw.c of lwip
static void tcpip_raw_error(void *arg, err_t err) {
        LWIP_UNUSED_ARG(arg);
        LOG_ERR("tcp raw error (code: 0x%X)", err);
}



static void tcpip_raw_close(struct tcp_pcb *tpcb)
{
        tcp_arg(tpcb, NULL);
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_err(tpcb, NULL);
        tcp_poll(tpcb, NULL, 0);

        tcp_close(tpcb);
}



static err_t tcpip_raw_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
        if (err == ERR_OK && p != NULL) {
                tcp_recved(tpcb, p->tot_len);
                push_to_rx_ctrl_blk(p);
                pbuf_free(p);
        }
        else {
                pbuf_free(p);
        }

        if ((err != ERR_OK) || (err == ERR_OK && p == NULL)) {
                tcpip_raw_close(tpcb);
        }

        return ERR_OK;
}



static err_t tcpip_raw_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
        err_t ret_err;

        tcpip_raw_pcb = newpcb;

        LWIP_UNUSED_ARG(arg);
        if ((err != ERR_OK) || (newpcb == NULL)) {
                LOG_DBG("Argument validation failure!");
                return ERR_VAL;
        }

        /* Unless this pcb should have NORMAL priority, set its priority now.
           When running out of pcbs, low priority pcbs can be aborted to create
           new pcbs of higher priority. */
        tcp_setprio(newpcb, TCP_PRIO_MIN);

        tcp_arg(newpcb, NULL); // TODO: think of passing Server data-struct as arg here.
        tcp_recv(newpcb, tcpip_raw_recv);
        tcp_err(newpcb, tcpip_raw_error);
        tcp_poll(newpcb, NULL, 0);
        tcp_sent(newpcb, NULL);
        ret_err = ERR_OK;

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
                tcpip_raw_close(tcpip_raw_pcb); // TODO: implement TcpIp close for client socket later
        }

        return retval;
}



// By this API service the TCP/IP stack is requested to bind a UDP or TCP socket
// to a local resource. The Server API.
Std_ReturnType TcpIp_Bind(TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr) {
        Std_ReturnType retval = E_OK;
        err_t err;

        if (PortPtr == NULL && tcpip_raw_pcb != NULL) {
                LOG_ERR("Argument validation failure!");
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
        Std_ReturnType retval;

        if (tcpip_raw_pcb == NULL) {
                LOG_ERR("Argument validation failure!");
                return E_NOT_OK; // TcpIp_Bind is not called!!
        }


        if (tcpip_raw_pcb->state < TCP_LISTEN_STATE) {
                tcpip_raw_pcb = tcp_listen(tcpip_raw_pcb);
                tcp_accept(tcpip_raw_pcb, tcpip_raw_accept);
        }

        // check if accept call back is received by TcpIp module
        if (tcpip_raw_pcb->state == TCP_LISTEN_STATE) {
                retval = E_OK;
        }
        else {
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
                LOG_ERR("tcp_pcp allocation failure!");
                return E_NOT_OK;
        }

        tcp_arg(newpcb, NULL); // TODO: think of passing Client data-struct as arg here.
        tcp_recv(newpcb, tcpip_raw_recv);
        tcp_err(newpcb, tcpip_raw_error);
        tcp_poll(newpcb, NULL, 0);
        tcp_sent(newpcb, NULL);

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

        // call tcp_recved(tpcb, plen); after careful study and thoughts

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