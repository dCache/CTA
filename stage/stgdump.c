#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include "stage_api.h"
#include "osdep.h"
#include "Cgetopt.h"
#include "serrno.h"
#include "rfio_api.h"

void log_callback _PROTO((int, char *));
void qry_callback _PROTO((struct stgcat_entry *, struct stgpath_entry *));
void usage _PROTO((char *));

int fd_stcp;
int fd_stpp;
char *stcp_output = NULL;
char *stpp_output = NULL;
int reqid;

int main(argc,argv)
	int argc;
	char **argv;
{
	struct stgcat_entry stcp_dummy;
	int nstcp_output = 0;
	int nstpp_output = 0;
	int rc;
	char *func = "stgdump";
	static int stcp_output_flag;
	static int stpp_output_flag;
	int errflg = 0;
	int c;
	char *stghost = NULL;

	static struct Coptions longopts[] =
	{
		{"stcp_output",        REQUIRED_ARGUMENT,  &stcp_output_flag,  1},
		{"stpp_output",        REQUIRED_ARGUMENT,  &stpp_output_flag,  1},
		{"hostname",           REQUIRED_ARGUMENT,  NULL,              'h'},
		{NULL,                 0,                  NULL,               0}
	};

	if (stage_setlog(&log_callback) != 0) {
		stage_errmsg(func,"startup : Cannot set log callback (%s)\n", sstrerror(serrno));
	}
	if (stage_setcallback(&qry_callback) != 0) {
		stage_errmsg(func,"startup : Cannot set callback (%s)\n", sstrerror(serrno));
	}

	Coptind = 1;
	Copterr = 1;

	stcp_output_flag = 0;
	stpp_output_flag = 0;

	while ((c = Cgetopt_long (argc, argv, "h:", longopts, NULL)) != -1) {
		switch (c) {
		case 'h':
			stghost = Coptarg;
			break;
		case 0:
			/* Long option without short option correspondance */
			if (stcp_output_flag && ! stcp_output) {
				/* --stcp_output */
				stcp_output = Coptarg;
			}
			if (stpp_output_flag && ! stpp_output) {
				/* --stpp_output */
				stpp_output = Coptarg;
			}
			break;
		case '?':
			errflg++;
			break;
		default:
			errflg++;
			break;
		}
        if (errflg != 0) break;
	}

	if (errflg != 0) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if ((stcp_output == NULL) && (stpp_output == NULL)) {
		stage_errmsg(func,"arguments : --stcp_output or --stpp_output are mandatory\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	
	if ((stcp_output != NULL) && (stpp_output != NULL)) {
		if (strcmp(stcp_output,stpp_output) == 0) {
			stage_errmsg(func,"arguments : --stcp_output and --stpp_output are equivalent\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Open output files */
	if (stcp_output != NULL) {
		PRE_RFIO;
		if ((fd_stcp = rfio_open(stcp_output, O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0) {
			stage_errmsg(func,"%s rfio_open error : %s\n", stcp_output, rfio_serror());
			exit(EXIT_FAILURE);
		}
	}
	if (stpp_output != NULL) {
		PRE_RFIO;
		if ((fd_stpp = rfio_open(stpp_output, O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0) {
			stage_errmsg(func,"%s open error : %s\n", stpp_output, rfio_serror());
			close(fd_stcp);
			exit(EXIT_FAILURE);
		}
	}

	memset(&stcp_dummy, 0, sizeof(struct stgcat_entry));
	if (stcp_output != NULL) {
		if ((rc = stage_qry('t',                    /* t_or_d */
							(u_signed64) STAGE_ALL, /* Flags */
							stghost,                /* Hostname */
							1,                      /* nstcp_input */
							&stcp_dummy,            /* stcp_input */
							&nstcp_output,          /* nstcp_output */
							NULL,                   /* stcp_output */
							&nstpp_output,          /* nstpp_output */
							NULL                    /* stpp_output */
			)) < 0) {
			stage_errmsg(func,"stage_qry error : %s\n", sstrerror(serrno));
		} else {
			stage_errmsg(func,"==> Got %d stcp\n", nstcp_output);
		}
	}
	if (stpp_output != NULL) {
		if ((rc = stage_qry('t',                    /* t_or_d */
							(u_signed64) STAGE_ALL|STAGE_LINKNAME, /* Flags */
							stghost,                /* Hostname */
							1,                      /* nstcp_input */
							&stcp_dummy,            /* stcp_input */
							&nstcp_output,          /* nstcp_output */
							NULL,                   /* stcp_output */
							&nstpp_output,          /* nstpp_output */
							NULL                    /* stpp_output */
			)) < 0) {
			stage_errmsg(func,"stage_qry error : %s\n", sstrerror(serrno));
		} else {
			stage_errmsg(func,"==> Got %d stpp\n", nstpp_output);
		}
	}

	if (stcp_output != NULL) {
		PRE_RFIO;
		if ((rc = rfio_close(fd_stcp)) < 0) {
			stage_errmsg(func,"%s close error : %s\n", stcp_output, rfio_serror());
		}
	}
	if (stpp_output != NULL) {
		PRE_RFIO;
		if ((rc = rfio_close(fd_stpp)) < 0) {
			stage_errmsg(func,"%s close error : %s\n", stpp_output, rfio_serror());
		}
	}

	exit(rc ? EXIT_FAILURE : EXIT_SUCCESS);

}

void log_callback(level,message)
	int level;
	char *message;
{
	if (level == MSG_ERR)
		fprintf(stderr,"%s",message);
	return;
}

void qry_callback(stcp,stpp)
	struct stgcat_entry *stcp;
	struct stgpath_entry *stpp;
{
	char *func = "qry_callback";
	if ((stcp == NULL) && (stpp == NULL)) {
		stage_errmsg(func,"(stcp == NULL) && (stpp == NULL) !?\n");
		return;
	}
	if ((stcp != NULL) && (stcp_output != NULL)) {
		/* stcp record */
		PRE_RFIO;
		if (rfio_write(fd_stcp, stcp, sizeof(struct stgcat_entry)) != sizeof(struct stgcat_entry)) {
			stage_errmsg(func,"write of stcp error : %s\n", rfio_serror());
		}
		
	}
	if ((stpp != NULL) && (stpp_output != NULL)) {
		/* stpp record */
		PRE_RFIO;
		if (rfio_write(fd_stpp, stpp, sizeof(struct stgpath_entry)) != sizeof(struct stgpath_entry)) {
			stage_errmsg(func,"write of stpp error : %s\n", rfio_serror());
		}
		
	}
}

void usage(prog)
	char *prog;
{
	fprintf(stderr,"Usage: %s [-h hostname] --stcp_output <filename> --stcpp_output <filename>\n", prog);
}
