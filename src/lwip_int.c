/*
 * Created on Fri Feb 16 2024 11:59:35 PM
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

// This file contains code that is required to integrate LWIP to Car-OS

#include <Eth.h>
#include <TcpIp_cfg.h>

#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/init.h>
#include <lwip/etharp.h>
#include <lwip/dhcp.h>
#include <lwip/timeouts.h>

#include <macphy.h> // TODO: revisit this


#define ETHERNET_MTU 1500

uint8_t Lwip_EthPkt[ETHERNET_MTU];
struct netif Lwip_NetIf[MAX_TCPIP_LOCAL_ADDRESS];

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lwip_int, LOG_LEVEL_DBG);


/////////////////////////////////////////////////
// netif functions
static err_t netif_output(struct netif *netif, struct pbuf *p) {
        LINK_STATS_INC(link.xmit);

        /* Start MAC transmit */
        LOG_DBG("TcpIp: Sending packet of len %d", p->len);
        macphy_pkt_send((uint8_t *)p->payload, p->len);

        return ERR_OK;
}



static void netif_status_callback(struct netif *netif) {
        LOG_INF("netif[%c%c] status changed %s", netif->name[0], netif->name[1],
                ip4addr_ntoa(netif_ip4_addr(netif)));
}



static err_t netif_initialize(struct netif *netif) {
        const uint8_t *mac;

        netif->linkoutput = netif_output;
        netif->output = etharp_output;
        netif->mtu = ETHERNET_MTU;
        netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | 
                        NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;

        // netif->output_ip6 = ethip6_output;
        // MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);

        mac = EthConfigs[0].ctrlcfg.mac_addres;
        SMEMCPY(netif->hwaddr, mac, sizeof(netif->hwaddr));
        netif->hwaddr_len = sizeof(netif->hwaddr);

        return ERR_OK;
}



///////////////////////////////////////////////////////////////////////////////
// lwip integration functions
void lwip_int_main(void) {
        struct pbuf *p = NULL;
        uint16_t packet_len;
        uint16_t i;

        // following line is a temporary test, this should be replaced by EthIf APIs
        packet_len = macphy_pkt_recv((uint8_t *)Lwip_EthPkt, ETHERNET_MTU);
        if (packet_len) {
                LOG_DBG("TcpIp: Received packet of length = %d", packet_len);
                p = pbuf_alloc(PBUF_RAW, packet_len, PBUF_POOL);
                pbuf_take(p, Lwip_EthPkt, packet_len);
                // free(Lwip_EthPkt);
                // Lwip_EthPkt = malloc(ETHERNET_MTU);
        }

        if (packet_len && p != NULL) {
                LINK_STATS_INC(link.recv);

                for (i = 0; i < MAX_TCPIP_LOCAL_ADDRESS; i++) {
                        if (Lwip_NetIf[i].input(p, &Lwip_NetIf[i]) != ERR_OK) {
                                pbuf_free(p);
                        }
                }
        }

        /* Cyclic lwIP timers check */
        sys_check_timeouts();
}



void lwip_int_init(void) {
        ip4_addr_t addr, mask, static_ip;
        u16 i;

        // TODO: refactor this code by adding a for loop to initialize all EthIf
        if (MAX_TCPIP_LOCAL_ADDRESS < 1) {
                LOG_ERR("Ethernet local address not configured!");
                return;
        }

        for (i = 0; i < MAX_TCPIP_LOCAL_ADDRESS; i++) {
                // IP Address
                IP4_ADDR(&static_ip, TcpIpLocalAddrConfigs[i].ip_addr[0], TcpIpLocalAddrConfigs[i].ip_addr[1],
                        TcpIpLocalAddrConfigs[i].ip_addr[2], TcpIpLocalAddrConfigs[i].ip_addr[3]);
                // IP Netmask
                IP4_ADDR(&mask, TcpIpLocalAddrConfigs[i].ip_netmask[0], TcpIpLocalAddrConfigs[i].ip_netmask[1],
                        TcpIpLocalAddrConfigs[i].ip_netmask[2], TcpIpLocalAddrConfigs[i].ip_netmask[3]);
                // Default Router IP
                IP4_ADDR(&addr, TcpIpLocalAddrConfigs[i].ip_dfroutr[0], TcpIpLocalAddrConfigs[i].ip_dfroutr[1],
                        TcpIpLocalAddrConfigs[i].ip_dfroutr[2], TcpIpLocalAddrConfigs[i].ip_dfroutr[3]);

                lwip_init();
                netif_add(&Lwip_NetIf[i], &static_ip, &mask, &addr, NULL, netif_initialize, netif_input);
                Lwip_NetIf[i].name[0] = 'e';
                Lwip_NetIf[i].name[1] = '0' + TcpIpLocalAddrConfigs[i].addr_id;

                netif_set_status_callback(&Lwip_NetIf[i], netif_status_callback);
                netif_set_default(&Lwip_NetIf[i]);
                netif_set_up(&Lwip_NetIf[i]);

                dhcp_inform(&Lwip_NetIf[i]);
                // Lwip_EthPkt = malloc(ETHERNET_MTU);

                netif_set_link_up(&Lwip_NetIf[i]);
        }
}
