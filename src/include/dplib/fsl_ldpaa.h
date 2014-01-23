/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_ldpaa.h

 @Description   This file contains LDPAA general definitions.
 *//***************************************************************************/
#ifndef __FSL_LDPAA_H
#define __FSL_LDPAA_H

#include "common/types.h"


/**************************************************************************//**
 @Group		AIOP_General_Definitionsa AIOP SL General Definitions

 @Description	AIOP General Definitions

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group         ldpaa_g  LDPAA API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Collection    LDPAA FD fields

 @{
*//***************************************************************************/

	/** FD length mask - without memory footprint */
#define FD_LENGTH_MASK		0xFFFFFFFF
	/** FD length mask - with memory footprint */
#define FD_LENGTH_MEM_MASK	0x0003FFFF
	/** FD Memory Footprint mask */
#define FD_MEM_MASK		0xFFF00000
	/** FD Bypass Memory Translation mask */
#define FD_BMT_MASK		0x80
	/** FD Invalid Pool ID mask */
#define FD_IVP_MASK		0x40
	/** FD Buffer Pool ID mask */
#define FD_BPID_MASK		0x3FFF
	/** FD Short Length mask */
#define FD_SL_MASK		0x40
	/** FD Format mask */
#define FD_FMT_MASK		0x30
	/** FD Data Offset mask */
#define FD_OFFSET_MASK		0x0FFF
	/** FD Virtual Address mask */
#define FD_VA_MASK		0x40
	/** FD Flow Context Bypass Memory Translation mask */
#define FD_CBMT_MASK		0x80
	/** FD Accelerator Specific Annotation Length mask */
#define FD_ASAL_MASK		0x0F
	/** FD Pass-Through annotation Valid 2  mask */
#define FD_PTV2_MASK		0x20
	/** FD Pass-Through annotation Valid 1  mask */
#define FD_PTV1_MASK		0x40
	/** FD Pass Through Annotation mask */
#define FD_PTA_MASK		0x80
	/** FD Drop Priority mask */
#define FD_DROPP_MASK		0x07
	/** FD Stashing Control mask */
#define FD_SC_MASK		0x08
	/** FD Dynamic Debug Mark mask */
#define FD_DD_MASK		0xF0
	/** FD Context Stashing amount mask */
#define FD_CS_MASK		0x0000000000000003
	/** FD Annotation Stashing amount mask */
#define FD_DS_MASK		0x0000000000000030
	/** FD Data Stashing amount mask */
#define FD_AS_MASK		0x000000000000000C
	/** FD Flow Context mask (with stashing) */
#define FD_FLC_STASH_MASK	0xFFFFFFFFFFFFFFC0
	/** FD Flow Context mask (without stashing) */
#define FD_FLC_NO_STASH_MASK	0xFFFFFFFFFFFFFFFF

/* Frame Descriptor (FD) Fields Offsets */
	/** FD address offset */
#define FD_ADDR_OFFSET		0x0
	/** FD mem_length offset */
#define FD_MEM_LENGTH_OFFSET	0x8
	/** FD bpid offset */
#define FD_BPID_OFFSET		0xC
	/** FD bmp_ivp offset */
#define FD_BMP_IVP_OFFSET	0xD
	/** FD offset offset */
#define FD_OFFSET_OFFSET	0xE
	/** FD sl_fmt offset */
#define FD_SL_FMT_OFFSET	0xF
	/** FD Frame Context offset */
#define FD_FRC_OFFSET		0x10
	/** FD Frame Errors offset */
#define FD_ERR_OFFSET		0x14
	/** FD cbmt_va offset */
#define FD_CBMT_VA_OFFSET	0x15
	/** FD pta_pvt_asa offset */
#define FD_PTA_PVT_ASA_OFFSET	0x16
	/** FD dd_sc_dropp offset */
#define FD_DD_SC_DROPP_OFFSET	0x17
	/** FD dd_sc_dropp offset */
#define FD_FLC_DS_AS_CS_OFFSET	0x18

/* Frame Descriptor (FD) Fields Shifts */
	/** FD MEM shift */
#define FD_MEM_SHIFT		(32-12)
	/** FD IVP shift */
#define FD_IVP_SHIFT		(8-2)
	/** FD BMT shift */
#define FD_BMT_SHIFT		(8-1)
	/** FD FMT shift */
#define FD_FMT_SHIFT		(8-4)
	/** FD SL shift */
#define FD_SL_SHIFT		(8-2)
	/** FD VA shift */
#define FD_VA_SHIFT		(8-2)
	/** FD CBMT shift */
#define FD_CBMT_SHIFT		(8-1)
	/** FD PTV2 shift */
#define FD_PTV2_SHIFT		(8-3)
	/** FD PTV1 shift */
#define FD_PTV1_SHIFT		(8-2)
	/** FD PTA shift */
#define FD_PTA_SHIFT		(8-1)
	/** FD SC shift */
#define FD_SC_SHIFT		(8-5)
	/** FD DD shift */
#define FD_DD_SHIFT		(8-4)
	/** FD AS shift */
#define FD_AS_SHIFT		(8-6)
	/** FD DS shift */
#define FD_DS_SHIFT		(8-4)

/* @} *//* end of LDPAA FD fields */


/**************************************************************************//**
 @Description	Frame Descriptor structure.

 The Frame Descriptor (FD) includes information related to the
 frame, such as frame format, the amount of frame data, presence
 of an annotation section containing frame meta-data.
 *//***************************************************************************/
struct ldpaa_fd {
	uint64_t addr; /**< Buffer address 	 */
	uint32_t length; /**< Frame data length 	 */
	uint32_t offset; /**< Frame data offset 	 */
	uint32_t frc; /**< Frame Context 	 */
	uint32_t control; /**< Fame control fields */
	uint64_t flc; /**< Flow Context 	 */
};


/**************************************************************************//**
 @Collection    LDPAA FD GETTER/SETTER MACROs

 @{
*//***************************************************************************/

/* FD fields Getters and Setters */
#define LDPAA_FD_GET_ADDR(_fd)
/**< Macro to get FD ADDRESS field.*/
/* Todo - 64/49 bit address. Phys to Virt? */
#define LDPAA_FD_SET_ADDR(_fd,_val)
/**< Macro to set FD ADDRESS field */
/* Todo - 64/49 bit address. Phys to Virt? */
#define LDPAA_FD_GET_LENGTH(_fd)
/**< Macro to get FD LENGTH field */
#define LDPAA_FD_SET_LENGTH(_fd,_val)
/**< Macro to set FD LENGTH field */
#define LDPAA_FD_GET_MEM(_fd)
/**< Macro to get FD MEM field */
#define LDPAA_FD_SET_MEM(_fd,_val)
/**< Macro to set FD MEM field */
#define LDPAA_FD_GET_BPID(_fd)
/**< Macro to get FD BPID field */
#define LDPAA_FD_SET_BPID(_fd,_val)
/**< Macro to set FD BPID field */
#define LDPAA_FD_GET_IVP(_fd)
/**< Macro to get FD IVP field */
#define LDPAA_FD_SET_IVP(_fd,_val)
/**< Macro to set FD IVP field */
#define LDPAA_FD_GET_BMT(_fd)
/**< Macro to get FD BMT field */
#define LDPAA_FD_SET_BMT(_fd,_val)
/**< Macro to set FD BMT field */
#define LDPAA_FD_GET_OFFSET(_fd)
/**< Macro to get FD OFFSET field */
#define LDPAA_FD_SET_OFFSET(_fd,_val)
/**< Macro to set FD OFFSET field */
#define LDPAA_FD_GET_FMT(_fd)
/**< Macro to get FD FMT field */
#define LDPAA_FD_SET_FMT(_fd,_val)
/**< Macro to set FD FMT field */
#define LDPAA_FD_GET_SL(_fd)
/**< Macro to get FD SL field */
#define LDPAA_FD_SET_SL(_fd,_val)
/**< Macro to set FD SL field */
#define LDPAA_FD_GET_FRC(_fd)
/**< Macro to get FD FRC field */
#define LDPAA_FD_SET_FRC(_fd,_val)
/**< Macro to set FD FRC field */
#define LDPAA_FD_GET_ERR(_fd)
/**< Macro to get FD ERR field */
#define LDPAA_FD_SET_ERR(_fd,_val)
/**< Macro to set FD ERR field */
#define LDPAA_FD_GET_VA(_fd)
/**< Macro to get FD VA field */
#define LDPAA_FD_SET_VA(_fd,_val)
/**< Macro to set FD VA field */
#define LDPAA_FD_GET_CBMT(_fd)
/**< Macro to get FD CBMT field */
#define LDPAA_FD_SET_CBMT(_fd,_val)
/**< Macro to set FD CBMT field */
#define LDPAA_FD_GET_ASAL(_fd)
/**< Macro to get FD ASAL field */
#define LDPAA_FD_SET_ASAL(_fd,_val)
/**< Macro to set FD ASAL field */
#define LDPAA_FD_GET_PTV2(_fd)
/**< Macro to get FD PTV2 field */
#define LDPAA_FD_SET_PTV2(_fd,_val)
/**< Macro to set FD PTV2 field */
#define LDPAA_FD_GET_PTV1(_fd)
/**< Macro to get FD PTV1 field */
#define LDPAA_FD_SET_PTV1(_fd,_val)
/**< Macro to set FD PTV1 field */
#define LDPAA_FD_GET_PTA(_fd)
/**< Macro to get FD PTA field */
#define LDPAA_FD_SET_PTA(_fd,_val)
/**< Macro to set FD PTA field */
#define LDPAA_FD_GET_DROPP(_fd)
/**< Macro to get FD DROPP field */
#define LDPAA_FD_SET_DROPP(_fd,_val)
/**< Macro to set FD DROPP field */
#define LDPAA_FD_GET_SC(_fd)
/**< Macro to get FD SC field */
#define LDPAA_FD_SET_SC(_fd,_val)
/**< Macro to set FD SC field */
#define LDPAA_FD_GET_DD(_fd)
/**< Macro to get FD DD field */
#define LDPAA_FD_SET_DD(_fd,_val)
/**< Macro to set FD DD field */
#define LDPAA_FD_GET_CS(_fd)
/**< Macro to get FD CS field */
#define LDPAA_FD_SET_CS(_fd,_val)
/**< Macro to set FD CS field */
#define LDPAA_FD_GET_AS(_fd)
/**< Macro to get FD AS field */
#define LDPAA_FD_SET_AS(_fd,_val)
/**< Macro to set FD AS field */
#define LDPAA_FD_GET_DS(_fd)
/**< Macro to get FD DS field */
#define LDPAA_FD_SET_DS(_fd,_val)
/**< Macro to set FD DS field */
#define LDPAA_FD_GET_FLC(_fd)
/**< Macro to get FD FLC field */
#define LDPAA_FD_SET_FLC(_fd,_val)
/**< Macro to set FD FLC field */

/* @} *//* end of LDPAA FD MACROs */

/** @} *//* end of ldpaa_g group */


#if defined(AIOP)
#include "arch/fsl_ldpaa_aiop.h"
#elif defined(MC)
#include "arch/fsl_ldpaa_mc.h"
#elif defined(GPP)
#include "arch/fsl_ldpaa_gpp.h"
#else
#error "SoC subsystem not defined!"
#endif /* defined(AIOP) */

/** @} */ /* end of AIOP_General_Definitions */

#endif /* __FSL_LDPAA_H */
