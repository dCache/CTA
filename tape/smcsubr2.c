/*
 * Copyright (C) 1998-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
#ifndef lint
static char sccsid[] = "@(#)$RCSfile: smcsubr2.c,v $ $Revision: 1.6 $ $Date: 2006/12/13 12:59:25 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "Ctape.h"
#include "serrno.h"
#include "smc.h"

smc_dismount (fd, loader, robot_info, drvord, vid)
int fd;
char *loader;
struct robot_info *robot_info;
int drvord;
char *vid;
{
    int c;
    struct smc_element_info element_info;
	char func[16];
	char *msgaddr;
	struct smc_status smc_status;
 
	ENTRY (smc_dismount);
	if ((c = smc_read_elem_status (fd, loader, 4, robot_info->device_start+drvord,
	    1, &element_info)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR020, "read_elem_status", msgaddr);
		RETURN (c);
	}
	if ((element_info.state & 0x1) == 0) {
		usrmsg (func, SR018, "demount", vid, drvord, "Medium Not Present");
		RETURN (RBT_OK);
	}
	if ((element_info.state & 0x8) == 0) {
		usrmsg (func, SR018, "demount", vid, drvord, "Drive Not Unloaded");
		RETURN (RBT_UNLD_DMNT);
	}
	if (*vid && strcmp (element_info.name, vid)) {
		usrmsg (func, SR009, vid, element_info.name);
		RETURN (RBT_NORETRY);
	}
	if ((c = smc_move_medium (fd, loader, robot_info->device_start+drvord,
	    element_info.source_address, (element_info.flags & 0x40) ? 1 : 0)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR018, "demount", vid, drvord, msgaddr);
		RETURN (c);
	}
    /* check that the vid is in a slot before returning */
    while (1) {   
          if ((c = smc_find_cartridge (fd, loader, vid, 0, 0, 1, &element_info)) < 0) {
              c = smc_lasterror (&smc_status, &msgaddr);
              usrmsg (func, SR017, "find_cartridge", vid, msgaddr);
              RETURN (c);
          }
         
          /* vid is in a storage slot */  
          if (element_info.element_type == 2) break; 
          /* give time for the tape enter the slot */
          sleep (2);
    }

	RETURN (0);
}

smc_export (fd, loader, robot_info, vid)
int fd;
char *loader;
struct robot_info *robot_info;
char *vid;
{
        int c;
        struct smc_element_info element_info;
	char func[16];
	int i;
        struct smc_element_info *impexp_info;
	char *msgaddr;
	int nbelem;
	struct smc_status smc_status;
 
	ENTRY (smc_export);
	if ((c = smc_find_cartridge (fd, loader, vid, 0, 0, 1, &element_info)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR017, "find_cartridge", vid, msgaddr);
		RETURN (c);
	}
	if (c == 0) {
		usrmsg (func, SR017, "export", vid, "volume not in library");
		RETURN (RBT_NORETRY);
	}
	if (element_info.element_type != 2) {
		usrmsg (func, SR017, "export", vid, "volume in use");
		RETURN (RBT_SLOW_RETRY);
	}
	/* look for a free export slot */

	nbelem = robot_info->port_count;
	if ((impexp_info = malloc (nbelem * sizeof(struct smc_element_info))) == NULL) {
		usrmsg (func, SR012);
		RETURN (RBT_NORETRY);
	}

	if ((c = smc_read_elem_status (fd, loader, 3, robot_info->port_start,
	    nbelem, impexp_info)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR020, "read_elem_status", msgaddr);
		free (impexp_info);
		RETURN (c);
	}
	for (i = 0; i < nbelem; i++) {
		if (((impexp_info+i)->state & 0x1) == 0)	/* element free */
			break;
	}
	if (i >= nbelem) {	/* export slots are full */
		usrmsg (func, SR013);
		free (impexp_info);
		RETURN (RBT_NORETRY);
	}

	if ((c = smc_move_medium (fd, loader, element_info.element_address,
	    (impexp_info+i)->element_address, 0)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR017, "export", vid, msgaddr);
		free (impexp_info);
		RETURN (c);
	}
	free (impexp_info);
	RETURN (0);
}

smc_import (fd, loader, robot_info, vid)
int fd;
char *loader;
struct robot_info *robot_info;
char *vid;
{
        int c;
	int device_start;
        struct smc_element_info *element_info;
	char func[16];
	int i, j;
	char *msgaddr;
	int nbelem;
	int port_start;
	int slot_start;
	struct smc_status smc_status;
 
	ENTRY (smc_import);
	nbelem = robot_info->transport_count + robot_info->slot_count +
		 robot_info->port_count + robot_info->device_count;
	if ((element_info = malloc (nbelem * sizeof(struct smc_element_info))) == NULL) {
		usrmsg (func, SR012);
		RETURN (RBT_NORETRY);
	}

	/* get inventory */

	if ((c = smc_read_elem_status (fd, loader, 0, 0, nbelem, element_info)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR020, "read_elem_status", msgaddr);
		free (element_info);
		RETURN (c);
	}
	for (i = 0; i < c; i++)
		if ((element_info+i)->element_type == 2) break;
	slot_start = i;
	for (i = 0; i < c; i++)
		if ((element_info+i)->element_type == 3) break;
	port_start = i;
	for (i = 0; i < c; i++)
		if ((element_info+i)->element_type == 4) break;
	device_start = i;

	/* mark home slots of cartridges currently on drives as non free */

	for (i = device_start; i < device_start+robot_info->device_count; i++) {
		if (((element_info+i)->state & 0x1) == 0) continue;
		for (j = slot_start; j < slot_start+robot_info->slot_count; j++)
			if ((element_info+i)->source_address ==
				(element_info+j)->element_address) break;
		(element_info+j)->state |= 1;
	}

	/* loop on all import slots */

	for (i = port_start; i < port_start+robot_info->port_count; i++) {
		if (*vid && strcmp (vid, (element_info+i)->name)) continue;
		if (*vid || (*vid == '\0' && ((element_info+i)->state & 2))) {

			/* find a free storage slot */

			for (j = slot_start; j < slot_start+robot_info->slot_count; j++)
				if (((element_info+j)->state & 0x1) == 0) break;
			if (j >= slot_start+robot_info->slot_count) {
				usrmsg (func, SR015);
				free (element_info);
				RETURN (RBT_NORETRY);
			}

			if ((c = smc_move_medium (fd, loader, (element_info+i)->element_address,
			    (element_info+j)->element_address, 0)) < 0) {
				c = smc_lasterror (&smc_status, &msgaddr);
				usrmsg (func, SR017, "import",
				    (element_info+i)->name, msgaddr);
				free (element_info);
				RETURN (c);
			}
			if (*vid || c) break;
			(element_info+j)->state |= 1;	/* dest slot is now full */
		}
	}
	free (element_info);
	RETURN (c);
}

smc_mount (fd, loader, robot_info, drvord, vid, invert)
int fd;
char *loader;
struct robot_info *robot_info;
int drvord;
char *vid;
int invert;
{
    int c;
    struct smc_element_info element_info;
	char func[16];
	char *msgaddr;
	struct smc_status smc_status;
 
	ENTRY (smc_mount);
	if ((c = smc_find_cartridge (fd, loader, vid, 0, 0, 1, &element_info)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR017, "find_cartridge", vid, msgaddr);
		RETURN (c);
	}
	if (c == 0) {
		usrmsg (func, SR018, "mount", vid, drvord, "volume not in library");
		RETURN (RBT_OMSG_NORTRY);
	}
	if (element_info.element_type != 2) {
		usrmsg (func, SR018, "mount", vid, drvord, "volume in use");
		RETURN (RBT_OMSG_SLOW_R);
	}
	if ((c = smc_move_medium (fd, loader, element_info.element_address,
	    robot_info->device_start+drvord, invert)) < 0) {
		c = smc_lasterror (&smc_status, &msgaddr);
		usrmsg (func, SR018, "mount", vid, drvord, msgaddr);
		RETURN (c);
	}
	RETURN (0);
}
