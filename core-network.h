#ifndef CORE_NETWORK_H
#define CORE_NETWORK_H 1

#define RESOLV		"/etc/resolv.conf"
#define ROUTES		"/proc/net/route"
#define ROUTER		"/proc/sys/net/ipv4/ip_forward"
#define TCPSTATUS	"/proc/net/tcp"
#define UDPSTATUS	"/proc/net/udp"
#define TCPLISTEN   "0A"
#define UDPLISTEN   "07"

extern int NET_Ipv4Prefix(char *oct);
extern int NET_Ipv6Prefix(void *ifa);
extern int NET_IfaceAddr();
extern int NET_GetDns();
extern int NET_GetRoutes();
extern int NET_IsARouter();
extern int NET_GetTcpSrv();
extern int NET_GetUdpSrv();
extern void NET_MainHandler();
extern void NET_OpenPorts();

#endif

