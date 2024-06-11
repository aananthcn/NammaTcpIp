/*
 * Created on Mon Jun 10 2024 18:16:52 PM
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
#ifndef SOCKET_CAR_OS_EXTENSIONS_H
#define SOCKET_CAR_OS_EXTENSIONS_H

#include <stdint.h>
#include <Platform_Types.h>

#include "tcpip_extensions.h"

int socket(int sockfd);

int bind(int sockfd, uint16_t port);

int listen(int sockfd);

int accept(int sockfd);

int recv(int sockfd, uint8_t* data, uint32_t length);

int send(int sockfd, uint8_t* data, uint32_t length);

#endif