/*
 * $Id: vdqm_Replica.c,v 1.3 1999/09/02 15:21:50 obarring Exp $
 * $Log: vdqm_Replica.c,v $
 * Revision 1.3  1999/09/02 15:21:50  obarring
 * Add osdep.h because of new u_signed64 decl. in vdqm.h
 *
 * Revision 1.2  1999/09/01 15:11:46  obarring
 * Fix sccsid string
 *
 * Revision 1.1  1999/07/27 09:21:39  obarring
 * First version
 *
 */

/*
 * Copyright (C) 1999 by CERN IT-PDP/DM
 * All rights reserved
 */

/*
 * vdqm_Replica.c - Update VDQM replica database (server only).
 */

#ifndef lint
static char sccsid[] = "@(#)$Id: vdqm_Replica.c,v 1.3 1999/09/02 15:21:50 obarring Exp $";
#endif /* not lint */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <osdep.h>
#include <net.h>
#include <log.h>
#include <Castor_limits.h>
#include <Cthread_api.h>
#include <vdqm_constants.h>
#include <vdqm.h>
#define thID Cthread_self()

int vdqm_UpdateReplica(dgn_element_t *dgn_context) {
    vdqm_volrec_t *vol;
    vdqm_drvrec_t *drv;

    if ( dgn_context == NULL ) return(-1);

    CLIST_ITERATE_BEGIN(dgn_context->vol_queue,vol) {
        if ( vol->update == 1 ) {
            log(LOG_DEBUG,"(ID %d)vdqm_UpdateReplica() update for VolReqID=%d\n",thID,
                vol->vol.VolReqID);
            vol->update = 0;
        }
    } CLIST_ITERATE_END(dgn_context->vol_queue,vol);

    CLIST_ITERATE_BEGIN(dgn_context->drv_queue,drv) {
        if ( drv->update == 1 ) {
            log(LOG_DEBUG,"(ID %d)vdqm_UpdateReplica() update for DrvReqID=%d\n",thID,
                drv->drv.DrvReqID);
            drv->update = 0;
        }
    } CLIST_ITERATE_END(dgn_context->drv_queue,drv);
    return(0);
}

int vdqm_DeleteFromReplica(vdqmVolReq_t *VolReq, vdqmDrvReq_t *DrvReq) {

    if ( VolReq != NULL ) {
        log(LOG_DEBUG,"(ID %d)vdqm_DeleteFromReplica() delete VolReqID=%d\n",thID,
            VolReq->VolReqID);
    }
    if ( DrvReq != NULL ) {
        log(LOG_DEBUG,"(ID %d)vdqm_DeleteFromReplica() delete DrvReqID=%d\n",thID,
            DrvReq->DrvReqID);
    }
    return(0);
}
