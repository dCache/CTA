/*
 * $Id: Cinit.h,v 1.2 2000/08/25 12:53:40 baud Exp $
 */

/*
 * Copyright (C) 2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*
 * @(#)$RCSfile: Cinit.h,v $ $Revision: 1.2 $ $Date: 2000/08/25 12:53:40 $ CERN IT-PDP/DM Jean-Philippe Baud
 */

#ifndef _CINIT_H
#define _CINIT_H

	/* structure to be used with Cinitdaemon()/Cinitservice() */

struct main_args {
	int	argc;
	char	**argv;
};
#endif
