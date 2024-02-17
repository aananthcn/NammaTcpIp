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

static struct tcp_pcb *tcpecho_raw_pcb;


Std_ReturnType TcpIp_Bind(TcpIp_SocketIdType SocketId, TcpIp_LocalAddrIdType LocalAddrId, uint16* PortPtr ) {
	Std_ReturnType retval = E_OK;
        err_t err;

	if (PortPtr == NULL) {
		return E_NOT_OK;
	}

        tcpecho_raw_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
        if (tcpecho_raw_pcb != NULL) {
                err = tcp_bind(tcpecho_raw_pcb, IP_ANY_TYPE, *PortPtr);
                if (err != ERR_OK) {
                        retval = E_NOT_OK;
                }
        }

        return retval;
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