#ifndef _CSEC_API_H
#define _CSEC_API_H

#include <sys/types.h>
#include <Csec_common.h>

/* Init and cleanup */
int  Csec_client_initContext _PROTO ((Csec_context_t *,
						int,
						Csec_protocol *));

int  Csec_server_initContext _PROTO ((Csec_context_t *,
						int,
						Csec_protocol *));
int  Csec_server_reinitContext  _PROTO ((Csec_context_t *,
						   int,
						   Csec_protocol *));

int  Csec_clearContext _PROTO ((Csec_context_t *));

/* context establishment */

int  Csec_server_establishContext _PROTO ((Csec_context_t *,
                                                     int s));
int  Csec_client_establishContext _PROTO ((Csec_context_t *,
                                                     int s));

/* Error lookup */
char* Csec_getErrorMessage _PROTO(());


/* security options, authorization id & delegated credential retrieval */

int Csec_client_setSecurityOpts _PROTO((Csec_context_t *, int));

int Csec_server_setSecurityOpts _PROTO((Csec_context_t *, int));

int Csec_server_getDelegatedCredential _PROTO((Csec_context_t *, char **, void **, size_t *));

int Csec_client_setAuthorizationId _PROTO((Csec_context_t *, const char *, const char *));

int Csec_server_getAuthorizationId _PROTO((Csec_context_t *, char **, char **));

char *Csec_server_get_client_vo _PROTO((Csec_context_t *));

char **Csec_server_get_client_fqans _PROTO((Csec_context_t *, int *));

int Csec_client_setVOMS_data _PROTO((Csec_context_t *, const char *, char **, int));

/* Service type & name handling */

char *Csec_client_get_service_name _PROTO((Csec_context_t *));

int Csec_server_set_service_type _PROTO ((Csec_context_t *, 
						   int));
char *Csec_server_get_service_name _PROTO((Csec_context_t *));

int Csec_server_getClientId _PROTO ((Csec_context_t *, char **, char **));

int Csec_client_get_service_type _PROTO ((Csec_context_t *));

int Csec_isIdAService _PROTO ((const char *, const char *));

int Csec_mapToLocalUser _PROTO ((const char *, const char *, char *, size_t, uid_t *, gid_t *));

/* Funtion providing a default per thread context */
Csec_context_t* Csec_get_default_context _PROTO(());

#endif
