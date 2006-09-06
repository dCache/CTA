<?php

/******************************************************************************************************
 *                                                                                                    *
 * utils.php                                                                                          *
 * Copyright (C) 2005 CERN IT/FIO (castor-dev@cern.ch)                                                *
 *                                                                                                    *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU *
 * General Public License as published by the Free Software Foundation; either version 2 of the       *
 * License, or (at your option) any later version.                                                    *
 *                                                                                                    *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without  *
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 * General Public License for more details.                                                           *
 *                                                                                                    *
 * You should have received a copy of the GNU General Public License along with this program; if not, *
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,  *
 * USA.                                                                                               *
 *                                                                                                    *
 ******************************************************************************************************/

/**
 * $Id: utils.php,v 1.2 2006/09/06 12:53:44 waldron Exp $
 */

include("config.php");

/**
 * get micro time
 */
function getmicrotime() {  
	$temparray  = split(" ", microtime());  
	$returntime = $temparray[0] + $temparray[1];  
	return $returntime;  
}

/**********************************************************************************************/

?>