#ifndef CORE_MISC_H
#define CORE_MISC_H 1

#define PASSWDFILE "/etc/passwd"
#define GROUPFILE "/etc/group"

extern int MI_Owner(int uid,int gid,char *sfile);
extern int MI_RegMatch(char *string,char *regexpression);
extern int MI_CountTok(char *value,char delimiter);
extern char *MI_GetField(char *msg,int position,char delimiter);
extern char *MI_RmChar(char *value,char target);


#endif
