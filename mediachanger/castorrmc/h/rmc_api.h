/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2002-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "osdep.h"
#include "smc_struct.h"

                        /*  function prototypes */

EXTERN_C int rmc_dismount(const char *const server, const char *const vid, const int drvord, const int force);
EXTERN_C int rmc_errmsg(const char *const func, const char *const msg, ...);
EXTERN_C int rmc_export(const char *const server, const char *const vid);
EXTERN_C int rmc_find_cartridge(const char *const server, const char *const pattern, const int type, const int startaddr, const int nbelem, struct smc_element_info *const element_info);
EXTERN_C int rmc_get_geometry(const char *const server, struct robot_info *const robot_info);
EXTERN_C int rmc_import(const char *const server, const char *const vid);
EXTERN_C int rmc_mount(const char *const server, const char *const vid, const int side, const int drvord);
EXTERN_C int rmc_read_elem_status(const char *const server, const int type, const int startaddr, const int nbelem, struct smc_element_info *const element_info);
EXTERN_C void rmc_seterrbuf(const char *const buffer, const int buflen);
EXTERN_C int send2rmc(const char *const host, const char *const reqp, const int reql, char *const user_repbuf, const int user_repbuf_len);
