/*
 * Created on Thu Feb 02 2023 2:11:51 AM
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

#include "tcpip_mpool.h"


//////////////////////////////////////////////
// BASIC ETHERNET Tx & Rx Buffers
static tcpip_mpool_t TcpIp_MemPool[TCPIP_MEM_POOL_SIZE];


// Memory Pool Functions
tcpip_mpool_t* get_new_tcpip_mpool(void) {
        tcpip_mpool_t* mpool_ptr = NULL;
        u16 i;

        for (i = 0; i < TCPIP_MEM_POOL_SIZE; i++) {
                if (TcpIp_MemPool[i].state == MPOOL_FREE) {
                        TcpIp_MemPool[i].state = MPOOL_ACQUIRED;
                        mpool_ptr = &TcpIp_MemPool[i];
                        break;
                }
        }

        return mpool_ptr;
}



tcpip_mpool_t* get_tcpip_mpool_w_data(void) {
        tcpip_mpool_t* mpool_ptr = NULL;
        u16 i;

        for (i = 0; i < TCPIP_MEM_POOL_SIZE; i++) {
                if (TcpIp_MemPool[i].state == MPOOL_DATA_FILLED) {
                        mpool_ptr = &TcpIp_MemPool[i];
                        break;
                }
        }

        return mpool_ptr;
}



boolean free_tcpip_mpool(tcpip_mpool_t* p_mpool) {
        boolean retval = TRUE;

        if ((p_mpool >= TcpIp_MemPool) && (p_mpool <= &TcpIp_MemPool[TCPIP_MEM_POOL_SIZE])) {
                p_mpool->state = MPOOL_FREE;
        }
        else {
                retval = FALSE;
        }

        return retval;
}
