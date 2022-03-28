/*
 * @project      The CERN Tape Archive (CTA)
 * @copyright    Copyright © 2003-2022 CERN
 * @license      This program is free software, distributed under the terms of the GNU General Public
 *               Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING". You can
 *               redistribute it and/or modify it under the terms of the GPL Version 3, or (at your
 *               option) any later version.
 *
 *               This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *               WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *               PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *               In applying this licence, CERN does not waive the privileges and immunities
 *               granted to it by virtue of its status as an Intergovernmental Organization or
 *               submit itself to any jurisdiction.
 */

#pragma once

struct rmc_srv_rqst_context {
  const char *localhost;
  int rpfd;
  char *req_data;
  const char *clienthost;
};

int rmc_srv_export  (const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_findcart(const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_getgeom (const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_import  (const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_mount   (const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_readelem(const struct rmc_srv_rqst_context *const rqst_context);
int rmc_srv_unmount (const struct rmc_srv_rqst_context *const rqst_context);
