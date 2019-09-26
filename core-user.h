#ifndef CORE_USER_H
#define CORE_USER_H 1

#define SHADOWFILE		"/etc/shadow"

struct udef {
	char *name;
	char *home;
	char *bash_history;
	char *mysql_history;
} userdef;

extern void USR_MainHandler();
extern int USR_GetUser();
extern int USR_BashHistory();
extern int USR_MyHist();
extern int USR_ShadFile();
extern void USR_FreeUserDefs();

#endif

