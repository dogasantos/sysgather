#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ftw.h>
#include "core-list.h"
#include "core-output.h"
#include "core-misc.h"
#include "core-security.h"
#include "core-environ.h"
#include "base64.h" // NibbleAndAHalf project

char *specialfiles[ ] = { ".bash_history",".mysql_history",".htpasswd",".rhosts", NULL };
int index_list;

void SEC_MainHandler() {
    OutputHandler(OUTPUT_FORMAT,SECTION,"%s","Security Informations");
    
	// - Hids and Nids Presence
    
    SEC_AslrCheck();                         // Stack Randomization state
    SEC_SuidDump();                          // Suid Dumpable State
    SEC_MinAddr();                           // Minimal Address Map Value
    SEC_AsciiArmor();                        // ASCII ARMOR is in place?
    SEC_CheckPatch();                        // Selinux, Apparmor, GrSec, Pax
    SEC_NxCheck();                           // Nx Flag Check
    SEC_SshKeys();                           // base64 hashed SSH Keys
    SEC_FindFiles("/",0,NULL);               // List all suid files
    if(getpid()!=0 && getgid()!=0){          // Skip when root
        SEC_FindFiles("/",3,NULL);           // List all writable *
        SEC_FindFiles("/",2,NULL);           // List all writable *
    }
    SEC_InterestingFiles();                  // List all interesting files
    
}
int SEC_AsciiArmor() {
 static const char *query[ ] = { "libc", NULL };
 char *end=NULL;
 char string[MAXBLEN],Mapped[MIDBLEN];
    int found;
 FILE *fp;
    
	fp=fopen(SELFMAPS,"r");
	if (!fp)
        return(-1);
	for(found=0;;){
        end = fgets(string,MAXBLEN,fp);
        if (end == NULL) break;
        if(strstr(string,query[0])!= NULL) {
            snprintf(Mapped,MIDBLEN,"%s",MI_GetField(string,1,' '));
            if(!strncmp(Mapped,"00",2))
                found=1;
        }
    }
    fclose(fp);
    if(!found)
        OutputHandler(OUTPUT_FORMAT,ITEM,"%s","ASCII-Armor: Not Present");
    else
        OutputHandler(OUTPUT_FORMAT,ITEM,"%s","ASCII-Armor: Present");    
 return 0;
}
int SEC_SshKeys() {
 struct node *a;
 int encoded_len,file_size,buf_size,read;
 unsigned char *input;
 char *encoded_data,*buf;
 FILE *fd;
 struct stat attr;

    OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Hashed SSH Keys");
    LST_InitHeader();
    index_list=0;
    SEC_FindFiles("/",S_FILES,SSHURSAKEY); //SEC_FindFiles feeds the linked list
    SEC_FindFiles("/",S_FILES,SSHUDSAKEY);  
    SEC_FindFiles("/",S_FILES,SSHAUTHKEY); 
    a=header->first;
    while(a) {
        stat(a->fullpath,&attr);
        file_size=attr.st_size;
        if(file_size<134217728) {
            fd=fopen(a->fullpath,"r");
            if(!fd)
                break;
            else {
                input=(unsigned char *)malloc(file_size); //i dont like the idea of alloc large space (like 128mb) of memory... but base64 appears to not support incremental hashing,like openssl's CTX update sha1 schematics...
                read=fread(input,1,file_size,fd);
                if(read>0){
                    encoded_data=base64(input,file_size+1,&encoded_len);
                    buf_size=encoded_len+strlen(a->fullpath);
                    buf=(char *)malloc(buf_size);
                    memset(buf,0,buf_size);
                    snprintf(buf,buf_size,"%s:%s",a->fullpath,encoded_data);
                    OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
                    free(buf);
                    free(input);
                }
            }
        }
        a=a->next;
    }
    LST_DestroyList();
 return 0;
}
int SEC_NxCheck() {
 static const char *query[ ] = { "flags","nx", NULL };
 char *end=NULL,*token=NULL;
 char linha[MAXBLEN];
 int i,found,qtde,len;
 FILE *fp;
    
	fp = fopen(CPUFILE,"r");
	if (!fp) return(errno);
	for(;;){
		end = fgets(linha,MAXBLEN,fp);
		if (end == NULL) break;
        if(strstr(linha,query[0])!= NULL) {
            qtde=MI_CountTok(linha,' ');
            for(i=2,found=0;i<=qtde;i++) {
                len=strlen(MI_GetField(linha,i,' '))+1;
                if(len>1) {
                    token=(char *)malloc(len);
                    memset(token,0,len);
                    snprintf(token,len,"%s",MI_GetField(linha,i,' '));
                    if(!strncmp(token,query[1],2))
                        found=1;
                    free(token);
                }
            }
        }
    }
    fclose(fp);
    if(!found)
        OutputHandler(OUTPUT_FORMAT,ITEM,"%s","NX Flag: Not Present");
    else
        OutputHandler(OUTPUT_FORMAT,ITEM,"%s","NX Flag: Present");

 return 0;
}

int SEC_FtwCallback(const char *name, const struct stat *status, int type) {
 int stuid,stgid;
 char *enty=(char *)name;
    
    if ((type == FTW_D) && fquery.type==S_DIRW) {
        if(!access(enty,W_OK)){
            if(!strstr(enty,"/proc")) {
				stuid=status->st_uid;
				stgid=status->st_gid;
				MI_Owner(stuid,stgid,enty);
            }
        }
    }
    if ((type == FTW_F) && fquery.type==S_SUID) {
		if(!strstr(enty,"/dev") && !strstr(enty,"/proc") && !strstr(enty,"/sys")) {
			if ((status->st_mode & S_ISUID)== S_ISUID) {
				stuid=status->st_uid;
				stgid=status->st_gid;
				MI_Owner(stuid,stgid,enty);
			} else return 0;
        }
    }
    if ((type == FTW_F) && fquery.type==S_FILES) {
		if(!strstr(enty,"/proc")) {
	        if (strstr(enty,fquery.pattern)){
				stuid=status->st_uid;
                stgid=status->st_gid;
                ++index_list;
                LST_InsertDataList(enty,stuid,stgid,index_list); //joga na lista encadeada
	            //MI_Owner(stuid,stgid,enty); //<- essa funcao eh chamada junto com a funcao de mostrar item de posicao da lista (joga num for e referencia item da lista pela posicao, pega o campo e joga no MI_Owner()
                
			}
        }
    }
    if ((type == FTW_F) && fquery.type==S_FILW) {
        if(!access(enty,W_OK)) {
            if(!strstr(enty,"/dev") && !strstr(enty,"/proc")) {
				stuid=status->st_uid;
				stgid=status->st_gid;
				MI_Owner(stuid,stgid,enty);
            }
        }
    }
    return 0;
}

int SEC_FindFiles(char *path,int mode,char *string) {
    
    if(mode==S_SUID) {           //suid find
        fquery.type=S_SUID;
		OutputHandler(OUTPUT_FORMAT,TITLE,"%s","List of suid files");
        ftw(path,SEC_FtwCallback,0);
    }
    if(mode==S_FILES) {           //search files
        fquery.type=S_FILES;
        fquery.pattern=string;
        ftw(path,SEC_FtwCallback,0);
    }
    if(mode==S_DIRW) {           //dir write perm search
        fquery.type=S_DIRW;
		OutputHandler(OUTPUT_FORMAT,TITLE,"%s","List of writable directories");
        ftw(path,SEC_FtwCallback,1);
    }
    if(mode==S_FILW) {           //file write perm search
        fquery.type=S_FILW;
		OutputHandler(OUTPUT_FORMAT,TITLE,"%s","List of writable files");
        ftw(path,SEC_FtwCallback,0);
    }
    return 0;
}

int SEC_SuidDump() {
 FILE *sd;
 char svalue,*buf;
    
	sd = fopen(SUIDDUMP,"r");
	if (!sd) return(errno);
	svalue = fgetc(sd);
	switch(svalue) {
		case '0': {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"Suid dumpable: %c (default)",svalue);
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} break;
		case '1': {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"Suid dumpable: %c (debug mode)",svalue);
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} break;
		case '2': {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"Suid dumpable: %c (suidsafe mode)",svalue);
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} break;
		default: break;
	}
	fclose(sd);
    return 0;
}

int SEC_MinAddr() {
 FILE *ma;
 char avalue[21],*buf;
    
	ma = fopen(MINADDR,"r");
	if (!ma) return (errno);
	fgets(avalue,20,ma);
	buf=(char *)malloc(MIDBLEN);
	memset(buf,0,MIDBLEN);
	snprintf(buf,MIDBLEN,"Minimal address map value: %d",atoi(avalue));
	OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
	free(buf);
	fclose(ma);
    return 0;
}

int SEC_AslrCheck() {
 FILE *rvs;
 char avalue[21],*buf;
    
	rvs = fopen(RANDOMIZEVA,"r");
	if (!rvs) return(errno);
	fgets(avalue,20,rvs);
    
	switch (atoi(avalue)){
		case 0: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"ASLR: %d (disabled)",atoi(avalue));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} break;;
		case 1: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"ASLR: %d (Conservative mode)",atoi(avalue));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		} break;
		case 2: {
			buf=(char *)malloc(MIDBLEN);
			memset(buf,0,MIDBLEN);
			snprintf(buf,MIDBLEN,"ASLR: %d (Full mode)",atoi(avalue));
			OutputHandler(OUTPUT_FORMAT,ITEM,"%s",buf);
			free(buf);
		}break;
		default: break;;
	}
	fclose(rvs);
    return 0;
}

int SEC_Selinux() {
 FILE *sd;
 char svalue[MAXBLEN],*end;
 int ret;
    
	sd = fopen(SELINUX,"r");
	if (!sd) ret=4;
	for(ret=0;;){
		end=fgets(svalue,MAXBLEN-1,sd);
		if (end==NULL) break;
		if(!MI_RegMatch(svalue,"SELINUX=disabled"))  {
			ret=3;
			break;
		}
		if(!MI_RegMatch(svalue,"SELINUX=permissive")) {
			ret=2;
			break;
		}
		if(!MI_RegMatch(svalue,"SELINUX=enforcing")) {
			ret=1;
			break;
		}
	}
	fclose(sd);
    return ret;
}

int SEC_CheckPatch() {
	if(!access(GRSECURITY, R_OK))
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","GrSecurty: Present");
	else
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","GrSecurty: Not Present");
    if(!access(APPARMOR, R_OK))
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","AppArmor: Present");
	else
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","AppArmor: Not Present");
	if(!access(PROCPAX, R_OK))
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Pax: Present");
	else
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Pax: Not Present");
	if(!access(SELINUX, R_OK)) {
		switch(SEC_Selinux()) {
			case 1: OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Selinux: Present (Enforcing Mode)"); break;
			case 2: OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Selinux: Present (Permissive Mode)"); break;
			case 3: OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Selinux: Present (Disabled)"); break;
		}
	} else
		OutputHandler(OUTPUT_FORMAT,ITEM,"%s","Selinux: Not Present");
    return 0;
}

void SEC_InterestingFiles() {
 int index;

	OutputHandler(OUTPUT_FORMAT,TITLE,"%s","Relevant files found on filesystem");
    LST_InitHeader();
	for (index=0;specialfiles[index];index++)
        SEC_FindFiles("/",S_FILES,specialfiles[index]);
    for (index=0;index<=header->len;index++)
        LST_ShowItem(index);
    LST_DestroyList();
}

