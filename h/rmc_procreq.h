/******************************************************************************
 *                      rmc_procreq.h
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef _RMC_PROCREQ_H
#define _RMC_PROCREQ_H 1

int rmc_srv_export(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_findcart(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_getgeom(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_import(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_mount( const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_readelem(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_unmount(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_genericmount(const int rpfd, char *const req_data, const char *const clienthost);
int rmc_srv_genericunmount(const int rpfd, char *const req_data, const char *const clienthost);
#endif
