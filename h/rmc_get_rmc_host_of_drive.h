/******************************************************************************
 *                h/rmc_get_rmc_host_of_drive.h
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

/******************************************************************************
 * Please note that this file is part of the internal API of the rmc daemon
 * and its client software and should therefore not be distributed to end users
 *****************************************************************************/

#pragma once

/**
 * Returns the rmc host of the specified drive string.  The drive string either
 * starts with:
 *
 *     acs@rmc_host,
 * or:
 *
 *     smc@rmc_host,
 *
 * @param drive The drive string
 * @param rmc_host_buf Output parameter.  The buffer into which the rmc host
 * should be written as a null terminated string.
 * @param rmc_host_buflen The length of the rmc host buffer.
 * @return 0 on success and -1 on failure.
 */
int rmc_get_rmc_host_of_drive(const char *const drive,
	char *const rmc_host_buf, const int rmc_host_buflen);

