#ifndef CORE_ENVIRON_H
#define CORE_ENVIRON_H 1

#define MEMFILE			"/proc/meminfo"
#define CPUFILE			"/proc/cpuinfo"
#define MTAB			"/etc/mtab"

#define LSBMODE		100
#define RELFMODE	101
#define PAGESIZE	0
#define CHILDMAX	1
#define OPENMAX		2
#define STREAMMAX	3
#define PHYSPAGES	4
#define AVPHYSPAGES	5

extern void ENV_MainHandler();
extern void ENV_KernelMetrics();
extern int ENV_GetSysConf(int opt);
extern int ENV_GetProcessor();
extern int ENV_MountTable();
extern int ENV_MemnLoad();
extern void ENV_BuildVer();
extern int ENV_KernelVersion();
extern void ENV_LibcVer();
extern void ENV_GccVer();
extern void ENV_HostId();
extern int ENV_DistroCheck();


#endif

