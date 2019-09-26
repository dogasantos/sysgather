#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <errno.h>
#include <pwd.h>
#include "core-output.h"
#include "core-misc.h"
#include "core-user.h"
#include "core-main.h"

/* Words to search for in bash history. They might bring interesting information. */
char *bhquery[] = {"ftp", "ssh", "scp","mysql", "http_proxy","user","password","mysqldump","kinit","htpasswd","su","sudo",".key",".crt",NULL};
char *mhquery[ ] = { "create","insert","pass","password","user","account","secret","md5",NULL };
/* Bad words to search for in a potential informative line. If the candidate line contains one of those, ignore the line. */
char *excludebhq[ ] = { "apt","rpm", "dpkg", "emerge","install","remove","man", NULL };
char *excludemhq[ ] = { "delete", NULL };

void USR_MainHandler(){
   
    OutputHandler(OUTPUT_FORMAT,SECTION,"%s","Userspace Informations");
    USR_GetUser();
    USR_ShadFile();
    USR_BashHistory();
    USR_MyHist();
    USR_FreeUserDefs();

}

int USR_GetUser(){
 int uid,gid,userlen,homelen,bhlen,mhlen;
 struct passwd *pw;
 char *bhistory = "/.bash_history";
 char *mhistory = "/.mysql_history";
 char buf[10];

	OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Current User informations:");
	uid=geteuid();
	gid=getegid();
	pw=getpwuid(uid);
	
	OutputHandler(OUTPUT_FORMAT,ITEM,"Current User: %s",pw->pw_name);
	OutputHandler(OUTPUT_FORMAT,ITEM,"User Home: %s",pw->pw_dir);
	memset(buf,0,sizeof(buf));
	snprintf(buf,10,"%d",uid);
	OutputHandler(OUTPUT_FORMAT,ITEM,"Current ID: %s",buf);
	memset(buf,0,sizeof(buf));
	snprintf(buf,10,"%d",gid);
	OutputHandler(OUTPUT_FORMAT,ITEM,"Current GID: %s",buf);
	OutputHandler(OUTPUT_FORMAT,ITEM,"User shell: %s",MI_RmChar(pw->pw_shell,'\n'));

    userlen=strlen(pw->pw_name)+1;
    if(userlen<1) return 1;                 //your uid must be present on /etc/password file... so uid hijack (for nfs hacks) doesnt work! please, get a hell of here!
    userdef.name=malloc(userlen);
    if(userdef.name==NULL) return 1;
    snprintf(userdef.name,userlen,"%s",pw->pw_name);

    homelen=strlen(pw->pw_dir)+1;
    if(homelen<1) return 1;                 //your uid must be present on /etc/password file... so uid hijack (for nfs hacks) doesnt work! please, get a hell of here!
    userdef.home=malloc(homelen);
    if(userdef.home==NULL) return 1;
    snprintf(userdef.home,homelen,"%s",pw->pw_dir);

    bhlen=homelen+strlen(bhistory);
    userdef.bash_history=malloc(bhlen);
    if(userdef.bash_history==NULL) return 1;
    memset(userdef.bash_history,0,bhlen);
    snprintf(userdef.bash_history,bhlen,"%s%s",userdef.home,bhistory);

    mhlen=homelen+strlen(mhistory);
    userdef.mysql_history=malloc(mhlen);
    if(userdef.mysql_history==NULL) return 1;
    memset(userdef.mysql_history,0,mhlen);
    snprintf(userdef.mysql_history,mhlen,"%s%s",userdef.home,mhistory);
    
    return 0;
}
int USR_BashHistory(){
 FILE *fhist;
 char alinebuff[MAXBHLINE+1];
 size_t i,j;
 int bbad;

	OutputHandler(OUTPUT_FORMAT,TITLE,"Interesting itens found on %s",userdef.bash_history);
	fhist = fopen(userdef.bash_history, "r");
	if (!fhist) {
		OutputHandler(OUTPUT_FORMAT,ERROR,"Failed opening bash history: %s",userdef.bash_history);
		return errno;
    }
	while (fgets(alinebuff,MAXBHLINE,fhist) != NULL) {
		bbad = 0;
		/*
		* Search for lines containing key words.
		*/
		for (i = 0; bhquery[i]; i++) {
			if (strstr(alinebuff, bhquery[i]) != NULL) {
			/*
			 * If the line contains one of the bad words we are sure it won't bring interesting information
			 * so ignore it.
			 */
				for (j = 0; excludebhq[j]; j++) {
					if (strstr(alinebuff, excludebhq[j]) != NULL) {
						bbad = 1;
						break;
					}
				}
				if (!bbad)
					OutputHandler(OUTPUT_FORMAT,ITEM,"%s",MI_RmChar(alinebuff,'\n'));
			}
		}
    }
    fclose(fhist);
 return 0;
}
int USR_MyHist() {
 FILE *fhist;
 char alinebuff[MAXBHLINE+1];
 size_t i,j;
 int bbad;

	OutputHandler(OUTPUT_FORMAT,TITLE,"Interesting itens found on %s",userdef.mysql_history);
	fhist = fopen(userdef.mysql_history,"r");
	if (!fhist) {
		OutputHandler(OUTPUT_FORMAT,ERROR,"Failed opening mysql history: %s",userdef.mysql_history);
		return errno;
    }
	while (fgets(alinebuff,MAXBHLINE,fhist) != NULL) {
		bbad = 0;
		/*
		* Search for lines containing key words.
		*/
		for (i=0;mhquery[i];i++) {
			if (strstr(alinebuff, mhquery[i]) != NULL) {
			/*
			 * If the line contains one of the bad words we are sure it won't bring interesting information
			 * so ignore it.
			 */
				for (j=0;excludemhq[j];j++) {
					if (strstr(alinebuff,excludemhq[j]) != NULL) {
						bbad = 1;
						break;
					}
				}
				if (!bbad)
					OutputHandler(OUTPUT_FORMAT,ITEM,"%s",MI_RmChar(alinebuff,'\n'));
			}
		}
    }
    fclose(fhist);
 return 0;
}

int USR_ShadFile(){
 FILE *sf;
 char *end, string[MAXBLEN+1];

	OutputHandler(OUTPUT_FORMAT,TITLE,"Interesting itens found on %s",SHADOWFILE);
	sf = fopen(SHADOWFILE,"r");
	if(!sf) {
		OutputHandler(OUTPUT_FORMAT,ERROR,"Failed opening %s",SHADOWFILE);
		return(errno);
	}
	for(;;){
		end=fgets(string,MAXBLEN,sf);
		if (end==NULL) break;
			if(!MI_RegMatch(string,"[a-z]+[0-9]"))
				OutputHandler(OUTPUT_FORMAT,ITEM,"%s",MI_RmChar(string,'\n'));
	}
    fclose(sf);
 return 0;
}

void USR_FreeUserDefs(){
    free(userdef.name);
    free(userdef.home);
    free(userdef.bash_history);
    free(userdef.mysql_history);
}

