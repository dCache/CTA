/*
 * $Id: stage.h,v 1.37 2001/02/01 12:43:04 jdurand Exp $
 */

/*
 * Copyright (C) 1993-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

			/* stage daemon constants and macros */

#ifndef __stage_h
#define __stage_h

/* ==================== */
/* Malloc Debug Library */
/* ==================== */
#ifdef USE_DMALLOC
#include <dmalloc.h>
#endif

#include "Castor_limits.h"
#include "Cns_api.h"
#include "osdep.h"
#include "serrno.h"     /* Contains ESTNACT etc... */

/* This macro returns TRUE is the file is an hpss one */
#define ISHPSS(xfile)   (strncmp (xfile, "/hpss/"  , 6) == 0 || strstr (xfile, ":/hpss/"  ) != NULL)

/* This macro returns TRUE is the host is an hpss one */
#define ISHPSSHOST(xfile) (strstr(xfile,"hpss") == xfile )

/* ISCASTORMIG macro returns TRUE if it is a CASTOR HSM file candidate for/beeing, migration/migrated */
/* ISCASTORBEINGMIG macro returns TRUE if it is a CASTOR HSM file beeing migrated */
/* ISCASTORWAITINGMIG macro returns TRUE if it is a CASTOR HSM file candidate for migration and waiting for a migration request itself */
/* ISCASTORCANBEMIG macro returns TRUE if it is a CASTOR HSM file candidate for migration */
/* ISCASTORFREE4MIG macro returns TRUE if it is a CASTOR/HPSS HSM file candidate for explicit migration */
/* ISCASTORWAITINGNS macro returns TRUE if it is a CASTOR HSM file waiting for creation in name server */
#define ISCASTORMIG(stcp) ((stcp->t_or_d == 'h') && ((stcp->status & PUT_FAILED) != PUT_FAILED) && ((stcp->status & CAN_BE_MIGR) == CAN_BE_MIGR))
#define ISCASTORBEINGMIG(stcp) (ISCASTORMIG(stcp) && (((stcp->status & BEING_MIGR) == BEING_MIGR) || (stcp->status == (STAGEPUT|CAN_BE_MIGR))))
#define ISCASTORWAITINGMIG(stcp) (ISCASTORMIG(stcp) && ((stcp->status & WAITING_MIGR) == WAITING_MIGR))
#define ISCASTORWAITINGNS(stcp) ((stcp->t_or_d == 'h') && ((stcp->status & WAITING_NS) == WAITING_NS))
#define ISCASTORCANBEMIG(stcp) (ISCASTORMIG(stcp) && (! (ISCASTORBEINGMIG(stcp) || ISCASTORWAITINGMIG(stcp))))
#define ISCASTORFREE4MIG(stcp) ((stcp->status == STAGEOUT) || (stcp->status == (STAGEOUT|PUT_FAILED)) || (stcp->status == (STAGEOUT|CAN_BE_MIGR)) || (stcp->status == (STAGEOUT|CAN_BE_MIGR|PUT_FAILED)) || (stcp->status == (STAGEWRT|CAN_BE_MIGR|PUT_FAILED)))
#define ISHPSSMIG(stcp) ((stcp->t_or_d == 'm') && ((stcp->status == STAGEPUT) || (stcp->status == STAGEWRT)))

/* This macro returns TRUE is the file is a castor one */
#ifdef NSROOT
#define ISCASTOR(xfile) (strncmp (xfile, NSROOT "/", strlen(NSROOT) + 1) == 0 || strstr (xfile, ":" NSROOT "/") != NULL)
#else
#define ISCASTOR(xfile) 0
#endif

/* This macro returns TRUE is the host is a castor one */
#if (defined(NSHOST) && defined(NSHOSTPFX))
#define ISCASTORHOST(xfile) (strstr(xfile,NSHOST) == xfile || strstr(xfile,NSHOSTPFX) == xfile)
#else
#ifdef NSHOST
#define ISCASTORHOST(xfile) (strstr(xfile,NSHOST) == xfile)
#else
#ifdef NSHOSTPFX
#define ISCASTORHOST(xfile) (strstr(xfile,NSHOSTPFX) == xfile)
#else
#define ISCASTORHOST(xfile) 0
#endif
#endif
#endif

/* This macro returns the hpss file pointer, without hostname */
#define HPSSFILE(xfile)   strstr(xfile,"/hpss/")

/* This macro returns the castor file pointer, without hostname */
#define CASTORFILE(xfile) strstr(xfile,"/castor/")

#define STGTIMEOUT 10   /* Stager network timeout (seconds) */
#define DEFDGN "CART"	/* default device group name */
#ifdef MAXPATH
#undef MAXPATH
#endif
#define MAXPATH 80	/* maximum path length */
#define	MAXRETRY 5
#define	MAXVSN 3	/* maximum number of vsns/vids on a stage command */
#define PRTBUFSZ 1024
#define REPBUFSZ  512	/* must be >= max stage daemon reply size */
#define REQBUFSZ 20000	/* must be >= max stage daemon request size */
#define CHECKI	10	/* max interval to check for work to be done */
#define	RETRYI	60
#define STGMAGIC    0x13140701
#define STG	"stage"	/* service name in /etc/services */

#define SHIFT_ESTNACT 198 /* Old SHIFT value when nomorestage - remapped in send2stgd */

#define UPPER(s) \
	{ \
	char * q; \
	for (q = s; *q; q++) \
		if (*q >= 'a' && *q <= 'z') *q = *q + ('A' - 'a'); \
	}

			/* stage daemon request types */

#define STAGEIN		1
#define STAGEOUT	2
#define STAGEWRT	3
#define STAGEPUT	4
#define STAGEQRY	5
#define STAGECLR	6
#define	STAGEKILL	7
#define	STAGEUPDC	8
#define	STAGEINIT	9
#define	STAGECAT	10
#define	STAGEALLOC	11
#define	STAGEGET	12
#define	STAGEMIGPOOL	13
#define STAGEFILCHG	14
#define STAGESHUTDOWN	15

			/* stage daemon reply types */

#define	MSG_OUT		0
#define	MSG_ERR		1
#define	RTCOPY_OUT	2
#define	STAGERC		3
#define	SYMLINK		4
#define	RMSYMLINK	5
#define API_STCP_OUT	6
#define API_STPP_OUT	7
#define UNIQUEID	8

			/* -C, -E and -T options */

#include "Ctape_constants.h"
#include "rtcp_constants.h"

			/* stage states */

#define	NOTSTAGED	  0
#define	WAITING_SPC	  0x10	/* waiting space */
#define	WAITING_REQ	  0x20	/* waiting on an other request */
#define	STAGED		  0x30	/* stage complete */
#define	KILLED		  0x40	/* stage killed */
#define	STG_FAILED	  0x50	/* stage failed */
#define	PUT_FAILED	  0x60	/* stageput failed */
#define	STAGED_LSZ	  0x100	/* stage limited by size */
#define	STAGED_TPE	  0x200	/* blocks with parity error have been skipped */
#define	CAN_BE_MIGR	  0x400	/* file can be migrated */
#define	LAST_TPFILE	  0x1000 /* last file on this tape */
#define	BEING_MIGR	  0x2000 /* file being migrated */
#define ISSTAGEDBUSY  0x4000  /* Internal error while scanning catalog : matched twice in catalog and the one to delete is busy */
#define ISSTAGEDSYERR 0x10000 /* Internal error while scanning catalog : matched twice in catalog and cannot delete one of them */
#define	WAITING_MIGR  0x20000 /* file being migrated and waiting confirmation (e.g. the real migration request) */
#define	WAITING_NS    0x40000 /* file waiting for an entry in the HSM name server */

			/* stage daemon messages */

#define STG00	"STG00 - stage daemon not available on %s\n"
#define STG01	"STG01 - no response from stage daemon\n"
#define STG02	"STG02 - %s : %s error : %s\n"
#define STG03   "STG03 - illegal function %d\n"
#define STG04   "STG04 - error reading request header, read = %d from %s (%s - %s)\n"
#define STG05	"STG05 - cannot get memory\n"
#define STG06	"STG06 - invalid value for option %s\n"
#define	STG07	"STG07 - pathname is mandatory\n"
#define STG08	"STG08 - incorrect pathname %s\n"
#define	STG09	"STG09 - fatal configuration error: %s %s\n"
#define	STG10	"STG10 - option G not allowed for user of group %s\n"
#define	STG11	"STG11 - invalid user: %s\n"
#define	STG12	"STG12 - vsn, vid or external_filename must be specified\n"
#define	STG13	"STG13 - duplicate option %s\n"
#define	STG14	"STG14 - too many vsns specified\n"
#define	STG15	"STG15 - parameter inconsistency with TMS: %s<->%s\n"
#define	STG16	"STG16 - extraneous parameter\n"
#define	STG17	"STG17 - option %s is invalid for %s\n"
#define	STG18	"STG18 - trailing dash in -q option is only valid for stagein\n"
#define	STG19	"STG19 - incorrect number of filenames specified\n"
#define	STG20	"STG20 - record length must be specified with recfm F\n"
#define	STG21	"STG21 - fseq string is too long\n"
#define	STG22	"STG22 - could not find corresponding stage request\n"
#define	STG23	"STG23 - %s is not accessible\n"
#define	STG24	"STG24 - pool %s is empty\n"
#define	STG25	"STG25 - %s name is missing\n"
#define	STG26	"STG26 - invalid description of %s %s\n"
#define	STG27	"STG27 - %s name %s is too long\n"
#define	STG28	"STG28 - missing or invalid value for DEFSIZE\n"
#define	STG29	"STG29 - no staging pool described\n"
#define	STG30	"STG30 - a default pool must be defined\n"
#define	STG31	"STG31 - staging host must be defined in configuration file\n"
#define	STG32	"STG32 - %s is not in the list of pools\n"
#define	STG33	"STG33 - %s: %s\n"
#define	STG34	"STG34 - program name must be stagein, stageout, stagewrt or stagecat\n"
#define	STG35	"STG35 - option(s) %s and %s are mutually exclusive\n"
#define	STG36	"STG36 - invalid group: %d\n"
#define	STG37	"STG37 - another request for same file is in progress\n"
#define	STG38	"STG38 - stage request too long\n"
#define	STG39	"STG39 - another stageinit is pending\n"
#define	STG40	"STG40 - another entry for same file exists already in catalog\n"
#define	STG41	"STG41 - %s %s for file %s on %s, return code %d\n\n"
#define	STG42	"STG42 - %s %s for file %s, return code %d\n\n"
#define	STG43	"STG43 - Retrying command, retry number %d\n\n"
#define	STG44	"STG44 - staging in afs directory is not supported\n"
#define	STG45	"STG45 - unable to allocate requested space\n"
#define	STG46	"STG46 - vid, linkname, internal or external filename must be specified\n"
#define	STG47	"STG47 - %s\n"
#define	STG48	"STG48 - the catalog %s seems to be corrupted\n"
#define	STG49	"STG49 - poolname mismatch: %s belongs to pool %s\n"
#define	STG50	"STG50 - poolname mismatch: %s does not belong to any pool\n"
#if defined(_WIN32)
#define	STG51	"STG51 - WSAStartup unsuccessful\n"
#define	STG52	"STG52 - you are not registered in the unix group/passwd mapping file\n"
#define	STG53	"STG53 - %s error %d\n"
#endif
#define	STG54	"STG54 - HSM hostname not specified\n"
#define	STG55	"STG55 - migrator name %s not defined\n"
#define	STG56	"STG56 - migration policy of migrator %s is not defined\n"
#define	STG57	"STG57 - Fileclass %s (classid %d) on name server %s (internal index %d) error : %s : %s\n"
#define	STG58	"STG58 - another stageshutdown is pending\n"
#define	STG59	"STG59 - Duplicate HSM file %s\n"
#define	STG60	"STG60 - Duplicated file sequence %s reduced by one\n"
#if defined(vms)
#define	STG80	"STG80 - invalid GRPUSER entry (username missing) : %s\n"
#define	STG81	"STG81 - invalid GRPUSER entry (uid missing) : %s\n"
#define	STG82	"STG82 - invalid GRPUSER entry (gid missing) : %s\n"
#define	STG83	"STG83 - -G parameter is mandatory for VMS users\n"
#endif
#define	STG92	"STG92 - %s request by %s (%d,%d) from %s\n"
#define	STG93	"STG93 - removing link %s\n"
#define	STG94	"STG94 - creating link %s\n"
#define	STG95	"STG95 - %s cleared by %s\n"
#define	STG96	"STG96 - %s already staged, size = %ld (%.1f MB), nbaccess = %d\n"
#define	STG97	"STG97 - %s:%s staged by (%s,%s), server %s  unit %s  ifce %s  size %s  wtim %d  ttim %d rc %d\n"
#define	STG98	"STG98 - %s\n"
#define	STG99	"STG99 - stage returns %d\n"
#define STG100  "STG100 - Database %s error (%s) at %s:%d\n"
#define STG101  "STG101 - HSM File %s previously staged under name %s. Catalog updated.\n"
#define STG102  "STG102 - Mixed %s HSM host with %s HSM filename: %s\n"
#define STG103  "STG103 - -F option is only for admin\n"
#define STG104  "STG104 - Internal error: status=0x%x but req not in waitq - Ask admin to try with -F option\n"
#define STG105  "STG105 - Internal error in %s : %s\n"
#define STG106  "STG106 - Internal error in %s for %s: %s\n"
#define	STG107	"STG107 - %s:%s segment %d staged by (%s,%s), server %s  unit %s  ifce %s  size %s  wtim %d  ttim %d rc %d\n"
#define	STG108	"STG108 - %s:%s staged using %d segments by (%s,%s), size %s rc %d\n"
#define	STG109	"STG109 - New fileclass %s@%s (classid %d), internal index %d, tppools=%s\n"
#define STG110  "STG110 - Internal error in %s for pool %s, class %s@%s: %s\n"
#define STG111  "STG111 - Last used tape pool \"%s\" unknown to fileclass %s@%s (classid %d)\n"
#define STG112  "STG112 - %s already have %d copies (its current fileclass specifies %d cop%s). Not migrated.\n"
#define STG113  "STG113 - Cannot find next tape pool - use the first in the list\n"
#define STG114  "STG114 - Found more files to migrate (%d) that what is known in advance (%d)\n"
#define STG115  "STG115 - Reqid %d (%s) have no tape pool associated yet\n"
#define STG116  "STG116 - Not all input records have a tape pool\n"
#define STG117  "STG117 - Inputs No %d (%s) and %d (%s) differs vs. their tape pool (\"%s\" and \"%s\")\n"
#define STG118  "STG118 - Warning, fileclasses %s and %s shared tape pool \"%s\" but have different %s values (%d and %d)\n"
#define STG119  "STG119 - Missing file path argument for HSM files %s\n"
#define STG120  "STG120 - %s already has a tppool assigned (\"%s\") - not expanded with respect to its class %s@%s (classid %d)\n"
#define STG121  "STG121 - No tape pool\n"
#define STG122  "STG122 - Bad tape pool \"%s\"\n"
#define STG123  "STG123 - (Warning) tape %s mounted but no segment writen at expected fseq %d, tape flagged %s\n"
#define STG124  "STG124 - Shutdown\n"
#define STG125  "STG125 - Your account \"%s\" (%s=%d) does not have the correct %s id (should be %d) to migrate %s\n"
#define STG126  "STG126 - Fileclass %s@%s (classid %d) have %d number of copies - Please contact your admin - rejected\n"
#define STG127  "STG127 - Fileclass %s@%s (classid %d) claims %d tape pools while we found %d of them - Please contact your admin - updated\n"
#define STG128  "STG128 - Fileclass %s@%s (classid %d) have duplicated tape pool %s - Please contact your admin  - duplicate removed\n"
#define STG129  "STG129 - Fileclass %s@%s (classid %d) have its number of tape pools finally reduced from %d to %d\n"
#define STG130  "STG130 - Fileclass %s@%s (classid %d) have %d number of copies v.s. %d tppools - copies possible reduced to %d\n"
#define STG131  "STG131 - %s not removed - Retention period %d > %d seconds lifetime\n"
#define STG132  "STG132 - %s fileclass : %s\n"
#define STG133  "STG133 - %s : Fileclass %s@%s (classid %d) specified retention period %d > %d seconds lifetime\n"
#define STG134  "STG134 - Tape %s is not accessible (%s status)\n"

			/* stage daemon return codes */

#define	USERR	  1	/* user error */
#define	SYERR 	  2	/* system error */
#define	UNERR	  3	/* undefined error */
#define	CONFERR	  4	/* configuration error */
#define	LNKNSUP	189	/* symbolic links not supported on that platform */
#define	CLEARED	192	/* aborted by stageclr */
#define	BLKSKPD	193	/* blocks were skipped */
#define	TPE_LSZ	194	/* blocks were skipped, stageing limited by size */
#define	MNYPARI	195	/* stagein stopped: too many tape errors, but -E keep */
#define	REQKILD	196	/* request killed by user */
#define	LIMBYSZ	197	/* limited by size */
#define	ENOUGHF	199	/* enough free space */

			/* stage daemon internal tables */

typedef char fseq_elem[7];
#if defined(_WIN32)
typedef long gid_t;
typedef long uid_t;
#endif

#if ! defined(vms)
struct stgcat_entry {		/* entry format in STGCAT table */
	int	blksize;	/* maximum block size */
	char	filler[2];
	char	charconv;	/* character conversion */
	char	keep;		/* keep data on disk after successful stagewrt */
	int	lrecl;		/* record length */
	int	nread;		/* number of blocks/records to be copied */
	char	poolname[CA_MAXPOOLNAMELEN+1];
	char	recfm[CA_MAXRECFMLEN+1];	/* record format */
	int	size;		/* size in Mbytes of data to be staged */
	char	ipath[(CA_MAXHOSTNAMELEN+MAXPATH)+1];	/* internal path */
	char	t_or_d;		/* 't' for tape/disk, 'd' for disk/disk , 'm' for non-CASTOR HSM, 'h' for CASTOR HSM */
	char	group[CA_MAXGRPNAMELEN+1];
	char	user[CA_MAXUSRNAMELEN+1];	/* login name */
	uid_t	uid;		/* uid or Guid */
	gid_t	gid;
#if defined(vms) || defined(_WIN32)
	int mask;
#else
	mode_t	mask;
#endif
	int	reqid;
	int	status;
	off_t	actual_size;
	time_t	c_time;
	time_t	a_time;
	int	nbaccesses;
	union {
	    struct {			/* tape specific info */
		char	den[CA_MAXDENLEN+1];	/* density */
		char	dgn[CA_MAXDGNLEN+1];	/* device group */
		char	fid[CA_MAXFIDLEN+1];	/* file id */
		char	filstat;	/* file status: new = 'n', old = 'o' */
		char	fseq[CA_MAXFSEQLEN+1];	/* file sequence number requested by user */
		char	lbl[CA_MAXLBLTYPLEN+1];	/* label type: al, nl, sl or blp */
		int	retentd;	/* retention period in days */
		char	tapesrvr[CA_MAXHOSTNAMELEN+1];	/* tape server */
		char	E_Tflags;	/* SKIPBAD, KEEPFILE, NOTRLCHK */
		char	vid[MAXVSN][CA_MAXVIDLEN+1];
		char	vsn[MAXVSN][CA_MAXVSNLEN+1];
	    } t;
	    struct {			/* info for disk file stageing */
		char	xfile[(CA_MAXHOSTNAMELEN+MAXPATH)+1];
		char	Xparm[23];
	    } d;
      struct {			/* Migrated files (non-CASTOR) */
		char	xfile[167];
	    } m;
	    struct {			/* HSM files (CASTOR) */
		char	xfile[167];
		char		server[CA_MAXHOSTNAMELEN+1];
		u_signed64	fileid;
		short	fileclass;
		char tppool[CA_MAXPOOLNAMELEN+1];
	    } h;
	} u1;
};

struct stgpath_entry {		/* entry format in STGPATH table */
	int	reqid;
	char	upath[(CA_MAXHOSTNAMELEN+MAXPATH)+1];
};

struct waitf {
	int	subreqid;
	int	waiting_on_req;
	u_signed64 size_to_recall;             /* Only used in read-mode (asyncrhoneous callbacks from RTCOPY) */
	int nb_segments;
	u_signed64 size_yet_recalled;
	char	upath[(CA_MAXHOSTNAMELEN+MAXPATH)+1];
};

struct waitq {
	struct waitq *prev;
	struct waitq *next;
	char	pool_user[CA_MAXUSRNAMELEN+1];
	char	clienthost[CA_MAXHOSTNAMELEN+1];
	char	req_user[CA_MAXUSRNAMELEN+1];	/* requestor login name */
	uid_t	req_uid;		/* uid or Guid */
	gid_t	req_gid;
	char	rtcp_user[CA_MAXUSRNAMELEN+1];	/* running login name */
	char	rtcp_group[CA_MAXGRPNAMELEN+1];	/* running group name */
	uid_t	rtcp_uid;		/* uid or Guid */
	gid_t	rtcp_gid;
	int	clientpid;
	u_signed64 uniqueid; /* Unique IDentifier when called from the API */
	int	copytape;
	int	Pflag;		/* stagealloc -P option */
	int	Upluspath;
	int	reqid;
	int	req_type;
	int	key;
	int	rpfd;
	int	ovl_pid;
	int	nb_subreqs;
	int	nbdskf;	/* >= nb_subreqs, because stagewrt may concatenate */
	int	nb_waiting_on_req;
	struct waitf *wf;
	int	nb_clnreq;
	char	waiting_pool[CA_MAXPOOLNAMELEN+1];	/* if waiting space */
	int	clnreq_reqid;		/* disk full reported by rtcopy */
	int	clnreq_rpfd;
	int	status;
	int	nretry;
	int	Aflag; /* Deferred allocation (path returned to RTCOPY after tape position) */
	int	concat_off_fseq; /* 0 or fseq just before the '-', like: 1-9,11- => concat_off_fseq = 11 */
	int	api_out; /* Flag to tell if we have to send structure in output (API mode) */
#if defined(vms) || defined(_WIN32)
	int openmode;  /* Used only to remember the openmode in entries in STAGEOUT|WAITING_NS state */
#else
	mode_t	openmode;  /* Used only to remember the openmode in entries in STAGEOUT|WAITING_NS state */
#endif
	int openflags;  /* Used only to remember the openflags in entries in STAGEOUT|WAITING_NS state */
	int	silent; /* Determine if done in silent mode or not */
	int use_subreqid; /* Says if we allow RTCOPY to do asynchroneous callback */
	int *save_subreqid; /* Array saying relation between subreqid and all wf at the beginning */
	int save_nbsubreqid; /* Save original number of entries */
};

struct pool {
	char	name[CA_MAXPOOLNAMELEN+1];
	struct pool_element *elemp;
	int	defsize;
	int	gc_start_thresh;
	int	gc_stop_thresh;
	char	gc[CA_MAXHOSTNAMELEN+MAXPATH+1];	/* garbage collector */
	int	no_file_creation;	/* Do not create empty file on stagein/out (for Objectivity DB) */
	int	nbelem;
	int	next_pool_elem;		/* next pool element to be used */
	u_signed64	capacity;	/* Capacity in bytes */
	u_signed64	free;		/* Free space in bytes */
	int	ovl_pid;
	time_t	cleanreqtime;
	int	cleanstatus;	/* 0 = normal, 1 = just cleaned */
	char	migr_name[CA_MAXMIGRNAMELEN+1];
	struct migrator *migr;
	int	mig_start_thresh;
	int	mig_stop_thresh;
	u_signed64 mig_data_thresh;
};

struct predicates {
  int nbfiles_canbemig;
  u_signed64 space_canbemig;
  int nbfiles_beingmig;
  u_signed64 space_beingmig;
};

struct fileclass {
	char server[CA_MAXHOSTNAMELEN+1];
	struct Cns_fileclass Cnsfileclass;
	char last_tppool_used[CA_MAXPOOLNAMELEN+1];
	int  flag;                                      /* Flag preventing us to double count next member */
	int  streams;                                   /* Number of streams using this fileclass while migrating */
	int being_migr;                                 /* Number of files being migrated in this fileclass */
};

struct migrator {
	char name[CA_MAXMIGRNAMELEN+1];
	int mig_pid;
	time_t migreqtime;
	time_t migreqtime_last_end;
	int nfileclass;
	struct fileclass **fileclass;
	struct predicates *fileclass_predicates;
	struct predicates global_predicates;
};

struct pool_element {
	char	server[CA_MAXHOSTNAMELEN+1];
	char	dirpath[MAXPATH];
	u_signed64	capacity;	/* filesystem capacity in bytes */
	u_signed64	free;		/* free space in bytes */
};

struct sorted_ent {
	struct sorted_ent *next;
	struct sorted_ent *prev;
	struct stgcat_entry *stcp;
	struct stgpath_entry *stpp;
	int scanned;            /* Flag telling if we yet scanned this entry */
	double	weight;
};
#endif

#endif /* __stage_h */
