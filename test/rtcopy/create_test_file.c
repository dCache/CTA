/*
 * $Id: create_test_file.c,v 1.2 2000/03/30 14:50:56 jdurand Exp $
 */

/*
 * Copyright (C) 1990-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

main(argc, argv)
int argc; 
char **argv;
{
	char *buffer;
	int  fd;
	int i ;
	int lrecl = 32400;
	int nbrecords = 50;

	if (argc > 2)
		lrecl = atoi (argv[2]);
	if (argc > 3)
		nbrecords = atoi (argv[3]);

	if ((buffer = malloc (lrecl)) == NULL) {
		perror ("malloc");
		exit (1);
	}

	for (i = 0; i < lrecl; i++) {
		buffer[i] = i;
	}
	if ((fd = open (argv[1], O_WRONLY|O_CREAT, 0644)) < 0) {
		perror ("opening disk file");
		exit (1);
	} 
	for (i = 0; i < nbrecords; i++) {
		write (fd, buffer, lrecl);
	}
	close (fd);
	exit (0);
}
