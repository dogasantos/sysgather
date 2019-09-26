#ifndef CORE_SECURITY_H
#define CORE_SECURITY_H 1
#include "core-main.h"

#define S_SUID		0
#define	S_FILES		1
#define S_DIRW		2
#define S_FILW		3

#define SSHURSAKEY  ".ssh/id_rsa"
#define SSHUDSAKEY  ".ssh/id_dsa"
#define SSHAUTHKEY  ".ssh/authorized_keys"
#define GRSECURITY	"/proc/sys/kernel/grsecurity"
#define PROCPAX		"/proc/sys/kernel/pax"
#define SELINUX		"/etc/selinux/config"
#define APPARMOR    "/proc/sys/kernel/security/apparmor"
#define RANDOMIZEVA "/proc/sys/kernel/randomize_va_space"
#define SUIDDUMP	"/proc/sys/fs/suid_dumpable"
#define MINADDR		"/proc/sys/vm/mmap_min_addr"
#define SELFMAPS    "/proc/self/maps"
#define PAGESIZE	0
#define CHILDMAX	1
#define OPENMAX		2
#define STREAMMAX	3
#define PHYSPAGES	4
#define AVPHYSPAGES	5

struct FileQuery {
    int type;
    char *pattern;
} fquery;

extern void SEC_MainHandler();
extern int SEC_FtwCallback(const char *name, const struct stat *status, int type);
extern int SEC_FindFiles(char *path,int mode,char *string);
extern int SEC_SuidDump();
extern int SEC_MinAddr();
extern int SEC_NxCheck();
extern int SEC_AslrCheck();
extern int SEC_Selinux();
extern int SEC_CheckPatch();
extern void SEC_InterestingFiles();
extern int SEC_SshKeys();
extern int SEC_AsciiArmor();

#endif

