/*
 * Created on Fri Feb 23 2024 6:45:36 PM
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

#ifndef TCPIP_MEM_POOL_H
#define TCPIP_MEM_POOL_H

#include <Platform_Types.h>
#include <Std_Types.h>
#include <stddef.h>


#define TCPIP_MEM_POOL_SIZE           (5)


typedef enum {
        MPOOL_FREE,
        MPOOL_ACQUIRED,
        MPOOL_DATA_FILLED,
        MAX_MPOOL_STATE
} tcpip_mpool_state_t;


typedef struct tcpip_mpool {
        struct tcpip_mpool *next;
        struct pbuf *pbuf;
        tcpip_mpool_state_t state;
} tcpip_mpool_t;


tcpip_mpool_t* get_new_tcpip_mpool(void);
tcpip_mpool_t* get_tcpip_mpool_w_data(void);
boolean free_tcpip_mpool(tcpip_mpool_t* p_mpool);


#endif
