# /*
#  * Created on Thu Jan 26 2023 10:12:29 PM
#  *
#  * The MIT License (MIT)
#  * Copyright (c) 2023 Aananth C N
#  *
#  * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
#  * and associated documentation files (the "Software"), to deal in the Software without restriction,
#  * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
#  * subject to the following conditions:
#  *
#  * The above copyright notice and this permission notice shall be included in all copies or substantial
#  * portions of the Software.
#  *
#  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
#  * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#  * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#  */

include ${CAR_OS_PATH}/path_defs.mk
include ${TCPIP_PATH}/lwip_filelists.mk

LWIPDIR  := ${TCPIP_PATH}/lwip/src
LWIP_DIR := ${TCPIP_PATH}/lwip

INCDIRS  += -I ${TCPIP_PATH}/lwip/src/include \
            -I ${TCPIP_PATH}/cfg \
            -I ${TCPIP_PATH}/api \
	    -I ${CAR_OS_INC_PATH}/autosar \
	    -I ${CAR_OS_INC_PATH}/car_os \
	    -I ${CAR_OS_BOARDSOC_PATH} \
	    -I ${OS_PATH}/include \
	    -I ${MCU_PATH}/src \
 	    -I ${MCU_PATH}/src/common \
	    -I ${MCU_PATH}/src/common/src \
	    -I ${MCU_PATH}/src/common/api \
	    -I ${OS_PATH}/include \
	    -I ${OS_BUILDER_PATH}/src \
	    -I ${SPI_PATH}/cfg \
	    -I ${ETH_PATH}/cfg \
	    -I ${ETH_PATH}/src/macphy \
	    -I ${ETH_PATH}/api

# TODO: remove 👆🏻 SPI_PATH, ETH_PATH, ETH_PATH after 15-Feb-2023; these are added for testing on 28 Jan 2023


$(info  )
$(info compiling TcpIp source files)


TCPIP_OBJS := \
	${TCPIP_PATH}/src/sys_arch.o \
	${TCPIP_PATH}/src/lwip_int.o \
	${TCPIP_PATH}/src/tcpip_mpool.o \
	${TCPIP_PATH}/cfg/TcpIp_cfg.o \
	${TCPIP_PATH}/src/TcpIp.o


TARGET 	:= libTcpIp.a

# include c_l_flags.mk to add more definitions specific to micro-controller
include ${CAR_OS_PATH}/c_l_flags.mk


%.o: %.c
	$(CC) -c ${CFLAGS} ${INCDIRS} $< -o $@


all: $(TARGET)

LWIP_SRCS := $(LWIPNOAPPSFILES)
LWIP_OBJS := ${LWIP_SRCS:.c=.o}

LIB_OBJS := $(LWIP_OBJS) $(TCPIP_OBJS)

$(TARGET): $(LIB_OBJS)
	$(AR) -rcs ${TARGET} ${LIB_OBJS}


clean:
	$(RM) $(LIB_OBJS) $(TARGET)
