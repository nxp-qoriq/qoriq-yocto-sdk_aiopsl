/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//*
 @File          drv.h

 @Description   DPNI driver structure and internal functions.
*//***************************************************************************/

#ifndef __DRV_H
#define __DRV_H

#include "common/types.h"
#include "dplib/fsl_dpni_drv.h"
#include "net/fsl_net.h"
#include "dplib/fsl_dprc.h"

int dpni_drv_probe(struct dprc *dprc, uint16_t	mc_niid, uint16_t aiop_niid,
		struct dpni_pools_cfg *pools_params);

#define DPNI_DRV_FLG_PARSE		0x80
#define DPNI_DRV_FLG_PARSER_DIS		0x40
#define DPNI_DRV_FLG_MTU_ENABLE		0x20


struct dpni_drv {
	/** network interface ID which is equal to this entry's index in the NI
	 *  table - internal to AIOP */
	uint16_t            aiop_niid;

#if 0
	/** TODO: the mc_niid field will be necessary if we decide to close the
	 *  DPNI at the end of Probe. network interface ID assigned by MC -
	 *  known outside AIOP */
	uint16_t            mc_niid;
#endif

	/** MAC address of this NI */
	uint8_t 	    mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	/** Storage profile ID */
	uint8_t             spid;
	uint8_t             res[1];
	/** Queueing destination for the enqueue. */
	uint16_t            qdid;
	/** starting HXS */
	uint16_t            starting_hxs;
	/** MTU value needed for the \ref dpni_drv_send() function */
	uint32_t            mtu;
	/** Parse Profile ID */
	uint8_t             prpid;
	/** \ref DPNI_DRV_DEFINES */
	uint8_t             flags;
	/* lock for multi-core support */
	uint8_t             dpni_lock;
	/** call back application function */
	rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
	/** call back application argument */
	dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
	/** connection for the command interface */
	struct dpni 		dpni;
};

void receive_cb(void);

#endif /* __DRV_H */
