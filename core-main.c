#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "core-main.h"
#include "core-security.h"
#include "core-services.h"
#include "core-misc.h"
#include "core-environ.h"
#include "core-user.h"
#include "core-output.h"
#include "core-network.h"
#include "core-plugins.h"

int main(int argc, char *argv[]) {
 int opc;
 boolean opt_e,opt_s,opt_n,opt_d,opt_u,opt_p,opt_x;

    printf("%s\n",BANNER);
	if(argc < 2) usage(argc,argv);
	while ((opc = getopt (argc, argv, "esndupxa")) != -1) {
		switch (opc) {
			case 'a': {
                opt_e=true;
                opt_s=true;
                opt_n=true;
                opt_d=true;
                opt_u=true;
                opt_p=true;
			} break;
			case 'e': opt_e=true;break; //environment
			case 's': opt_s=true;break; //security
			case 'n': opt_n=true;break; //network
			case 'd': opt_d=true;break; //services
			case 'u': opt_u=true;break; //user
			case 'p': opt_p=true;break; //load plugins
			case 'x': opt_x=true;break; //xml output
			case '?': OutputHandler(STD,ERROR,"%s","Missing Argument");exit (1);
			default:usage(argc,argv);break;
		}
	}
    
    if(opt_e==true)
        ENV_MainHandler();
    if(opt_s==true)
        SEC_MainHandler();
    if(opt_n==true)
        NET_MainHandler();
    if(opt_d==true)
        SVC_MainHandler();
    if(opt_u==true)
        USR_MainHandler();
    if(opt_p==true)
        PLG_MainHandler();

    if(opt_x==true)
        OUTPUT_FORMAT=XML;
    else
        OUTPUT_FORMAT=STD;
 return 0;
}

void usage (int argc, char *argv[]){
        printf("Usage: %s <options>\n", argv[0]); 
		printf("Available options:\n");
		printf("  -e\t    \t\tRetrieve System Environment\n");
		printf("  -s\t    \t\tRetrieve Security Informations\n");
		printf("  -n\t    \t\tRetrieve Network Informations\n");
		printf("  -d\t    \t\tRetrieve Services Informations\n");
		printf("  -u\t    \t\tRetrieve User Environment\n");
        printf("  -m\t    \t\tRetrieve Active Kernel Modules\n");
        printf("  -i\t    \t\tRetrieve Installed Packages\n");
		printf("  -a\t    \t\tRetrieve all informations above\n");
        printf("  -x\t    \t\tPrint output as XML\n");
        exit (1);
}
