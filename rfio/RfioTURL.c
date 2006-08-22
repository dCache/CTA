/*
 *
 * Copyright (C) 2003 by CERN/IT/ADC/CA
 * All rights reserved
 *
 * @(#)$RCSfile: RfioTURL.c,v $ $Revision: 1.19 $ $Release$ $Date: 2006/08/22 08:25:12 $ $Author: gtaur $
 *
 *
 *
 * @author Olof Barring
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: RfioTURL.c,v $ $Revision: 1.19 $ $Release$ $Date: 2006/08/22 08:25:12 $ Olof Barring";
#endif /* not lint */
/** RfioTURL.c - RFIO TURL handling
 *
 * Auxiliary routines for handling the RFIO TURL convention:
 * <BR><CODE>
 * rfio://[hostname][:port]/path
 * </CODE><P>
 * Examples:
 * - Physical (remote) disk file following the SHIFT/CASTOR "NFS" convention:
 *   - TURL: rfio://pub001d//shift/pub001d/data01/c3/stage/abc.123
 *   - RFIO path: /shift/pub001d/data01/c3/stage/abc.123
 * - Physical (remote) disk file, standard format:
 *   - TURL: rfio://wacdr002d//tmp/abc.123
 *   - RFIO path: wacdr002d:/tmp/abc.123
 * - CASTOR file:
 *   - TURL: rfio:////castor/cern.ch/user/n/nobody/abc.123 or 
 *           rfio://STAGE_HOST:STAGE_PORT/?
 *                     svcClass=myClass
 *                      &castorVersion=2
 *                       &path=/castor/cern.ch/user/n/nobody/abc.123
 *           rfio://STAGE_HOST:STAGE_PORT//castor/cern.ch/user/n/nobody/abc.123?
 *                     svcClass=myClass
 *                      &castorVersion=2
 *
 *           SvcClass and CastorVersion can be undefined  and default values are used.  
 *
 *   - RFIO path: /castor/cern.ch/user/n/nobody/abc.123
 *
 * - Remote file on a windows file server (shows the importance of the '/' delimiter)
 *   - TURL: rfio://pcwin32/c:\temp\abc.123
 *   - RFIO path: pcwin32:c:\temp\abc.123
 *
 */

#include <RfioTURL.h>
#include <stager_mapper.h>
#include <Castor_limits.h>
#include <grp.h>
#include <sys/types.h>
#include <common.h>
#define DEFAULT_HOST "stagepublic"
#define DEFAULT_PORT2 9002
#define DEFAULT_PORT1 5007
#define DEFAULT_SVCCLASS ""  
#define DEFAULT_VERSION 1

static int tURLPrefixKey = -1;
static int tURLPrefixLenKey = -1;

int tStageHostKey = -1;
int tStagePortKey = -1;
int tSvcClassKey = -1;
int tCastorVersionKey = -1;
// functions used only in this file.

static int checkAlphaNum(char *p) 
{
  int i;
  if ( p == NULL ) return(-1);
  for ( i=0; p[i] != '\0'; i++ ) {
    if ( !isalnum(p[i]) ) return(0);
  }
  return(1);
}

static int checkNum(char *p) 
{ 
  int i;
  if ( p == NULL ) return(-1);
  for ( i=0; p[i] != '\0'; i++ ) {
    if ( !isdigit(p[i]) ) return(0);
  }
  return(1);
}


char *getRfioTURLPrefix() 
{
  char *str;
  int rc, *tURLLen = NULL;
  
  rc = Cglobals_get(&tURLPrefixLenKey,(void *)&tURLLen,sizeof(int));
  if ( rc == -1 || tURLLen == NULL ) return(NULL);

  if ( *tURLLen == 0 ) return(DEFAULT_RFIO_TURL_PREFIX);
  
  rc = Cglobals_get(&tURLPrefixKey,(void *)&str,*tURLLen);
  if ( rc == -1 || str == NULL ) return(NULL);
  return(str);
}



/********************************************************************************************************************
 * This Function:                                                                                                   *
 * if *host or *svc are null or empty strings retrive the values, the same if *port and *version are <= 0.          * 
 * To retrive the values it looks if:                                                                               *
 * enviroment variables are set or stager_mapper as values defined or castor.conf or (if it doesn't have valid)     * 
 * it uses default hard coded values.                                                                               *
 *                                                                                                                  *
 *******************************************************************************************************************/   

int getDefaultForGlobal(
			char** host,
			int* port,
			char** svc,
			int* version)


{ 
	char *hostMap, *hostDefault, *svcMap, *svcDefault;
	int versionMap,versionDefault, portDefault, ret;
	char* aux=NULL; 
        struct group* grp=NULL; 
	gid_t gid;

       hostMap=hostDefault=svcMap=svcDefault=NULL;
       versionMap=versionDefault=portDefault=ret=0;

	if(host == NULL || port == NULL || svc == NULL || version == NULL ){ return (-1);}
	hostDefault=*host;
	svcDefault=*svc;
	portDefault=*port;
	versionDefault=*version;

	gid=getgid();

	if((grp = getgrgid(gid)) == NULL ){
		ret=just_stage_mapper(getenv("USER"),NULL,&hostMap,&svcMap,&versionMap); 
	}
	else{
		ret=just_stage_mapper(NULL,grp->gr_name,&hostMap,&svcMap,&versionMap);
	}

	if(hostDefault==NULL || strcmp(hostDefault,"")==0){
		if(hostDefault){free(hostDefault);}
		aux=getenv("STAGE_HOST");
		hostDefault=aux==NULL?NULL:strdup(aux);
		if (hostDefault==NULL || strcmp(hostDefault,"")==0 ){
			if(hostDefault){free(hostDefault);}
			if (hostMap==NULL || strcmp(hostMap,"")==0 ){
				aux=(char*)getconfent("STAGER","HOST", 0);
				hostDefault=aux==NULL?NULL:strdup(aux);
				if (hostDefault==NULL || strcmp(hostDefault,"")==0 ){
					if(hostDefault){free(hostDefault);}
					hostDefault=strdup(DEFAULT_HOST);
				}
			}
			else{
				if(hostDefault){free(hostDefault);}
				hostDefault=strdup(hostMap);
			}
		}
	}
	

	if (svcDefault==NULL || strcmp(svcDefault,"")==0){
		if(svcDefault){free(svcDefault);}
		aux=getenv("STAGE_SVCCLASS");
		svcDefault= aux==NULL? NULL: strdup(aux);
		if (svcDefault==NULL || strcmp(svcDefault,"")==0 ){
			if(svcDefault){free(svcDefault);}
			if (svcMap==NULL || strcmp(svcMap,"")==0 ){
				aux=(char*)getconfent("STAGER","SVC_CLASS", 0);
				svcDefault=aux==NULL?NULL:strdup(aux);
				if (svcDefault==NULL || strcmp(svcDefault,"")==0 ){
					if(svcDefault){free(svcDefault);}
					svcDefault=strdup(DEFAULT_SVCCLASS);
				}
			}
			else{
				if(svcDefault){free(hostDefault);}
				svcDefault=strdup(svcMap);
			}
		}

	}
	if (versionDefault<=0){
		aux=getenv("RFIO_USE_CASTOR_V2");
		if(aux){
		        
			versionDefault=strcasecmp(aux,"YES")==0?2:1;
		}else{versionDefault=0;}
    		if (versionDefault<=0){
			versionDefault=versionMap;
			if (versionDefault<=0){
			aux=(char*)getconfent("STAGER","VERSION",0);
			versionDefault=aux==NULL?0:atoi(aux);
				if (versionDefault<=0){
					   versionDefault= DEFAULT_VERSION;
			 	}
			}
		}
	}

	if (portDefault<=0){
		aux=getenv("STAGE_PORT");
		portDefault=aux==NULL?0:atoi(aux);
		if (portDefault<=0){
			aux=(char*)getconfent("STAGER", "PORT", 0);
			portDefault=aux==NULL?0:atoi(aux);
			if (portDefault<=0){
			   portDefault= versionDefault==2?DEFAULT_PORT2:DEFAULT_PORT1;
			}
		}
		
	}

	if (*host==NULL || strcmp(*host,"")){*host=hostDefault;}	
	if (port==NULL || *port<=0) {*port=portDefault;}
	if (*svc==NULL || strcmp(*svc,"")){*svc=svcDefault;}
	if (version==NULL || *version<=0){*version=versionDefault;}

	return (1);
}

/********************************************************************************************************************
 * This Function is parsing the string and returning the Turl struct that will be used by parse.c.                  *
 * Also it sets global variables thread safe that are used in the other functions.                                  *
 *                                                                                                                  *
 *******************************************************************************************************************/   

int rfioTURLFromString(
                       char *tURLStr,
                       RfioTURL_t *tURL
                       ) 
{
  char *p, *q, *q1, *orig, *prefix;
  RfioTURL_t _tURL;
  int ret;
  char *path1, *path2, *mySvcClass, *myCastorVersion;
  char* buff;
  int versionNum=0;
  //void ** auxPointer=0;
  char endMark;

  char ** globalHost,  **globalSvc;
  int *globalVersion, *globalPort;

  globalHost=globalSvc=0;
  globalVersion=globalPort=0;

  path1=path2=myCastorVersion=mySvcClass=myCastorVersion=NULL;
  if ( tURLStr == NULL || tURL == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  prefix = getRfioTURLPrefix();
  if ( prefix == NULL ) return(-1);	
  if ( strstr(tURLStr,prefix) != tURLStr) {
    serrno = EINVAL;
    return(-1);
  }  
  /*
   * Extract the protocol name
   */

  orig = p = strdup(tURLStr);
  if ( p == NULL ) return(-1);
  
  q = strstr(p,":");
  if ( q == NULL ) {
    serrno = EINVAL;
    free(orig);
    return(-1);
  }
  *q = '\0';
  if ( strlen(p) >= sizeof(tURL->rfioProtocolName) ) {
    serrno = E2BIG;
    free(orig);
    return(-1);
  }

  if ( checkAlphaNum(p) != 1 ) {
    serrno = EINVAL;
    free(orig);
    return(-1);
  }
  
  strcpy(_tURL.rfioProtocolName,p);
  *q = ':';

  /*
   * Split off the prefix since we don't need it anymore.
   */
  p += strlen(prefix);

  /*
   * Expect the [host][:port] name at this point. The hostname is delimited from the path by a '/'
   * which is mandatory (thus, even if there is no hostname, like in a castor path)
   */

  q = strstr(p,"/");
  q1= strstr(p,"?");

  if ( q == NULL) {
       serrno = EINVAL;
       free(orig);
       return(-1);
  }

  if (q1 && q1<q) q=q1;

  endMark=*q;
  *q = '\0';

  q1 = strstr(p,":");
  if ( q1 != NULL ) {
    *q1 = '\0';
    q1++;
    if ( checkNum(q1) == 0 ) {
      serrno = EINVAL;
      free(orig);
      return(-1);
    }
    _tURL.rfioPort = atoi(q1);
  } else _tURL.rfioPort = 0;

  if ( strlen(p) >= sizeof(tURL->rfioHostName) ) {
    serrno = E2BIG;
    free(orig);
    return(-1);
  }
  strcpy(_tURL.rfioHostName,p);
   
  /*
   * Finally we only have the path and parameters  left
   */

/*  magic things to deal with a single slash */	
  
  *q=endMark;
  p=q;

  q=strstr(p,"?");
  if (!q){
     // no parameters specified
        path1=p;
  }
  else{  // parameters to parse
  
        if(p==q){
	    // no path after the port number
            path1=NULL;
	    *p='\0';
             p++;
        }
        else {
	    path1=p;
            *q='\0';
	    p=q+1;	
        }

	mySvcClass=strstr(p,"svcClass=");
	if (mySvcClass){
	    mySvcClass+=9; // to remove "svcClass="
       
	}


	myCastorVersion=strstr(p,"castorVersion=");
	if (myCastorVersion){
	    myCastorVersion+=14; // to remove "castorVersion="	
	
	}

	path2=strstr(p,"path=");
	if (path2){
	    path2+=5; // to remove "path="	
	   
	}

	if (mySvcClass){
              q1=strstr(mySvcClass,"&");
	       if (q1){*q1='\0';}
	}
	if (myCastorVersion){
              q1=strstr(myCastorVersion,"&");
              if(q1){*q1='\0';}
	}
	if (path2){
	      q1=strstr(path2,"&");
              if(q1){*q1='\0';}
	}

  }
   
   if (path2)
       path1=NULL;  // if I have the path given as option is ignored the other one.


   if (!path1 && !path2 ){
   // at least the path should be specified
      serrno = EINVAL;
      free(orig);
      return(-1);

   }

   if (path1){
     
     if ( strlen(path1) >= sizeof(tURL->rfioPath) ) {
         serrno = E2BIG;
         free(orig);
         return(-1);

     }

     strcpy(_tURL.rfioPath,path1);
     
   }

   if (path2){

     if ( strlen(path2) >= sizeof(tURL->rfioPath) ) {
         serrno = E2BIG;
         free(orig);
         return(-1);
     }
     
     strcpy(_tURL.rfioPath,path2);
   }

/* to remove the // or /// because of srm1 problems */

   if (_tURL.rfioPath && (strstr(_tURL.rfioPath,"//")==_tURL.rfioPath )){ 
	strcpy(_tURL.rfioPath,(char*)&_tURL.rfioPath[1]);
        if (_tURL.rfioPath && (strstr(_tURL.rfioPath,"//") == _tURL.rfioPath)){ 
	   strcpy(_tURL.rfioPath,(char*)&_tURL.rfioPath[1]);
        }
   }

   if (strstr(_tURL.rfioPath,"/castor") == _tURL.rfioPath ){ 
     
    /* from here setting the proper enviroment variable */
    /* getting default value that can be used */

	if (myCastorVersion){ 
      		if (!strcmp(myCastorVersion,"2")){
       			 versionNum=2;
       		}
		if (!strcmp(myCastorVersion,"1")){
       			 versionNum=1;
       		}
		
     	}
	
	/* Let's now set the global variable thread specific */
	
	ret=Cglobals_get(&tStageHostKey,(void **)&globalHost,sizeof(void*));
        
	if (ret<0){
		serrno = EINVAL;
		free(orig);
		return -1;

	}
	if(*globalHost){free(*globalHost); *globalHost=NULL;}

	if (strcmp(_tURL.rfioHostName,"")){
		*globalHost=strdup(_tURL.rfioHostName);	
	}

	ret=Cglobals_get(&tSvcClassKey,(void **)&globalSvc,sizeof(void*));

	if (ret<0){
		serrno = EINVAL;
		if(*globalHost){free(*globalHost);*globalHost=NULL;}
		free(orig);
		return -1;

	}
	if(*globalSvc){free(*globalSvc);*globalSvc=NULL;}

	if (mySvcClass && strcmp(mySvcClass,"")){
		*globalSvc=strdup(mySvcClass);
	}
	ret=Cglobals_get(&tStagePortKey,(void **)&globalPort,sizeof(int));
	if (ret<0){
		serrno = EINVAL;
		if(*globalHost){free(*globalHost);*globalHost=NULL;}
		if(*globalSvc){free(*globalSvc);*globalSvc=NULL;}
		free(orig);
		return -1;

	}
	*globalPort=0;
	if (_tURL.rfioPort){
		*globalPort=_tURL.rfioPort;
	}
	ret=Cglobals_get(&tCastorVersionKey,(void **)&globalVersion,sizeof(int));

	if (ret<0){
		serrno = EINVAL;
		if(*globalHost){free(*globalHost);*globalHost=NULL;}
		if(*globalSvc){free(*globalSvc);*globalSvc=NULL;}
		free(orig);
		return -1;

	}
	*globalVersion=0;
	if (versionNum){
		*globalVersion=versionNum;
	}
	ret=getDefaultForGlobal(globalHost,globalPort,globalSvc,globalVersion);
	if (ret<0){
			serrno = EINVAL;
			if(*globalHost){free(*globalHost);*globalHost=NULL;}
			if(*globalSvc){free(*globalSvc);*globalSvc=NULL;}
			free(orig);
			return -1;

	}
	_tURL.rfioHostName[0]=0;
	_tURL.rfioPort=-1;
     }
    
     free(orig);

  /*
   * Everything OK. Copy the temporary structure to the output
   */
    *tURL = _tURL;

     return(0);
}


