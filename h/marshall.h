/*
 * $Id: marshall.h,v 1.15 2005/02/22 13:28:13 jdurand Exp $
 */

/*
 */

/*
 * Copyright (C) 1990-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*  marshall.h   -   marshalling/unmarshalling definitions */

#ifndef _MARSHALL_H_INCLUDED_
#define _MARSHALL_H_INCLUDED_

#include <osdep.h>              /* Operating system dependencies        */
#include <memory.h>             /* memory operations definition         */
#include <arpa/inet.h>

#define SHORT			WORD
/* #define U_SHORT			U_WORD */
#define SHORTSIZE		WORDSIZE

#define marshall_WORD		marshall_SHORT
#define unmarshall_WORD		unmarshall_SHORT		

#define INC_PTR(ptr,n)		(ptr) = (char*)(ptr) + (n)
#define DIFF_PTR(ptr,base)      (char*)(ptr) - (char*)(base)

/*
 * BIT manipulation
 */

#define  BITSOFBYTE     8       /* number of bits in a byte             */

#define  bitsof(t)      sizeof(t)*BITSOFBYTE /* number of bits in a type*/

typedef  char*          bitvct; /* bit vector type definition           */

/*
 * Allocate enough memory for a 'bitvct' type variable containing
 * 'size' bits
 */

#define  bitalloc(size)		(bitvct)malloc(size/BITSOFBYTE + \
				((size%BITSOFBYTE) ? 1 : 0))

/*
 *  Set the bit 'bit-th' starting from the byte pointed to by 'ptr'
 */

#define  BIT_SET(ptr,bit)	{ char *p = (char*)(ptr) + (bit)/8; \
				  *p = *p | (1 << (7-(bit)%8)) ; \
				}

/*
 *  Clear the bit 'bit-th' starting from the byte pointed to by 'ptr'
 */

#define  BIT_CLR(ptr,bit)	{ char *p = (char*)(ptr) + (bit)/8; \
				  *p = *p & ~(1 << (7-(bit)%8)); \
				}

/*
 *  Test the bit 'bit-th' starting from the byte pointed to by 'ptr'
 */

#define  BIT_ISSET(ptr,bit)	(*(char*)((char*)(ptr)+(bit)/8) & (1 << (7-(bit)%8)))


/*
 *    B Y T E
 */

#define marshall_BYTE(ptr,n)	{ BYTE n_ = n; \
				  (void) memcpy((ptr),&n_,1); \
				  INC_PTR(ptr,1); \
				} 

#define unmarshall_BYTE(ptr,n)  { BYTE n_ = 0; \
				  (void) memcpy(&n_,(ptr),1); \
				  n = n_; \
				  INC_PTR(ptr,1); \
				} 

/*
 *    S H O R T
 */

#define marshall_SHORT(ptr,n)	{ SHORT n_ = htons((unsigned short)(n)); \
				  (void) memcpy((ptr),&n_,SHORTSIZE); \
				  INC_PTR(ptr,SHORTSIZE); \
				}
                                                                 
#define unmarshall_SHORT(ptr,n)	{ SHORT n_ = 0;  \
				  (void) memcpy(&n_,(ptr),SHORTSIZE); \
                                  n = ntohs((unsigned short)(n_)); \
				  if ( BIT_ISSET(ptr,0) && sizeof(SHORT)-SHORTSIZE > 0 ) \
					 (void) memset((char *)&n,255,sizeof(SHORT)-SHORTSIZE); \
				  INC_PTR(ptr,SHORTSIZE); \
				}

/*
 *    L O N G
 */

#define marshall_LONG(ptr,n)	{ LONG n_ = htonl((unsigned long)(n)); \
				  (void) memcpy((ptr),&n_,LONGSIZE); \
				  INC_PTR(ptr,LONGSIZE); \
				}
                                                                 
#define unmarshall_LONG(ptr,n)	{ LONG n_ = 0;  \
				  (void) memcpy(&n_,(ptr),LONGSIZE); \
                                  n = ntohl((unsigned long)(n_)); \
				  if ( BIT_ISSET(ptr,0) && sizeof(LONG)-LONGSIZE > 0 ) \
					 (void) memset((char *)&n,255,sizeof(LONG)-LONGSIZE); \
				  INC_PTR(ptr,LONGSIZE); \
				}

/*
 *    S T R I N G
 */

#define  marshall_STRING(ptr,str)       {  (void) strcpy((char*)(ptr),(char*)(str)); \
					   INC_PTR(ptr,strlen(str)+1); \
					}

#define  marshall_STRINGN(ptr,str,n)    {  (void) strncpy((char*)(ptr),(char*)(str),n); \
                                           ((char*)(ptr))[n-1] = 0;                     \
                                           if (strlen(str)+1 > n)                       \
                                             INC_PTR(ptr,n);                            \
                                           else                                         \
                                             INC_PTR(ptr,strlen(str)+1);                \
                                        }

#define  unmarshall_STRING(ptr,str)     { (void) strcpy((char*)(str),(char*)(ptr)); \
					  INC_PTR(ptr,strlen(str)+1); \
					}

EXTERN_C int _unmarshall_STRINGN (char **, char*, int);
#define  unmarshall_STRINGN(ptr,str,n)  _unmarshall_STRINGN(&ptr, str, n)

/*
 *    H Y P E R   ( 6 4   B I T S )
 */

#define  marshall_HYPER(ptr,n)          { U_HYPER u_ = n; \
					  U_LONG n_ = htonl((U_LONG)u_); \
					  (void) memcpy((ptr),&n_,LONGSIZE); \
					  INC_PTR(ptr,LONGSIZE); \
					  n_ = htonl((U_LONG)(u_ >> 32)); \
					  (void) memcpy((ptr),&n_,LONGSIZE); \
					  INC_PTR(ptr,LONGSIZE); \
					}

#define  unmarshall_HYPER(ptr,n)        { U_HYPER u_ = 0; \
					  U_LONG n_ = 0;  \
					  (void) memcpy(&n_,(ptr),LONGSIZE); \
					  u_ = ntohl(n_); \
					  INC_PTR(ptr,LONGSIZE); \
					  n_ = 0;  \
					  (void) memcpy(&n_,(ptr),LONGSIZE); \
					  u_ |= (((U_HYPER)ntohl(n_)) << 32); \
					  INC_PTR(ptr,LONGSIZE); \
					  n = u_; \
					}

/*
 *    O P A Q U E
 */

#define marshall_OPAQUE(ptr,raw,n)	{ (void) memcpy((ptr),(raw),(n)); \
					  INC_PTR(ptr,(n)); \
					}

#define unmarshall_OPAQUE(ptr,raw,n)	{ (void) memcpy((raw),(ptr),(n)); \
					  INC_PTR(ptr,(n)); \
					}

/*
 *    T I M E
 */

#define  marshall_TIME_T(ptr,n)       {  \
					   TIME_T _marshall_time_dummy = (TIME_T) n; \
					   marshall_HYPER(ptr,_marshall_time_dummy); \
					}

#define  unmarshall_TIME_T(ptr,n)     { \
					   TIME_T _unmarshall_time_dummy; \
					   unmarshall_HYPER(ptr,_unmarshall_time_dummy); \
					   n = (time_t) _unmarshall_time_dummy; \
					}

#endif /* _MARSHALL_H_INCLUDED_ */
