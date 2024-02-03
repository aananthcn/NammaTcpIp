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

#include <os_api.h> // for LOG_ERR()
#include <macphy.h> // TODO: revisit this

#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include <lwip/init.h>
#include <lwip/etharp.h>
#include <lwip/dhcp.h>
#include <lwip/timeouts.h>


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(TcpIp, LOG_LEVEL_DBG);



#define ETHERNET_MTU 1500

/////////////////////////////////////////////////
// temporary functions
static err_t netif_output(struct netif *netif, struct pbuf *p)
{
    LINK_STATS_INC(link.xmit);

    /* Start MAC transmit */
    LOG_DBG("TcpIp: Sending packet of len %d", p->len);
    macphy_pkt_send((uint8_t *)p->payload, p->len);

    return ERR_OK;
}



static void netif_status_callback(struct netif *netif)
{
    LOG_INF("netif status changed %s", ip4addr_ntoa(netif_ip4_addr(netif)));
}



static err_t netif_initialize(struct netif *netif)
{
	const uint8_t *mac;
	netif->linkoutput = netif_output;
	netif->output = etharp_output;
	// netif->output_ip6 = ethip6_output;
	netif->mtu = ETHERNET_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
	// MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);
	mac = EthConfigs[0].ctrlcfg.mac_addres;
	SMEMCPY(netif->hwaddr, mac, sizeof(netif->hwaddr));
	netif->hwaddr_len = sizeof(netif->hwaddr);
	return ERR_OK;
}



/////////////////////////////////////////////////


uint8_t eth_pkt[ETHERNET_MTU];
struct netif netif;

// Function: TcpIp_Init will be called by EcuM
void TcpIp_Init(const TcpIp_ConfigType* ConfigPtr) {
	ip4_addr_t addr, mask, static_ip;

	IP4_ADDR(&static_ip, 192, 168, 3, 111);
	IP4_ADDR(&mask, 255, 255, 255, 0);
	IP4_ADDR(&addr, 192, 168, 3, 1);

	lwip_init();
	netif_add(&netif, &static_ip, &mask, &addr, NULL, netif_initialize, netif_input);
	netif.name[0] = 'e';
	netif.name[1] = '0';

	netif_set_status_callback(&netif, netif_status_callback);
	netif_set_default(&netif);
	netif_set_up(&netif);

	dhcp_inform(&netif);
	// eth_pkt = malloc(ETHERNET_MTU);

	netif_set_link_up(&netif);

	LOG_INF("Init complete!");
}



// Function: TcpIp_MainFunction will be called by Os
void TcpIp_MainFunction(void) {
	struct pbuf *p = NULL;
	uint16_t packet_len;

	// following line is a temporary test
	packet_len = macphy_pkt_recv((uint8_t *)eth_pkt, ETHERNET_MTU);
	if (packet_len) {
		LOG_DBG("TcpIp: Received packet of length = %d", packet_len);
		p = pbuf_alloc(PBUF_RAW, packet_len, PBUF_POOL);
		pbuf_take(p, eth_pkt, packet_len);
		// free(eth_pkt);
		// eth_pkt = malloc(ETHERNET_MTU);
	}

	if (packet_len && p != NULL) {
		LINK_STATS_INC(link.recv);

		if (netif.input(p, &netif) != ERR_OK) {
			pbuf_free(p);
		}
	}

	/* Cyclic lwIP timers check */
	sys_check_timeouts();
}