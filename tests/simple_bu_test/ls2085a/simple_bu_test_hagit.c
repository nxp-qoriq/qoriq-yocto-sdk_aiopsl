/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

#include "fsl_types.h"
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "fsl_cdma.h"
#include "fsl_fdma.h"
#include "fsl_ip.h"
#include "fsl_l4.h"
#include "fsl_malloc.h"
#include "fsl_tman.h"
#include "fsl_slab.h"
#include "fsl_malloc.h"
#include "fsl_frame_operations.h"
#include "fsl_ipr.h"
#include "ipr.h"
#include "fsl_l2.h"
#include "fsl_table.h"
#include "fsl_keygen.h"

#include "simple_bu_test.h"

extern struct  ipr_global_parameters ipr_global_parameters1;
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

//#define FRAME_SIZE	124
#define FRAME_SIZE	158
#define	COPY_SIZE	16
#define AIOP_SP_BDI     0x00080000
#define SP_BP_PBS_MASK  0x3FFF

#define BU_TMI_BUF_ALIGN 64 /* TMI address alignment (64 bytes) */
/**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#define BU_TMI_ALIGN_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
        								(~(((uint64_t)(ALIGNMENT)) - 1)))
#define BU_TMI_ADDR_ALIGN(ADDRESS) \
	BU_TMI_ALIGN_64((ADDRESS), BU_TMI_BUF_ALIGN)
/* Aligned Descriptor Address (parameters area start) */
#define BU_TMI_ADDR(ADDRESS) BU_TMI_ADDR_ALIGN(ADDRESS)


int simple_bu_hagit_test(void)
{
	int        err  = 0;
	uint8_t prpid;


	fsl_print("Running simple bring-up test\n");
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;
	
	default_task_params.current_scope_level = 1;
	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;

	// run create_frame on default frame
	{
		struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;
/*
		uint8_t frame_data[FRAME_SIZE] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x08,0x00,0x45,0x00,\
				0x00,0x6e,0x00,0x00,0x00,0x00,0xff,0x11,0x3a,\
				0x26,0xc0,0x55,0x01,0x02,0xc0,0x00,0x00,0x01,\
				0x04,0x00,0x04,0x00,0x00,0x5a,0xff,0xff,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf5,0xd0,\
				0x64,0x51,0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,\
				0x6e,0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a};
*/
		
/*
		uint8_t frame_data[FRAME_SIZE] = { 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x10, 0x94, 0x00, 0x00, 0x02, 0x08, 0x00, 0x45, 0x00,\
				 0x00, 0x8a, 0x12, 0x34, 0x00, 0x00, 0xff, 0x32, 0x38, 0xc6, 0x45, 0xa4, 0xe1, 0x4c, 0xed, 0x03,\
				 0x5c, 0x45, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x6e, 0x00, 0x00,\
				 0x00, 0x00, 0xff, 0x11, 0x3a, 0x26, 0xc0, 0x55, 0x01, 0x02, 0xc0, 0x00, 0x00, 0x01, 0x04, 0x00,\
				 0x04, 0x00, 0x00, 0x4e, 0x05, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
				 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
				 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
				 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
				 0x00, 0x00, 0x00, 0x00, 0xf5, 0xd0, 0x64, 0x51, 0xac, 0x9f, 0x69, 0xd4, 0xd3, 0xf7, 0x39, 0x6e,\
				 0x20, 0x0e, 0x97, 0xb7, 0xe9, 0xe4, 0x56, 0x3a};
		
*/
		
		uint8_t frame_data[FRAME_SIZE] = {0x00, 0x1e, 0x0b, 0xaa, 0x95, 0xd3, 0x00, 0x04, 0x9f, 0x02, 0x7b, 0x2b, 0x08, 0x00, 0x45, 0x00,\
		  0x00, 0x90, 0x00, 0x4c, 0x00, 0x00, 0xff, 0x11, 0x21, 0xbc, 0xc0, 0xa8, 0x0d, 0x01, 0xc0, 0xa8,\
		  0x0b, 0x03, 0x11, 0x94, 0x11, 0x94, 0x00, 0x7c, 0x00, 0x00, 0xf2, 0x1f, 0x4a, 0x58, 0x00, 0x00,\
		  0x00, 0x4d, 0x1f, 0x79, 0x93, 0xe4, 0xaa, 0xfe, 0x5a, 0xd4, 0x01, 0x6f, 0x66, 0x03, 0x7f, 0xdc,\
		  0x8b, 0xbe, 0x9c, 0x1e, 0xfd, 0xd8, 0xb9, 0x8f, 0x35, 0x4f, 0x6e, 0xfc, 0x0b, 0xa6, 0x4d, 0xbf,\
		  0x68, 0x70, 0xc9, 0x4f, 0x8a, 0x11, 0x79, 0x1c, 0x2a, 0x8e, 0xb5, 0xd3, 0x79, 0x07, 0x17, 0xa1,\
		  0x1b, 0x1a, 0x87, 0xd6, 0x38, 0x50, 0xea, 0x41, 0xdf, 0x0f, 0xd8, 0x0b, 0x14, 0x16, 0x79, 0xb9,\
		  0x87, 0x74, 0xe4, 0x56, 0xda, 0xf3, 0x83, 0xdf, 0x0a, 0xc7, 0x04, 0x31, 0x78, 0x32, 0x77, 0xe6,\
		  0xef, 0xe3, 0xac, 0x0b, 0x19, 0x64, 0xdb, 0xa8, 0xf0, 0xd6, 0x08, 0x2d, 0xe0, 0xb8, 0xe7, 0x37,\
		  0x01, 0xf7, 0xe6, 0xcc, 0xb5, 0x09, 0x14, 0x06, 0x25, 0xb6, 0xb4, 0x9c, 0x89, 0x8e};
/*
		uint8_t frame_data_read[FRAME_SIZE+4] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x81,0x00,0xaa,0xbb,0x08,0x00,0x45,0x00,\
				0x00,0x6e,0x00,0x00,0x00,0x00,0xff,0x11,0x3a,\
				0x26,0xc0,0x55,0x01,0x02,0xc0,0x00,0x00,0x01,\
				0x04,0x00,0x04,0x00,0x00,0x5a,0xff,0xff,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf5,0xd0,\
				0x64,0x51,0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,\
				0x6e,0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a};
*/

		uint8_t frame_handle;
		uint32_t vlan = 0x8100aabb;
		//int parse_status;
		//uint8_t *frame_presented;
		//uint8_t *seg_addr;
		struct fdma_amq amq;
		uint16_t icid, flags = 0;
		uint8_t tmp;

		struct kcr_builder kb __attribute__((aligned(16)));
		uint8_t constant = 0x12;
		uint8_t num = 1;
		uint8_t keyid;
		uint8_t key[128] __attribute__((aligned(16)));
		uint8_t key_size;
		int i;

		keygen_kcr_builder_init(&kb);

		
		//uint32_t frame_length;
		/* setting SPID = 0 */
		*((uint8_t *)HWC_SPID_ADDRESS) = 0;
		icid = (uint16_t)(storage_profile[0].ip_secific_sp_info >> 48);
		icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
		tmp = (uint8_t)(storage_profile[0].ip_secific_sp_info >> 40);
		if (tmp & 0x08)
			flags |= FDMA_ICID_CONTEXT_BDI;
		if (tmp & 0x04)
			flags |= FDMA_ICID_CONTEXT_PL;
		if (storage_profile[0].mode_bits2 & sp1_mode_bits2_VA_MASK)
			flags |= FDMA_ICID_CONTEXT_VA;
		amq.icid = icid;
		amq.flags = flags;
		set_default_amq_attributes(&amq);
		*(uint32_t *)(&storage_profile[0].pbs2) = *(uint32_t *)(&storage_profile[0].pbs1);

		for (i=0; i<8 ; i++)
			fsl_print("storage profile arg %d: 0x%x \n", i, *((uint32_t *)(&(storage_profile[0]))+i));
		
		
		err = create_frame(fd, frame_data, FRAME_SIZE, &frame_handle);
		if (err)
			fsl_print("ERROR: create frame failed with err = %x!\n", err);

		fsl_print("parse result before create frame - \n");
		
		fsl_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
				
		fsl_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
			
/*
		fsl_print("udp offset %d %x\n", 
					PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());
*/

		for (i=0; i<16 ; i++)
			fsl_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));

		/* My Test */

		keygen_kcr_builder_init(&kb);
		err = keygen_kcr_builder_add_protocol_specific_field
				(KEYGEN_KCR_MACSRC_FECID,
						0,
						&kb);
		if (err) 
			fsl_print("ERROR: keygen_kcr_builder_add_protocol_specific_field failed!\n");

		err = keygen_kcr_builder_add_protocol_specific_field
				(KEYGEN_KCR_IPSRC_1_FECID,
						0,
						&kb);
		if (err) 
			fsl_print("ERROR: keygen_kcr_builder_add_protocol_specific_field failed!\n");

		err = keygen_kcr_builder_add_protocol_specific_field
				(KEYGEN_KCR_IPDST_1_FECID,
						0,
						&kb);
		if (err) 
			fsl_print("ERROR: keygen_kcr_builder_add_protocol_specific_field failed!\n");

		err = keygen_kcr_builder_add_valid_field_fec
				(0,
				&kb);
		if (err) 
			fsl_print("ERROR: keygen_kcr_builder_add_valid_field_fec failed!\n");

		err = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
					kb.kcr,
					&keyid);
		if (err) 
			fsl_print("ERROR: keygen_kcr_create failed!\n");
		else
			fsl_print("keyid = %d\n",keyid);
		err = keygen_gen_key(KEYGEN_ACCEL_ID_CTLU,
				     keyid,
				     0,
				     &key,
				     &key_size);
		if (err) 
			fsl_print("ERROR: keygen_gen_key failed!\n");
		else
		{
			for (i=0;i<128;i=i+4)
				fsl_print("key word %d = 0x%x\n",i, *(uint32_t *)(key+i));
			fsl_print("key size = %d\n", key_size);
		}

		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		
	}
	
	if (!err) {
		fsl_print("Simple bring-up test completed successfully\n");
	}
	
	return err;
}



