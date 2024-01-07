#ifndef __NET_IPADDR_H__
#define __NET_IPADDR_H__

#include "types.h"

extern int get_localip(const char *netif, char *o_ipaddr, int size);
extern int set_localip(const char *netif, const char *ipaddr);

extern bool is_valid_ip(const char ipaddr[16]);  /*literal checking for all: IP, netmask, or subnet*/
extern bool is_valid_ip2(const char ipaddr[16]); /*firstly binding the ip; then unbind it: just for IP, not for netmask, nor subnet*/
extern bool is_valid_netmask(const char netmask[16], uint32_t expected);
extern bool is_valid_netmask2(uint32_t netmask, uint32_t expected);

extern int get_localnetmask(const char *netif, char *o_netmask, int size);
extern int set_localnetmask(const char *netif, const char *szNetMask);

extern int get_localmac(const char *netif, char * o_hwaddr, int size);

extern int set_localgateway(const char *gw);
extern int get_localgateway(const char *netif, char *o_gw, int size);

extern int set_localip2netmask(const char *netif, const char *ipaddr, const char *szNetMask);

extern int down_localnetif(const char *netif);
extern int up_localnetif(const char *netif);

extern bool netif_exist(const char *ifc);

bool net_check_default_route(const char *ifc);

#endif /*__QXWZ_NET_IPADDR_H__*/
