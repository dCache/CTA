/*
	$Id: check_Cdbentry.c,v 1.15 2002/01/24 10:42:41 jdurand Exp $
*/

#include "Cstage_db.h"

/* ============== */
/* System headers */
/* ============== */
#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* =============================================================== */
/* Local headers for threads : to be included before ANYTHING else */
/* =============================================================== */
#include "osdep.h"
#include "Cdb_api.h"                /* CASTOR Cdb Interface */
#include "Cstage_db.h"              /* Generated STAGE/Cdb header */
#include "stage_struct.h"           /* CASTOR's STAGE structures */
#include "serrno.h"                 /* CASTOR's serrno */
#include "Cstage_ifce.h"            /* Conversion routines */
#include "u64subr.h"
#include "Cgetopt.h"
#include "stage_api.h"

/* ====== */
/* Macros */
/* ====== */
#define CDB_USERNAME "Cdb_Stage_User"
#define CDB_PASSWORD "Cdb_Stage_Password"

#ifdef __STDC__
#define NAMEOFVAR(x) #x
#else
#define NAMEOFVAR(x) "x"
#endif

#define DUMP_VAL(st,member) {					\
	printf("%-23s : %10d\n", NAMEOFVAR(member) ,	\
				 (int) st->member);				\
}

#define DUMP_VALHEX(st,member) {					\
	printf("%-23s : %10lx (hex)\n", NAMEOFVAR(member) ,	\
				 (unsigned long) st->member);				\
}

#define DUMP_U64(st,member) {					\
    char tmpbuf[21];                            \
	printf("%-23s : %10s\n", NAMEOFVAR(member) ,	\
				 u64tostr((u_signed64) st->member, tmpbuf,0));	\
}

#define DUMP_CHAR(st,member) {										\
	printf("%-23s : %10c\n", NAMEOFVAR(member) ,						\
				 st->member != '\0' ? st->member : ' ');			\
}

#define DUMP_STRING(st,member) {				\
	printf("%-23s : %10s\n", NAMEOFVAR(member) ,	\
				 st->member);					\
}


/* =================== */
/* Internal prototypes */
/* =================== */
void check_Cdbentry_usage _PROTO(());

/* ================ */
/* Static variables */
/* ================ */
int reqid;

int main(argc,argv)
		 int argc;
		 char **argv;
{
	/* For options */
	int errflg = 0;
	int c;
	int help = 0;
	char *Cdb_username = NULL;
	char *Cdb_password = NULL;
	Cdb_db_t Cdb_db;
	Cdb_sess_t Cdb_session;
	int Cdb_session_opened = 0;
	int Cdb_db_opened = 0;
	char *db[] = {
		"stgcat_tape",
		"stgcat_disk",
		"stgcat_hpss",
		"stgcat_hsm",
		"stgcat_alloc",
		"stgcat_link",
	};
	char *key[] = {
		"stgcat_tape_per_reqid",
		"stgcat_disk_per_reqid",
		"stgcat_hpss_per_reqid",
		"stgcat_hsm_per_reqid",
		"stgcat_alloc_per_reqid",
		"stgcat_link_per_upath",
	};
	int ndb = 6;
	int idb;
	struct stgcat_tape tape;
	struct stgcat_disk disk;
	struct stgcat_hpss hsm;
	struct stgcat_hsm castor;
	struct stgcat_alloc alloc;
	struct stgcat_link link;
	struct stgcat_entry stcp;
	struct stgpath_entry stpp;
	Cdb_off_t found_offset;
	char tmpbuf[21];
	char *error = NULL;
	int rc;
	int iargv;
	int stgpath_mode = 0;

	Coptind = 1;
	Copterr = 1;
	while ((c = Cgetopt(argc,argv,"Chu:p:")) != -1) {
		switch (c) {
		case 'C':
			stgpath_mode = 1;
			break;
		case 'h':
			help = 1;
			break;
		case 'u':
			Cdb_username = Coptarg;
			break;
		case 'p':
			Cdb_password = Coptarg;
			break;
		case '?':
			++errflg;
			printf("Unknown option\n");
			break;
		default:
			++errflg;
			printf("?? Cgetopt returned character code 0%o (octal) 0x%lx (hex) %d (int) '%c' (char) ?\n"
						 ,c,(unsigned long) c,c,(char) c);
			break;
		}
	}

	if (errflg != 0) {
		printf("### Cgetopt error\n");
		check_Cdbentry_usage();
		return(EXIT_FAILURE);
	}

	if (help != 0) {
		check_Cdbentry_usage();
		return(EXIT_SUCCESS);
	}

	if (Coptind >= argc) {
		printf("?? Cgetopt parsing error\n");
		check_Cdbentry_usage();
		return(EXIT_FAILURE);
	}

	/* Check the arguments */
	if (stgpath_mode == 0) {
      for (iargv = Coptind; iargv < argc; iargv++) {
        if ((reqid = atoi(argv[iargv])) <= 0) {
          printf("### one of the argument is <= 0\n");
          check_Cdbentry_usage();
          return(EXIT_FAILURE);
        }
      }
	}

	/* We open a connection to Cdb */
	if (Cdb_username == NULL) {
		Cdb_username = CDB_USERNAME;
	}
	if (Cdb_password == NULL) {
		Cdb_password = CDB_PASSWORD;
	}

	if (Cdb_login(Cdb_username,Cdb_password,&Cdb_session) != 0) {
		printf("### Cdb_login(\"%s\",\"%s\",&Cdb_session) error, %s - %s\n"
					 ,Cdb_username
					 ,Cdb_password
					 ,sstrerror(serrno)
					 ,strerror(errno));
		if (Cdb_api_error(&Cdb_session,&error) == 0) {
			printf("--> more info:\n%s",error);
		}
		rc = EXIT_FAILURE;
		goto check_Cdbentry_return;
	}

	Cdb_session_opened = -1;

	/* We open the database */
	if (Cdb_open(&Cdb_session,"stage",&Cdb_stage_interface,&Cdb_db) != 0) {
		printf("### Cdb_open(&Cdb_session,\"stage\",&Cdb_stage_interface,&Cdb_db) error, %s\n"
					 ,sstrerror(serrno));
		if (Cdb_api_error(&Cdb_session,&error) == 0) {
			printf("--> more info:\n%s",error);
		}
		printf("### PLEASE SUBMIT THE STAGER DATABASE WITH THE TOOL Cdb_create IF NOT ALREADY DONE.\n");
		rc = EXIT_FAILURE;
		goto check_Cdbentry_return;
	}

	Cdb_db_opened = -1;

	/* Loop on the arguments */
	for (iargv = Coptind; iargv < argc; iargv++) {
		int global_find_status = 0;
        char *upath;

        if (stgpath_mode == 0) {
          reqid = atoi(argv[iargv]);
        } else {
          upath = argv[iargv];
        }

		/* Search for the corresponding record in all databases */
		for (idb = 0; idb < ndb; idb++) {
			int find_status;
			
			switch (idb) {
			case 0:
				if (stgpath_mode != 0) {
					break;
				}
				memset(&tape,0,sizeof(struct stgcat_tape));
				tape.reqid = reqid;
				if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&tape,&found_offset,&tape)) == 0) {
					Cdb2stcp(&stcp,&tape,NULL,NULL,NULL,NULL);
					++global_find_status;
					printf("--> Reqid %d found at offset %s in database \"%s\":\n",
								 reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
								 db[idb]);
					print_stcp(&stcp);
				}
				break;
			case 1:
				if (stgpath_mode != 0) {
					break;
				}
				memset(&disk,0,sizeof(struct stgcat_disk));
				disk.reqid = reqid;
				if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&disk,&found_offset,&disk)) == 0) {
					Cdb2stcp(&stcp,NULL,&disk,NULL,NULL,NULL);
					++global_find_status;
					printf("--> Reqid %d found at offset %s in database \"%s\":\n",
								 reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
								 db[idb]);
					print_stcp(&stcp);
				}
				break;
			case 2:
				if (stgpath_mode != 0) {
					break;
				}
				memset(&hsm,0,sizeof(struct stgcat_hpss));
				hsm.reqid = reqid;
				if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&hsm,&found_offset,&hsm)) == 0) {
					Cdb2stcp(&stcp,NULL,NULL,&hsm,NULL,NULL);
					++global_find_status;
					printf("--> Reqid %d found at offset %s in database \"%s\":\n",
								 reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
								 db[idb]);
					print_stcp(&stcp);
				}
				break;
			case 3:
				if (stgpath_mode != 0) {
					break;
				}
				memset(&castor,0,sizeof(struct stgcat_hsm));
				castor.reqid = reqid;
				if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&castor,&found_offset,&castor)) == 0) {
					Cdb2stcp(&stcp,NULL,NULL,NULL,&castor,NULL);
					++global_find_status;
					printf("--> Reqid %d found at offset %s in database \"%s\":\n",
								 reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
								 db[idb]);
					print_stcp(&stcp);
				}
				break;
			case 4:
				if (stgpath_mode != 0) {
					break;
				}
				memset(&alloc,0,sizeof(struct stgcat_alloc));
				alloc.reqid = reqid;
				if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&alloc,&found_offset,&alloc)) == 0) {
					Cdb2stcp(&stcp,NULL,NULL,NULL,NULL,&alloc);
					++global_find_status;
					printf("--> Reqid %d found at offset %s in database \"%s\":\n",
								 reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
								 db[idb]);
					print_stcp(&stcp);
				}
				break;
			case 5:
				if (stgpath_mode == 0) {
					break;
				}
				memset(&link,0,sizeof(struct stgcat_link));
                strcpy(link.upath,upath);
                if ((find_status = Cdb_keyfind_fetch(&Cdb_db,db[idb],key[idb],NULL,&link,&found_offset,&link)) == 0) {
                  Cdb2stpp(&stpp,&link);
                  ++global_find_status;
                  printf("--> Upath %s (reqid=%d) found at offset %s in database \"%s\":\n",
                         upath, link.reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
                         db[idb]);
                  print_stpp(&stpp);
                  while (Cdb_keynext_fetch(&Cdb_db,db[idb],key[idb],NULL,&found_offset,&link) == 0) {
                    Cdb2stpp(&stpp,&link);
                    ++global_find_status;
                    printf("--> Upath %s (reqid=%d) found again at offset %s in database \"%s\":\n",
                           upath, link.reqid, u64tostr((u_signed64) found_offset, tmpbuf, 0),
                           db[idb]);
                    print_stpp(&stpp);
                  }
				}
				break;
			default:
				printf("### Internal error\n");
				rc = EXIT_FAILURE;
				goto check_Cdbentry_return;
			}
		}
		if (global_find_status == 0) {
			if (stgpath_mode != 0) {
				printf("--> Upath %s not found\n",upath);
			} else {
				printf("--> Reqid %d not found\n",reqid);
			}
		} else {
          if (stgpath_mode != 0) {
            printf("==> Upath %s found %d time%s\n",
                   upath,
                   global_find_status,
                   global_find_status > 1 ? "s" : ""
                   );
          } else {
            printf("==> Reqid %d found %d time%s\n",
                   reqid,
                   global_find_status,
                   global_find_status > 1 ? "s" : ""
                   );
          }
		}
	}


 check_Cdbentry_return:
	if (Cdb_db_opened != 0) {
		Cdb_close(&Cdb_db);
	}
	if (Cdb_session_opened != 0) {
		Cdb_logout(&Cdb_session);
	}
	return(rc);
}

void check_Cdbentry_usage() {
	printf(
				 "\n"
				 "Usage : check_Cdbentry [-h] [-p password] [-u username] reqid [reqid [...]]\n"
				 "\n"
				 "or\n"
				 "        check_Cdbentry [-C] [-h] [-p password] [-u username] upath [upath [...]]\n"
				 "\n"
				 "  This program will load entries with reqid (>0) from Cdb and print in human format\n"
				 "\n"
				 "  -C                Search for an entry in stgpath-like database\n"
				 "  -h                Print this help and exit\n"
				 "  -u                Cdb username\n"
				 "  -p                Cdb password\n"
				 "\n");
}
