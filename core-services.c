#include <stdio.h>
#include <stdlib.h>
#include "core-misc.h"
#include "core-services.h"
#include "core-main.h"
#include "core-output.h"

void SVC_MainHandler() {
    
    OutputHandler(OUTPUT_FORMAT,SECTION,"%s","Services Informations");

//	- Ssh configuration
//  - ftp configuration
//  - tftp configuration
//  - named configuration
//	- Snmp configuration
//	- Sudoers configuration
//	- Available cronjobs
        // - processes running (b64)
    
}
