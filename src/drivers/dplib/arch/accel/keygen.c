/**************************************************************************//**
@File		keygen.c

@Description	This file contains the AIOP SW Key Generation API implementation.

		Copyright 2013-2014 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_keygen.h"

#include "keygen.h"
#include "system.h"
#include "id_pool.h"

extern uint64_t ext_keyid_pool_address;

int32_t ctlu_kcr_builder_init(struct ctlu_kcr_builder *kb)
{
	/* clear NFEC (first byte in the KCR) */
	kb->kcr[CTLU_KCR_NFEC] = 0;
	/* Initialize KCR length to 1 */
	kb->kcr_length = 1;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_constant_fec(uint8_t constant, uint8_t num,
					       struct ctlu_kcr_builder *kb)
{

	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1;

	if ((curr_byte + CTLU_KCR_CONST_FEC_SIZE) > CTLU_KCR_MAX_KCR_SIZE)
		return CTLU_KCR_SIZE_ERR;

	if (num > 16)
		return CTLU_KCR_UDC_FEC_ERR;

	/* Build the FEC */
	/* User-defined FECID, no mask extension */
	fecid = CTLU_KCR_UDC_FECID << 1;
	/* OP0 = The number of times the user defined constant is repeated */
	op0 = (num-1);
	/* OP1 = User defined constant */
	op1 = constant;

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;

	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += 3;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_protocol_specific_field(enum
	kcr_builder_protocol_fecid protocol_fecid,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = CTLU_KCR_PROTOCOL_SPECIFIC_FEC_SIZE;
	uint8_t fecid;

	/* Build the FEC */
	/* Protocol Specific FECID, mask extension indication */
	fecid = (uint8_t)protocol_fecid << 1;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */

		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+7] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+5] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+3] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+4] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+3] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+2] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+1] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}
	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_protocol_based_generic_fec(
	enum kcr_builder_parse_result_offset pr_offset,
	uint8_t extract_offset, uint8_t extract_size,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = CTLU_KCR_PROTOCOL_BASED_GENERIC_FEC_SIZE;
	uint8_t fecid, op0, op1, op2;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	/* OP0 for Protocol based extraction */
	if (pr_offset > CTLU_KCR_PROTOCOL_MAX_OFFSET)
		return CTLU_KCR_PR_OFFSET_ERR;
	else
		op0 = CTLU_KCR_OP0_HET_PROTOCOL |
			  CTLU_KCR_PROTOCOL_HVT |
			  (uint8_t)pr_offset;

	/* OP1 = Extract Offset */
	if (extract_offset > CTLU_KCR_MAX_EXTRACT_OFFET)
		return CTLU_KCR_EXTRACT_OFFSET_ERR;
	else
		op1 = extract_offset;

	/* OP2 = Extract Size*/
	if (extract_size > CTLU_KCR_MAX_EXTRACT_SIZE)
		return CTLU_KCR_EXTRACT_SIZE_ERR;
	else
		op2 = extract_size;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_generic_extract_fec(uint8_t offset,
	uint8_t extract_size, uint32_t flags,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t aligned_offset, extract_offset;
	uint8_t	fec_bytes_num = CTLU_KCR_GENERIC_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	/* OP0 for General extraction */
	op0 = 0;
	aligned_offset = offset & CTLU_KCR_16_BYTES_ALIGNMENT;
	if (flags & CTLU_KCR_GEC_FRAME) {
		/*! Generic extraction from start of frame */
		switch (aligned_offset) {

		case (0x00):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x00;
			break;

		case (0x10):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x10;
			break;

		case (0x20):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x20;
			break;

		case (0x30):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x30;
			break;

		case (0x40):
			op0 = CTLU_KCR_OP0_HET_GEC |
			     CTLU_KCR_EOM_FRAME_OFFSET_0x40;
			break;

		case (0x50):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x50;
			break;

		case (0x60):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x60;
			break;

		case (0x70):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x70;
			break;

		case (0x80):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x80;
			break;

		case (0x90):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x90;
			break;

		case (0xA0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xA0;
			break;

		case (0xB0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xB0;
			break;

		case (0xC0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xC0;
			break;

		case (0xD0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xD0;
			break;

		case (0xE0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xE0;
			break;

		case (0xF0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xF0;
			break;

		default:
			break;

		} /* switch */
	} /* if */
	else if (flags & CTLU_KCR_GEC_PARSE_RES) {
		if (aligned_offset > 0x30)
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		else
		switch (aligned_offset) {

		case (0x00):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x00;
			break;

		case (0x10):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x10;
			break;

		case (0x20):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x20;
			break;

		case (0x30):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x30;
			break;

		default:
			break;
		}
	} else
		return CTLU_KCR_PROTOCOL_GEC_ERR;


	/* OP1 = Extract Offset */
	extract_offset = offset & CTLU_KCR_OFFSET_WITHIN_16_BYTES;
	if (extract_offset > CTLU_KCR_MAX_EXTRACT_OFFET)
		return CTLU_KCR_EXTRACT_OFFSET_ERR;
	else
		op1 = extract_offset;

	/* OP2 = Extract Size*/
	if (extract_size > CTLU_KCR_MAX_EXTRACT_SIZE)
		return CTLU_KCR_EXTRACT_SIZE_ERR;
	else
		op2 = extract_size;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}

int32_t ctlu_kcr_builder_add_lookup_result_field_fec(uint8_t extract_field,
	uint8_t offset_in_opaque, uint8_t extract_size_in_opaque,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t	fec_bytes_num = CTLU_KCR_LOOKUP_RES_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	switch (extract_field) {

	case (CTLU_KCR_EXT_OPAQUE0):
		if ((offset_in_opaque + extract_size_in_opaque) > 8) {
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		} else {
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x00;
		op1 = CTLU_KCR_EXT_OPAQUE0_OFFSET + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		}
		break;
	case (CTLU_KCR_EXT_OPAQUE1):
		if ((offset_in_opaque + extract_size_in_opaque) > 8) {
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		} else {
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x00;
		op1 = CTLU_KCR_EXT_OPAQUE1_OFFSET + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		}
		break;
	case (CTLU_KCR_EXT_OPAQUE2):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_OPAQUE2_OFFSET;
		op2 = CTLU_KCR_EXT_OPAQUE2_SIZE;
		break;
	/*case (CTLU_KCR_EXT_UNIQUE_ID):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_UNIQUE_ID_OFFSET;
		op2 = CTLU_KCR_EXT_UNIQUE_ID_SIZE;
		break;
	case (CTLU_KCR_EXT_TIMESTAMP):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_TIMESTAMP_OFFSET;
		op2 = CTLU_KCR_EXT_TIMESTAMP_SIZE;
		break;*/
	default:
		return CTLU_KCR_BUILDER_EXT_LOOKUP_RES_ERR;
		break;
	}


	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += 4 + mask_bytes;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


/* This function returns a deep copy of the key composition rule (kcr)
		created using the kcr builder. - OBSOLETE

int32_t ctlu_kcr_builder_get_kcr(struct ctlu_kcr_builder *kb, uint8_t *kcr)
{
	int i;

	/* Deep copy
	for (i = 0; i < CTLU_KCR_MAX_KCR_SIZE; i++)
		kcr[i] = kb->kcr[i];


	return CTLU_KCR_SUCCESSFUL_OPERATION;
}
*/
int32_t ctlu_kcr_builder_add_valid_field_fec(uint8_t mask,
					 struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	nmsk_moff0 = 0x00; /* indicates 1 bytes mask from offset 0x00 */
	uint8_t	fec_bytes_num = CTLU_KCR_VALID_FIELD_FEC_SIZE;

	if (mask) {
		fec_bytes_num = fec_bytes_num + 2;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		kb->kcr[curr_byte] = (CTLU_KCR_VF_FECID << 1) |
				     CTLU_KCR_MASK_EXT;
		kb->kcr[curr_byte+1] = 0x0;
		kb->kcr[curr_byte+2] = nmsk_moff0;
		kb->kcr[curr_byte+3] = mask;
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		kb->kcr[curr_byte] = CTLU_KCR_VF_FECID << 1;
		kb->kcr[curr_byte+1] = 0x0;
	}
	/* Build the FEC */
	/* Valid field FECID, mask extension indication*/

	/* Update kcr_builder struct */
	kb->kcr[0] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;

}


int32_t ctlu_kcr_create(enum table_hw_accel_id acc_id,
			uint8_t *kcr,
			uint8_t *keyid)
{
	int32_t status;
/*	uint16_t keyid_pool[SYS_KEYID_POOL_LENGTH];*/

	status = get_id(ext_keyid_pool_address, SYS_NUM_OF_PRPIDS, keyid);

	if (status != 0)		/* TODO check status ??? */
		return status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)(*keyid)) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status)
		return CTLU_KCR_CREATE_SUCCESS;
	else
		return CTLU_KCR_CREATE_STATUS_KCR_REPLACED;
}


int32_t ctlu_kcr_replace(enum table_hw_accel_id acc_id,
			 uint8_t *kcr,
			 uint8_t keyid)
{
	int32_t status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status) /* In this command status 0 means failure */
		return CTLU_KCR_REPLACE_FAIL_INVALID_KID;
	else
		return
			CTLU_KCR_REPLACE_SUCCESS;
}


int32_t ctlu_kcr_delete(enum table_hw_accel_id acc_id,
			uint8_t keyid)
{
	int32_t status;
/*	uint16_t keyid_pool[SYS_KEYID_POOL_LENGTH];*/
	uint8_t fake_kcr = 0;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)&fake_kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	if (!status)	/* In this command status 0 means failure */
		return CTLU_KCR_DELETE_FAIL_INVALID_KID;

	status = release_id(keyid, ext_keyid_pool_address);
	/*TODO check status ??? */
	return status;
}


int32_t ctlu_kcr_query(enum table_hw_accel_id acc_id,
		       uint8_t keyid, uint8_t *kcr,
		       uint8_t *size)
{
	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_QUERY_MTYPE, (uint32_t)kcr,
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	*size = *(((uint8_t *)HWC_ACC_OUT_ADDRESS) + 5);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_gen_key(enum table_hw_accel_id acc_id,
		     uint8_t keyid,
		     union ctlu_key *key,
		     uint8_t *key_size)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_GENERATE_EPRS_MTYPE, ((uint32_t)key),
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);


	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	*key_size = *(((uint8_t *)HWC_ACC_OUT_ADDRESS) + 5);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_gen_hash(union ctlu_key *key, uint8_t key_size, uint32_t *hash)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_HASH_GEN_KEY_MTYPE, ((uint32_t)key) << 16,
	       ((uint32_t)key_size) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(TABLE_ACCEL_ID_CTLU); /*TODO*/

	*hash = *(((uint32_t *)HWC_ACC_OUT_ADDRESS) + 1);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

