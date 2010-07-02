/*
 * $Id: vmgr_server.h,v 1.6 2009/04/06 12:37:19 waldron Exp $
 */

/*
 * Copyright (C) 1999-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
/*
 * @(#)$RCSfile: vmgr_server.h,v $ $Revision: 1.6 $ $Date: 2009/04/06 12:37:19 $ CERN IT-PDP/DM Jean-Philippe Baud
 */
 
#ifndef _VMGR_SERVER_H
#define _VMGR_SERVER_H
#ifdef USE_ORACLE
#ifdef USE_OCI
#include <ocidfn.h>
#ifdef __STDC__
#include <ociapr.h>
#else
#include <ocikpr.h>
#endif
#if defined(__alpha) && defined(__osf__)
#define HDA_SIZE 512
#else
#define HDA_SIZE 256
#endif
#endif
#else
#ifdef USE_MYSQL
#include <mysql.h>
#else
#ifdef USE_RAIMA
#include <rds.h>
#else
#include "Cdb_api.h"
#endif
#endif
#endif
#include "vmgr_struct.h"
#ifdef VMGRCSEC
#include "Csec_api.h"
#endif 
#include "vmgr_constants.h" 
#include "vmgr.h" 
 
                        /* volume manager constants and macros */

#define CHECKI	5	/* max interval to check for work to be done */
#define VMGR_NBTHREADS	6
#define VMGR_MAXNBTHREADS 100	/* maximum number of threads */

#define RETURN(x) { \
	vmgrlogit (func, "returns %d\n", (x)); \
	if (thip->dbfd.tr_started) { \
		if (x) { \
			(void) vmgr_abort_tr (&thip->dbfd); \
		} else { \
			(void) vmgr_end_tr (&thip->dbfd); \
		} \
	} \
	return ((x)); \
}

			/* volume manager tables and structures */

struct vmgr_dbfd {
	int		idx;		/* index in array of vmgr_dbfd */
#ifdef USE_ORACLE
#ifdef USE_OCI
	Lda_Def		lda;		/* logon data area */
	ub1		hda[HDA_SIZE];	/* host data area */
#endif
#else
#ifdef USE_MYSQL
        MYSQL           mysql;
#else
#ifdef USE_RAIMA
	RDM_SESS	hSess;		/* session handle */
	RDM_DB		hDb;		/* database handle */
#else
	Cdb_sess_t	Cdb_sess;
	Cdb_db_t	Cdb_db;
#endif
#endif
#endif
	int		tr_started;
	int             connected;
};

#ifdef USE_ORACLE
typedef char vmgr_dbrec_addr[19];
typedef int DBLISTPTR;
#else
#ifdef USE_MYSQL
typedef char vmgr_dbrec_addr[21];
typedef MYSQL_RES *DBLISTPTR;
#else
#ifdef USE_RAIMA
typedef DB_ADDR vmgr_dbrec_addr;
#else
typedef Cdb_off_t vmgr_dbrec_addr;
typedef Cdb_off_t DBLISTPTR;
#endif
#endif
#endif

struct vmgr_srv_thread_info {
	int		s;		/* socket for communication with client */
	struct vmgr_dbfd dbfd;
	char		errbuf[PRTBUFSZ];
#ifdef VMGRCSEC
        Csec_context_t sec_ctx;
        uid_t Csec_uid;
        gid_t Csec_gid;
        int Csec_service_type;    /* Type of the service if client is another Castor server */
#endif
};

struct vmgr_tape_side {
	char		vid[CA_MAXVIDLEN+1];
	int		side;
	char		poolname[CA_MAXPOOLNAMELEN+1];
	short		status;		/* TAPE_FULL, DISABLED, EXPORTED */
	int		estimated_free_space;	/* in kbytes */
	int		nbfiles;
};

struct vmgr_tape_tag {
	char		vid[CA_MAXVIDLEN+1];
	char		text[CA_MAXTAGLEN+1];
};

			/* volume manager function prototypes */

EXTERN_C int vmgrlogit (char *, char *, ...);
EXTERN_C int sendrep (int, int, ...);

EXTERN_C int vmgr_abort_tr (struct vmgr_dbfd *);
EXTERN_C int vmgr_closedb (struct vmgr_dbfd *);
EXTERN_C int vmgr_delete_denmap_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_dgnmap_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_library_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_model_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_pool_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_side_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_tag_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_delete_tape_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *);
EXTERN_C int vmgr_end_tr (struct vmgr_dbfd *);
EXTERN_C int vmgr_get_denmap_entry (struct vmgr_dbfd *, char *, char *, char *, struct vmgr_tape_denmap *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_dgnmap_entry (struct vmgr_dbfd *, char *, char *, struct vmgr_tape_dgnmap *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_library_entry (struct vmgr_dbfd *, char *, struct vmgr_tape_library *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_model_entry (struct vmgr_dbfd *, char *, struct vmgr_tape_media *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_pool_entry (struct vmgr_dbfd *, char *, struct vmgr_tape_pool *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_side_by_fullid (struct vmgr_dbfd *, char *, int, struct vmgr_tape_side *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_side_by_size (struct vmgr_dbfd *, char *, u_signed64, struct vmgr_tape_side *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_tag_by_vid (struct vmgr_dbfd *, char *, struct vmgr_tape_tag *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_get_tape_by_vid (struct vmgr_dbfd *, char *, struct vmgr_tape_info *, int, vmgr_dbrec_addr *);
EXTERN_C int vmgr_insert_denmap_entry (struct vmgr_dbfd *, struct vmgr_tape_denmap *);
EXTERN_C int vmgr_insert_dgnmap_entry (struct vmgr_dbfd *, struct vmgr_tape_dgnmap *);
EXTERN_C int vmgr_insert_library_entry (struct vmgr_dbfd *, struct vmgr_tape_library *);
EXTERN_C int vmgr_insert_model_entry (struct vmgr_dbfd *, struct vmgr_tape_media *);
EXTERN_C int vmgr_insert_pool_entry (struct vmgr_dbfd *, struct vmgr_tape_pool *);
EXTERN_C int vmgr_insert_side_entry (struct vmgr_dbfd *, struct vmgr_tape_side *);
EXTERN_C int vmgr_insert_tag_entry (struct vmgr_dbfd *, struct vmgr_tape_tag *);
EXTERN_C int vmgr_insert_tape_entry (struct vmgr_dbfd *, struct vmgr_tape_info *);
EXTERN_C int vmgr_list_denmap_entry (struct vmgr_dbfd *, int, struct vmgr_tape_denmap *, int, DBLISTPTR *);
EXTERN_C int vmgr_list_dgnmap_entry (struct vmgr_dbfd *, int, struct vmgr_tape_dgnmap *, int, DBLISTPTR *);
EXTERN_C int vmgr_list_library_entry (struct vmgr_dbfd *, int, struct vmgr_tape_library *, int, DBLISTPTR *);
EXTERN_C int vmgr_list_model_entry (struct vmgr_dbfd *, int, struct vmgr_tape_media *, int, DBLISTPTR *);
EXTERN_C int vmgr_list_pool_entry (struct vmgr_dbfd *, int, struct vmgr_tape_pool *, int, DBLISTPTR *);
EXTERN_C int vmgr_list_side_entry (struct vmgr_dbfd *, int, char *, char *, struct vmgr_tape_side *, int, DBLISTPTR *);
EXTERN_C int vmgr_opendb (struct vmgr_dbfd *);
EXTERN_C int vmgr_start_tr (int, struct vmgr_dbfd *);
EXTERN_C int vmgr_update_library_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_library *);
EXTERN_C int vmgr_update_model_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_media *);
EXTERN_C int vmgr_update_pool_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_pool *);
EXTERN_C int vmgr_update_side_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_side *);
EXTERN_C int vmgr_update_tag_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_tag *);
EXTERN_C int vmgr_update_tape_entry (struct vmgr_dbfd *, vmgr_dbrec_addr *, struct vmgr_tape_info *);

EXTERN_C int vmgr_srv_deletedenmap (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deletedgnmap (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deletelibrary (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deletemodel (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deletepool (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deletetape (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_deltag (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_enterdenmap (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_enterdgnmap (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_enterlibrary (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_entermodel (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_enterpool (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_entertape (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_gettag (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_gettape (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_listdenmap (int, char *, char *, struct vmgr_srv_thread_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_listdgnmap (char *, char *, struct vmgr_srv_thread_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_listlibrary (char *, char *, struct vmgr_srv_thread_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_listmodel (int, char *, char *, struct vmgr_srv_thread_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_listpool (char *, char *, struct vmgr_srv_thread_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_listtape (int, char *, char *, struct vmgr_srv_thread_info *, struct vmgr_tape_info *, int, DBLISTPTR *);
EXTERN_C int vmgr_srv_modifylibrary (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_modifymodel (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_modifypool (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_modifytape (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_querypool (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_querylibrary (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_querymodel (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_querytape (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_reclaim (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_settag (char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_shutdown (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_tpmounted (int, char *, char *, struct vmgr_srv_thread_info *);
EXTERN_C int vmgr_srv_updatetape (int, char *, char *, struct vmgr_srv_thread_info *);

#endif
