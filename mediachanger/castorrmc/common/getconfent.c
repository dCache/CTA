/*
 * $Id: getconfent.c,v 1.5 1999/07/21 12:42:20 jdurand Exp $
 * $Log: getconfent.c,v $
 * Revision 1.5  1999/07/21 12:42:20  jdurand
 * HP-UX's cc [without options] don't like function prototypes. Changed it
 * to old C style.
 *
 * Revision 1.4  1999/07/20 15:11:39  obarring
 * Add CVS log directive
 *
 */

#include <stdio.h>
#include <string.h>
#include <Cglobals.h>
#include <serrno.h>

#ifndef PATH_CONFIG
#if defined(_WIN32)
#define PATH_CONFIG "%SystemRoot%\\system32\\drivers\\etc\\shift.conf"
#else
#define PATH_CONFIG "/etc/shift.conf"
#endif
#endif /* PATH_CONFIG */

#ifdef CONST
#undef CONST
#endif
#if defined(__STDC__)
#define CONST const
#else
#define CONST
#endif

CONST char char_set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+|~`\\=-{}[]:;''\",./<>?";


char *getconfent_r(category, name, flags, buffer, bufsiz)
     char *category;
     char *name;
     int flags;
     char *buffer;
     int bufsiz;
{
	char    *filename=PATH_CONFIG;
	FILE    *fp;
	char    *p, *cp, *ep;
	char    *getenv();
	int     found = 0;
	char    path_config[256];
	char    *separator;
        if ((p = getenv("PATH_CONFIG")) != NULL)       {
                filename=p;
        }
#if defined(_WIN32)
        if (strncmp (filename, "%SystemRoot%\\", 13) == 0 &&
            (p = getenv ("SystemRoot")))
                sprintf (path_config, "%s\\%s", p, strchr (filename, '\\'));
        else
#endif
                strcpy (path_config, filename);

        if ((fp = fopen(path_config,"r")) == NULL)       {
		serrno = SENOCONFIG;
                return (NULL);
        }

        for (;;)        {
                p = fgets(buffer, bufsiz-1, fp);
                if (p == NULL)  {
                        break;
                }
		if (*p == '#') continue;
		if ((cp = strpbrk(p,char_set)) == NULL) continue;
		if (*cp == '#') continue;
		if ((ep = strpbrk(cp," \t")) == NULL) ep = &cp[strlen(cp)];
		if ( *ep == '\n' ) continue;
		*ep  = '\0';
		if (strcmp(cp,category) == 0) {
			p = &ep[1];
			if ((cp = strpbrk(p,char_set)) == NULL ) continue;
			if (*cp == '#') continue;
			if ((ep = strpbrk(cp," \t")) == NULL) ep = &cp[strlen(cp)];
			if ( *ep == '\n' ) continue;
			*ep = '\0';
			if ( strcmp(cp,name) == 0) {

				if (flags != 0) {
				   separator = "#\n";
				} else {
				   separator = "#\t \n";
				}
				p = &ep[1];
				if ((cp = strpbrk(p,char_set)) == NULL) continue;
				if (*cp == '#') continue;
				if ((ep = strpbrk(cp,separator)) == NULL) ep = &cp[strlen(cp)];
				*ep = '\0';
				found++;
				break;
			}
			else {
				continue;
			}
		} else {
			continue;
		}
	}
	if (fclose(fp)) return(NULL);
	if (found == 0) return(NULL);
	else return(cp);
}

static int value_key = -1;

char *getconfent(category, name, flags)
     char *category;
     char *name;
     int flags;
{
	char *value = NULL;

	Cglobals_get(&value_key,(void **) &value,BUFSIZ+1);
	if ( value == NULL ) {
		return(NULL);
	}

	return(getconfent_r(category,name,flags,value,BUFSIZ+1));
}
