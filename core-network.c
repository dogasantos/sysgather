#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <netdb.h>
#include "core-network.h"
#include "core-main.h"
#include "core-misc.h"
#include "core-output.h"


void NET_MainHandler(){
    
    OutputHandler(OUTPUT_FORMAT,SECTION,"%s","Network Informations");
    NET_IfaceAddr();
    NET_GetDns();
    NET_IsARouter();
    NET_GetRoutes();
    NET_OpenPorts();
}

void NET_OpenPorts(){
    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","TCP/UDP Open Ports:");
    NET_GetTcpSrv();
    NET_GetUdpSrv();
}

/* Function: NET_Ipv4Prefix()
 * Return: int
 * Return Use: prefix value
 * Description: This function is used to retrieve the 
 *				prefix CIDR for a given ipv4 netmask.
 *
 */
int NET_Ipv4Prefix(char *oct){
 char *mask,*oct1, *ptr1;
 char *oct2, *ptr2, *oct3, *ptr3,*oct4, *ptr4;
 int x,w,y,z,n,prefix=0;
 
	mask=(char *)malloc(strlen(oct)+1);
	strcpy(mask,oct);
	oct1 = strtok_r(mask,".",&ptr1);
	oct2 = strtok_r(ptr1,".",&ptr2);
	oct3 = strtok_r(ptr2,".",&ptr3);
	oct4 = strtok_r(ptr3,".",&ptr4);
	x=atoi(oct1);
	w=atoi(oct2);
	y=atoi(oct3);
	z=atoi(oct4);
	for(n=0;n<8; n++){
		if((x & 0x80)!=0)
			++prefix;
		x = x<<1;
		if((w & 0x80)!=0)
			++prefix;
		w = w<<1;
		if((y & 0x80)!=0)	
			++prefix;
		y = y<<1;
		if((z & 0x80)!=0)
			++prefix;
		z = z<<1;	
	}
 return prefix;
}
/* Function: NET_Ipv6Prefix()
 * Return: int
 * Return Use: prefix value
 * Description: This function is used to retrieve the 
 *				prefix for a given ipv6 netmask.
 *
 */
int NET_Ipv6Prefix(void *ifaptr){
 int index,prefix;
 struct ifaddrs *ifa = (struct ifaddrs *)ifaptr; //void casting to supress warnings about struct on prototype
 struct in6_addr *addr;
	
	addr = &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;
	for (prefix=0,index=0;index<16;index++){
		if((int)addr->s6_addr[index]==255) //FF
			prefix += 8;
		if ((int)addr->s6_addr[index]==254) //FE
			prefix += 7;
		if ((int)addr->s6_addr[index]==252) //FC
			prefix += 6;
		if ((int)addr->s6_addr[index]==248) //F8
			prefix += 5;
		if ((int)addr->s6_addr[index]==240) //F0
			prefix += 4;
		if ((int)addr->s6_addr[index]==224) //E0
			prefix += 3;
		if ((int)addr->s6_addr[index]==192) //C0 
			prefix += 2;
		if ((int)addr->s6_addr[index]==128) //80
			prefix += 1;
		if ((int)addr->s6_addr[index]==0) //00
			prefix += 0;
	}
 return prefix;
}

/* Function: NET_IfaceAddr()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function is used to retrieve all IP address
 *              on all interfaces and have support to ipv6 too.
 *              TODO: ipv6 CIDR calcs for netmask
 *
 */

int NET_IfaceAddr(void) {
 struct ifaddrs *ifaddr, *ifa;
 int family, s,ipv4prefix,ipv6prefix;
 char host[NI_MAXHOST];
 char mask[NI_MAXHOST];
 char *addr,*buf;

	if (getifaddrs(&ifaddr) == -1) {
        OutputHandler(OUTPUT_FORMAT,ERROR,"%s","Cannot retrieve interface address");
		return errno;
	}
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6) {
			s = getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			getnameinfo(ifa->ifa_netmask, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), mask, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) return(errno);
			if(!strstr(ifa->ifa_name,"lo")) {
				if (strlen(host)) { //interface has ip address
					if(family == AF_INET6) {
						ipv6prefix=NET_Ipv6Prefix(ifa);
						addr = strtok(host,"%");
                        buf=(char *)malloc(MIDBLEN);
                        memset(buf,0,MIDBLEN);
                        snprintf(buf,MIDBLEN,"IPv6 %s: %s/%d",ifa->ifa_name,addr,ipv6prefix);
                        OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
                        free(buf);
					} else {
						ipv4prefix = NET_Ipv4Prefix(mask);
                        buf=(char *)malloc(MIDBLEN);
                        memset(buf,0,MIDBLEN);
                        snprintf(buf,MIDBLEN,"IPv4 %s: %s/%d",ifa->ifa_name,host,ipv4prefix);
                        OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
                        free(buf);
					}
				}
			}
		}
	}
	freeifaddrs(ifaddr);
 return 0;
}

/* Function: NET_GetDns()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function is used to retrieve the configured
 *              DNS servers. The file "/etc/resolv.conf"
 *              is used to retrieve relevant informations.
 */

int NET_GetDns(){
 FILE *sd;
 char *end,string[MAXBLEN],*ptr,*ptr2;

	sd = fopen(RESOLV,"r");
	if(!sd) {
        OutputHandler(OUTPUT_FORMAT,ERROR,"Cannot open %s file",RESOLV);
		return(errno);
	}
	for(;;){
		end=fgets(string,MAXBLEN-1,sd);
		if (end==NULL) break;
        /*
         * Use regex to skip commented lines
         */
		if(MI_RegMatch(string,"^#")) {
			if(strstr(string,"nameserver")) {
				ptr=MI_GetField(string,2,' ');
				ptr2=MI_RmChar(ptr,'\n');
                OutputHandler(OUTPUT_FORMAT,ITEM,"Nameserver: %s",ptr2);
			}
		}
	}
    fclose(sd);
 return 0;
}

/* Function: NET_GetRoutes()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function is used to retrieve the configured
 *              routing table (ipv4). The file "/proc/net/route"
 *              is used to retrieve relevant informations.
 */

int NET_GetRoutes(){
 FILE *rt;
 int loop,cidr;
 char *iface,*end,*buf;
 char string[MAXBLEN],fulldst[MIDBLEN],fullsrc[MIDBLEN];
 struct in_addr gate,dest,mask;

    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Route Table");
	rt=fopen(ROUTES,"r");
	if(!rt){
        OutputHandler(OUTPUT_FORMAT,ERROR,"%s","Cannot open route table");
		return(errno);
	}
	for (loop=0;;loop++) {
		end = fgets (string,MAXBLEN,rt);
		if(end == NULL) break;
		if(loop>0) { //loop is used to skip header tag line
			dest.s_addr=strtoul(MI_GetField(string,2,'\t'),NULL,16);
			mask.s_addr=strtoul(MI_GetField(string,8,'\t'),NULL,16);
			gate.s_addr=strtoul(MI_GetField(string,3,'\t'),NULL,16);
			cidr=NET_Ipv4Prefix(inet_ntoa(mask));
            iface=(char *)malloc(MAXBLEN);
            memset(iface,0,MAXBLEN);
            snprintf(iface,MAXBLEN,"%s",MI_GetField(string,1,'\t'));
            snprintf(fullsrc,MIDBLEN,"%s/%d",inet_ntoa(dest),cidr);
            snprintf(fulldst,MIDBLEN,"via %s on %s",inet_ntoa(gate),iface);
            buf=(char *)malloc(MAXBLEN);
            memset(buf,0,MAXBLEN);
            snprintf(buf,MAXBLEN,"Route:%s %s",fullsrc,fulldst);
            OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
            free(buf);
            free(iface);
			//printf("  + %s/%d via %s on %s\n",inet_ntoa(dest),cidr,inet_ntoa(gate),iface); <== i dont know whats happening with gateway output... it just bug and shows wrong value
		}
	}
	fclose(rt);
 return 0;
}

/* Function: NET_IsARouter()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function just tell us if the current linux
 *               server is configured with routing capabilities.
 *               This information could be interesting during a
 *               penetration test for some obvious reasons.
 */

int NET_IsARouter(){
 FILE *sd;
 char svalue;

	sd = fopen(ROUTER,"r");
	if (!sd) return(errno);
	svalue = fgetc(sd);
	switch(svalue) {
		case '0': OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Router Capability: 0 (Disabled)"); break;
		case '1': OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Router Capability: 1 (Enabled)"); break;
		default: break;
	}
	fclose(sd);
 return 0;
}
/* Function: NET_GetTcpSrv()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function was designed to retrieve all tcp ports
 *              on LISTENING state. The file "/proc/net/tcp" is
 *              used to get relevant informations.
 *
 */

int NET_GetTcpSrv(){
 FILE *sd;
 char *end,string[MAXBLEN],local[14],*buf;
 int port;
 struct in_addr address;

	sd = fopen(TCPSTATUS,"r");
	if (!sd) return(errno);
    
	for(;;) {
		end = fgets(string,MAXBLEN,sd);
		if(end == NULL) break;
        /*
         * Ok, so now we must look for "0A"
         * this mark tell us about the port state,
         * which means LISTEN state.
         */
		if(strstr(string,TCPLISTEN)) {
            snprintf(local,sizeof(local),"%s",MI_GetField(string,5,' '));
			address.s_addr = strtoul(MI_GetField(local,1,':'),NULL,16);
			port=strtoul(MI_GetField(local,2,':'),NULL,16);
            buf=(char *)malloc(MIDBLEN);
            memset(buf,0,MIDBLEN);
            snprintf(buf,MIDBLEN,"TCP:%s:%d",inet_ntoa(address),port);
            OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
            free(buf);
        }
        /*
         * Ok, we got all open ports, but we don't know
         * what service use these ports.
         * TODO:    some function to get the service running (lsof like for sockets)
         *          maybe /proc fd checking... 
         */
	}
	fclose(sd);
 return 0;
}

/* Function: NET_GetUdpSrv()
 * Return: int
 * Return Use: 0 on success, on error "errno" value is used
 * Description: This function was designed to retrieve all udp ports
 *              on LISTENING state. The file "/proc/net/udp" is
 *              used to get relevant informations.
 *
 */

int NET_GetUdpSrv(){
 FILE *sd;
 char *end,string[MAXBLEN],local[14],*buf;
 int port;
 struct in_addr address;

	sd = fopen(UDPSTATUS,"r");
	if (!sd) return(errno);
	for(;;) {
		end = fgets(string,MAXBLEN,sd);
		if(end == NULL) break;
		if(strstr(string,UDPLISTEN)){
            snprintf(local,sizeof(local),"%s",MI_GetField(string,4,' '));
			address.s_addr = strtoul(MI_GetField(local,1,':'),NULL,16);
			port=strtoul(MI_GetField(local,2,':'),NULL,16);
            buf=(char *)malloc(MIDBLEN);
            memset(buf,0,MIDBLEN);
            snprintf(buf,MIDBLEN,"UDP:%s:%d",inet_ntoa(address),port);
            OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
            free(buf);
		}
	}
	fclose(sd);
 return 0;
}
