#ifndef CORE_MAIN_H
#define CORE_MAIN_H 1

#define BANNER	"\n[cpgather v1.0 - Information Gathering Tool]\n"

#define MAXFPATH		32000
#define MAXBHLINE		2500
#define MAXBLEN			500
#define MIDBLEN			256

typedef enum { false, true } boolean;

extern void usage (int argc, char *argv[]);
extern int main(int argc,char *argv[]);

#endif