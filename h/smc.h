/*
 * $Id: smc.h,v 1.5 2002/07/24 07:18:43 baud Exp $
 */

/*
 * Copyright (C) 1998-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*
 * @(#)$RCSfile: smc.h,v $ $Revision: 1.5 $ $Date: 2002/07/24 07:18:43 $ CERN IT-PDP/DM   Jean-Philippe Baud
 */

struct robot_info {
	char inquiry[32];
	int transport_start;
	int transport_count;
	int slot_start;
	int slot_count;
	int port_start;
	int port_count;
	int device_start;
	int device_count;
};

struct smc_element_info {
	int element_address;
	int element_type;
	int state;
	unsigned char asc;
	unsigned char ascq;
	int flags;
	int source_address;
	char name[9];
};

struct smc_status {
	unsigned char asc;
	unsigned char ascq;
	int save_errno;
	int rc;		/* return code from send_scsi_cmd */
	unsigned char sensekey;
	int skvalid;	/* sense key is valid */
};
