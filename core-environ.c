#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <gnu/libc-version.h>
#include <features.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <errno.h>
#include "core-environ.h"
#include "core-misc.h"
#include "core-main.h"
#include "core-output.h"

const char *distrofiles[] = {	"/etc/SUSE-release",        //"SUSE",
								"/etc/redhat-release",      //"REDHAT1",
								"/etc/redhat_version",      //"REDHAT2",
								"/etc/fedora-release",      //"FEDORA",
								"/etc/slackware-release",   //"SLACKWARE1",
								"/etc/slackware-version",   //"SLACKWARE2",
								"/etc/debian_release",      //"DEBIAN1",
								"/etc/debian_version",      //"DEBIAN2",
								"/etc/mandrake-release",    //"MANDRAKE",
								"/etc/yellowdog-release",   //"YELLOWDOG",
								"/etc/sun-release",         //"SUNJDS",
								"/etc/release",             //"SOLARIS",
								"/etc/gentoo-release",      //"GENTOO",
								"/etc/UnitedLinux-release", //"UNITEDLINUX",
								"/etc/lsb-release",         //"UBUNTU",
							NULL };


void ENV_MainHandler() {
    OutputHandler(OUTPUT_FORMAT,SECTION,"%s","System Environment");
    ENV_HostId();                    // Host ID forensic style
    ENV_GetProcessor();              // Processor informations
    ENV_MemnLoad();                  // Memory and Load Average
    ENV_MountTable();                // Mount Points with special flags
    ENV_KernelVersion();             // Kernel Informations
    ENV_KernelMetrics();             // Kernel Metrics
    ENV_BuildVer();                  // Gcc Version, Libc Version
    ENV_DistroCheck();               // Linux Distribution check
}
void ENV_KernelMetrics(){
 int opt;
	OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Kernel Metrics");
    for(opt=0;;opt++)
        if(ENV_GetSysConf(opt))
            break;
}
int ENV_GetSysConf(int opt){
 char *buf;
    
	switch(opt) {
		case PAGESIZE: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_PAGE_SIZE: %ld",sysconf(_SC_PAGE_SIZE));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		case CHILDMAX: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_CHILD_MAX: %ld",sysconf(_SC_CHILD_MAX));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		case OPENMAX: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_OPEN_MAX: %ld",sysconf(_SC_OPEN_MAX));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		case STREAMMAX: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_STREAM_MAX: %ld",sysconf(_SC_STREAM_MAX));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		case PHYSPAGES: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_PHYS_PAGES: %ld",sysconf(_SC_PHYS_PAGES));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		case AVPHYSPAGES: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"_SC_AVPHYS_PAGES: %ld",sysconf(_SC_AVPHYS_PAGES));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} return 0;
		default: return 1;
	}
    return 1;
}
int ENV_GetProcessor() {
 static const char *query[ ] = { "model name", NULL };
 char *end=NULL,*token=NULL;
 char linha[MAXBLEN];
 int i;
 FILE *fp;
    
	fp = fopen(CPUFILE,"r");
	if (!fp) return(errno);
    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Processor Informations");
	for(;;){
		end = fgets(linha,MAXBLEN-1,fp);
		if (end == NULL) break;
		for (i=0;query[i];i++) {
			if(strstr(linha,query[i])!= NULL) {
				token=strtok(linha,":");
				while(token) {
					if(strstr(token,query[i]) == NULL){
                        OutputHandler(OUTPUT_FORMAT,ITEM,"Processor:%s",MI_RmChar(token,'\n'));
                    }
					token=strtok(NULL,":");
				}
			}
		}
	}
	fclose(fp);
    return 0;
}
int ENV_MountTable() {
 FILE *dm;
 char *end,mp[MAXBLEN],string[MAXBLEN],*token=NULL,*token1=NULL,*token2=NULL,*buf=NULL;
 int i,qtde,size;
 static const char *dmquery[ ] = { "noexec","nosuid","ntfs","sshfs","loop",NULL };
 dm = fopen(MTAB,"r");
 if(!dm) return(errno);
    
    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Mounpoints special flags");
	for(;;) {
		end=fgets(string,MAXBLEN,dm);
		if (end==NULL) break;
		token=strdup(string);
		strtok_r(token," ",&token1);
		strtok_r(token1," ",&token);
		strcpy(mp,token1);
		strtok_r(token," ",&token2);
		token=strdup(token2);
		strcpy(string,token);
		qtde=MI_CountTok(string,',');
		for(;qtde>0;qtde--) {
			token2=MI_GetField(string,qtde,',');
			for (i=0;dmquery[i];i++)
				if(strstr(token2,dmquery[i]) != NULL) {
                    size=strlen(dmquery[i])+strlen(mp)+100;
                    buf=(char *)malloc(size);
                    snprintf(buf,size,"%s:\t%s",dmquery[i],mp);
                    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
                    free(buf);
                }
		}
	}
	fclose(dm);
    return 0;
}
int ENV_MemnLoad(){
 const long minute = 60;
 const long hour = minute * 60;
 const long day = hour * 24;
 const double megabyte = 1024 * 1024;
 struct sysinfo si;
 char *buf;
    
    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Memory and load average");
	if(sysinfo(&si))
		return(errno);
    buf=(char *)malloc(MAXBLEN);
    memset(buf,0,MAXBLEN);
    snprintf(buf,MAXBLEN,"Uptime:%ld days, %ld:%02ld:%02ld",si.uptime / day, (si.uptime % day) / hour, (si.uptime % hour) / minute, si.uptime % minute);
    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
    memset(buf,0,MAXBLEN);
    snprintf(buf,MAXBLEN,"Total RAM:%5.1f MB",si.totalram / megabyte);
    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
    memset(buf,0,MAXBLEN);
    snprintf(buf,MAXBLEN,"Free RAM:%5.1f MB",si.freeram / megabyte);
    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
    /* Disabled:
     memset(buf,0,MAXBLEN);
     snprintf(buf,MAXBLEN,"Running Process: %d",si.procs);
     OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
     */
    free(buf);
    return 0;
}
void ENV_BuildVer(){
	OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Build Tools Version");
	ENV_LibcVer();
	ENV_GccVer();
}
int ENV_KernelVersion(){
 struct utsname info_s;

	if (uname(&info_s)>=0){
		OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Running Kernel");
		OutputHandler(OUTPUT_FORMAT,ITEM,"Kernel: %s",info_s.release);
		OutputHandler(OUTPUT_FORMAT,ITEM,"Date: %s",info_s.version);
		OutputHandler(OUTPUT_FORMAT,ITEM,"Architeture: %s",info_s.machine);
		OutputHandler(OUTPUT_FORMAT,ITEM,"Node: %s",info_s.nodename);
	}
	return 0;
}
void ENV_LibcVer() {
	OutputHandler(OUTPUT_FORMAT,ITEM,"Libc version: %s",(char *)gnu_get_libc_version());
	OutputHandler(OUTPUT_FORMAT,ITEM,"Libc Release: %s",(char *)gnu_get_libc_release());
}
void ENV_GccVer(){
#  if __GNUC_PREREQ(4,8)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.8");
	return;
#endif
#  if __GNUC_PREREQ(4,7)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.7");
	return;
#endif
#  if __GNUC_PREREQ(4,6)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.6");
	return;
#endif
#  if __GNUC_PREREQ(4,5)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.5");
	return;
#endif
#  if __GNUC_PREREQ(4,4)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.4");
	return;
#endif
#  if __GNUC_PREREQ(4,3)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.3");
	return;
#endif
#  if __GNUC_PREREQ(4,2)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.2");
	return;
#endif
#  if __GNUC_PREREQ(4,1)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.1");
	return;
#endif
#  if __GNUC_PREREQ(4,0)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 4.0");
	return;
#endif
#  if __GNUC_PREREQ(3,4)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 3.4");
	return;
#endif
#  if __GNUC_PREREQ(3,3)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 3.3");
	return;
#endif
#  if __GNUC_PREREQ(3,2)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 3.2");
	return;
#endif
#  if __GNUC_PREREQ(3,1)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 3.1");
	return;
#endif
#  if __GNUC_PREREQ(3,0)
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Gcc version: 3.0");
	return;
#endif
}

void ENV_HostId(){
 char *buf;
 int size=10;

	buf=(char *)malloc(size);
	memset(buf,0,size);
	snprintf(buf,size,"%08x",(unsigned int)gethostid());
	OutputHandler(OUTPUT_FORMAT,ITEM,"Host ID: %s",buf);
	free(buf);
}

int ENV_DistroCheck(){
 FILE *sd;
 char svalue[MAXBLEN];
 int i=0;
 boolean dfound;

	while(distrofiles[i]!=NULL){
		if(access(distrofiles[i], R_OK)==0) {
			sd = fopen(distrofiles[i],"r");
			if (!sd) return(1);
			while (fgets(svalue,MAXBLEN,sd) != NULL) {
				if(MI_CountTok(svalue,'\n') > 0) {
					printf("  + Linux Distro: %s\n",MI_GetField(svalue,1,'\n'));
					dfound=true;
					break;
				}
			}
			fclose(sd);
		}
		i++;
	}
	if(dfound!=true) printf("  + Linux Distro: Uknown\n");
 return 0;
}