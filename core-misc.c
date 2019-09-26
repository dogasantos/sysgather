#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <stdint.h>
#include "core-misc.h"
#include "core-main.h"
#include "core-output.h"


int MI_Owner(int uid,int gid,char *sfile){
 FILE *fpass,*fgrp;
 char string[MAXBLEN],username[MAXBLEN],groupname[MAXBLEN],userid[32],groupid[32],*buf,*end;
 int size;
    
 	fpass=fopen(PASSWDFILE,"r");
	if(!fpass) return(-1);
	for(;;){
        end=fgets(string,MAXBLEN,fpass);
        if (end==NULL)
            break;
        snprintf(userid,MAXBLEN,"%s",MI_GetField(string,3,':'));
        if(atoi(userid)==uid) {
            snprintf(username,MAXBLEN,"%s",MI_GetField(string,1,':'));
            break;
        }
        else
            snprintf(username,MAXBLEN,"%d",uid); //Do not match any user name... file comes in a tarbal maybe??
    }
    fclose(fpass);
    fgrp=fopen(GROUPFILE,"r");
    if(!fgrp) return(-1);
    for(;;){
        end=fgets(string,MAXBLEN,fgrp);
        if(end==NULL)
            break;
        snprintf(groupid,MAXBLEN,"%s",MI_GetField(string,3,':'));
        if(atoi(groupid)==gid) {
            snprintf(groupname,MAXBLEN,"%s",MI_GetField(string,1,':'));
            break;
        }
        else
            snprintf(groupname,MAXBLEN,"%d",gid); //Do not match any group name... file comes in a tarbal maybe??
    }
    fclose(fgrp);
    
    size=strlen(username)+strlen(groupname)+strlen(sfile)+100;
    buf=(char *)malloc(size);
    memset(buf,0,size);
    snprintf(buf,size,"%s\t%s\t%s",username,groupname,MI_RmChar(sfile,'\n'));
    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
    free(buf);
    
 return 0;
}
int MI_RegMatch(char *string,char *regexpression) {
 regex_t regex;
 int ret;
	
	if(regcomp(&regex,regexpression, REG_EXTENDED|REG_NOSUB) != 0) return -1;
	if(regexec(&regex,string,0,NULL,0) == 0)
		ret=0; //match
	else
		ret=1; //do not match
    regfree(&regex);
 return ret;
}

int MI_CountTok(char *value,char delimiter) {
 int p=0,i=0;
 char buf[MAXBLEN];

	strncpy(buf,value,MAXBLEN);
	while(buf[i]!='\0') {
		i++;
		if(buf[i]==delimiter) p++;
	}
 return ++p;
}

char *MI_GetField(char *msg,int position,char delimiter) {
 static char ret[MAXBLEN];
 int i,cpos,s;

	memset(ret,0,MAXBLEN);
	for(cpos=1,i=0,s=0;;i++) {
		if(msg[i]=='\0') break;
		if(cpos>position) break;
		if(msg[i]==delimiter) { cpos++; continue; }
		if(cpos<position) continue;
		ret[s]=msg[i];
		s++;
	}
	ret[s+1]='\0';
 return ret;
}

char *MI_RmChar(char *value,char target) {
 int i;
 static char ret[MAXBLEN];

	if(strlen(value)>MAXBLEN)
		return "ERROR";
	memset(ret,0,MAXBLEN);
	for(i=0;i<=strlen(value);i++) {
		if(value[i]!=target)
			ret[i]=value[i];
	}
	return ret;
}