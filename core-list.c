#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "core-list.h"
#include "core-misc.h"
#include "core-main.h"


void LST_InitHeader() {
	header=(struct head *)malloc(sizeof(struct head));
	header->len=0;
	header->first=NULL;
	header->last=NULL;
}
void LST_ShowList(){
 struct node *a;

	a=header->first;
	while(a){
        MI_Owner(a->uid,a->gid,a->fullpath);
		a=a->next;
	}
}
void LST_ShowItem(int position){
 struct node *a;
    
	a=header->first;
	while(a!=NULL){
        if(a->fid==position)
            MI_Owner(a->uid,a->gid,a->fullpath);
		a=a->next;
	}
}
void LST_DestroyList(){
 struct node *a,*b;
	
	a=header->first;
	while(a!=NULL){
		b=a->next;
		header->first=b;
		header->len--;
		free(a);
		a=b;
	}
    if(header!=NULL)
        free(header);
}
int LST_InsertDataList(char *file,int uid,int gid,int fid) {
 
	if(header->len==0){
		list=(struct node *)malloc(sizeof(struct node));
		if (!list || !header)
			return(1);
        if(strlen(file)>MAXFPATH)
            strncpy(list->fullpath,"NAH",4);
        else
            strncpy(list->fullpath,file,MAXFPATH);
        list->uid=gid;
        list->gid=uid;
        list->fid=fid;
		list->prev=NULL;
		list->next=NULL;
		header->first=list;
		header->len=1;
		header->last=list;
	} else {
		aux=(struct node *)malloc(sizeof(struct node));
        if(strlen(file)>MAXFPATH)
            strncpy(aux->fullpath,"NAH",4);
        else
            strncpy(aux->fullpath,file,MAXFPATH);
        aux->uid=uid;
        aux->gid=gid;
        aux->fid=fid;
		aux->next = NULL;
		aux->prev = header->last;
		aux2=header->last;
		aux2->next=aux;
		header->last = aux;
		header->len++;
	}
    
 return 0;
}