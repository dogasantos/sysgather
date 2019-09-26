#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "core-output.h"
/*
* Disclaimer: Avoid to use tird party libraries to maintain the portable behavior
*           this tool was designed to be used by pentesters on a post-shell AND
*           by system administrators who wish to retrieve relevant informations
*           about managed systems.
*           So i let libxml and others amazing libraries outside of this project.
*           If you wish to use them, you can submit a tiket on whislist, i'll be 
*           glad to hear you and write some codes for you. If you write some module
*           for cpgather and wish to share, we can include on future official releases.
*/

/*
 * Function: WriteGrp
 * Arguments: the type of data (section,title or item) and the data to be written.
 * Return: Well, for now its always 0, but to maintain a comcept, i wrote the return handlers to future decisions.
 * Description:
 *           Write the data in grepable format, a nice decision to be used on scripts or some network monitor tool
 *           like nagios, zabbix or similars.
 *           Also, the output should be easy parseable with standard linux tools like awk, sed, cut and similars.
 */
int WriteGrp(int st_type,char *st_fmt,char *st_data){

    switch(st_type) {
        case SECTION: printf("[*] %s\n",st_data);break;
        case TITLE: printf("  + %s\n",st_data);break;
        case ITEM: printf("    - %s\n",st_data);break;
        default: break;
    }
    
 return 0;
}

/*
 * Function: WriteXml
 * Arguments: the type of data (section,title or item) and the data to be written.
 * Return: Well, for now its always 0, but to maintain a comcept, i wrote the return handlers to future decisions.
 * Description:
 *           Write the data in XML format, a nice decision to be combined with custom softwares to database import.
 *           Also, the output should be easy parseable with standard linux tools like awk, sed, cut and similars.
 */

int WriteXml(int st_type,char *st_fmt,char *st_data){
    
    switch(st_type) {
        case SECTION: printf("[*] %s\n",st_data);break;
        case TITLE: printf("  + %s\n",st_data);break;
        case ITEM: printf("    - %s\n",st_data);break;
        default: break; 
    }
    
 return 0;
}

/*
 * Function: WriteStd
 * Arguments: the type of data (section,title or item) and the data to be written.
 * Return: Well, for now its always 0, but to maintain a comcept, i wrote the return handlers to future decisions.
 * Description:
 *           Write the data in standard format, its good enough to be viewed on terminal as a quick use.
 */

int WriteStd(int st_type,char *st_fmt,char *st_data){
 char *buf;
 int size;

	switch(st_type) {
        case SECTION: {
			size=strlen(st_fmt)+strlen(st_data);
			buf=(char *)malloc(size);
			memset(buf,0,size);
			snprintf(buf,size,st_fmt,st_data);
			printf("[*] %s\n",buf);
			free(buf);
		} break;
        case TITLE: {
			size=strlen(st_fmt)+strlen(st_data);
			buf=(char *)malloc(size);
			memset(buf,0,size);
			snprintf(buf,size,st_fmt,st_data);
			printf("  + %s\n",buf);
			free(buf);
		} break;
        case ITEM: {
			size=strlen(st_fmt)+strlen(st_data);
			buf=(char *)malloc(size);
			memset(buf,0,size);
			snprintf(buf,size,st_fmt,st_data);
			printf("    - %s\n",buf);
			free(buf);
		} break;
        case ERROR: {
			size=strlen(st_fmt)+strlen(st_data);
			buf=(char *)malloc(size);
			memset(buf,0,size);
			snprintf(buf,size,st_fmt,st_data);
			printf("    x %s\n",buf);
			free(buf);
		} break;
        default: break;
    }
    
 return 0;
}

/*
* Function: OutputHandler
* Arguments: a void value to be "casted" to struct odata for now.
* Return: Well, for now its always 0, but to maintain a comcept, i wrote the return handlers to future decisions.
* Description: 
*           Designed to be a unique point of interaction to the whole project, so is easy to
*           implement new modules or rewrite some function.
*           All output request present on cpgather should call this function, and here we redirect to the proper function
*           as desired. The children functions should not call functions on the main project to maintain the "plugin" comcept.
*           The argument should be a void for the same idea, keep portable and easy.
*/

int OutputHandler(int st_format,int st_type,char *st_fmt,char *st_data){
 int ret=0;
  
    switch(st_format) {
        case STD: ret=WriteStd(st_type,st_fmt,st_data);break;
        case XML: ret=WriteXml(st_type,st_fmt,st_data);break;
        case GRP: ret=WriteGrp(st_type,st_fmt,st_data);break;
    }
 return ret;
}




