#ifndef CORE_LIST_H
#define CORE_LIST_H 1
#include "core-main.h"

struct node {
    char fullpath[MAXFPATH];
    int fid;
    int uid;
    int gid;
    struct node *next;
    struct node *prev;
} *list, *aux, *aux2;

struct head {
    int len;
    struct node *first;
    struct node *last;
} *header;

extern void LST_InitHeader();
extern void LST_ShowList();
extern void LST_ShowItem(int position);
extern void LST_DestroyList();
extern int LST_InsertDataList(char *file,int uid,int gid,int fid);

#endif