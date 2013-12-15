/**************************************************************************//**
 @File          fsl_gro.h

 @Description   This file contains the AIOP SW TCP GRO API
*//***************************************************************************/
#ifndef __FSL_GRO_H
#define __FSL_GRO_H

#include "common/types.h"


/**************************************************************************//**
@Group		FSL_AIOP_GRO FSL AIOP GRO

@Description	FSL AIOP GRO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	GRO_MACROS GRO Macros

@Description	GRO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	GRO_GENERAL_DEFINITIONS GRO General Definitions

 @Description GRO General Definitions.

 @{
*//***************************************************************************/

	/** TCP GRO context size definition. */
#define TCP_GRO_CONTEXT_SIZE	128
	/** TCP GRO context definition. */
typedef uint8_t tcp_gro_ctx_t[TCP_GRO_CONTEXT_SIZE];



/**************************************************************************//**
@Description	User callback function, called after aggregation timeout is
		expired.
		The user provides this function and the GRO process invokes it.

@Param[in]	arg - Address (in HW buffers) of the argument to the callback
		function.

 *//***************************************************************************/
typedef void (gro_timeout_cb_t)(uint64_t arg);

/** @} */ /* end of GRO_GENERAL_DEFINITIONS */

/**************************************************************************//**
 @Group	GRO_FLAGS GRO Flags

 @Description GRO Flags.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GRO_AGG_FLAGS TCP GRO Aggregation Flags

 @Description Flags for \ref tcp_gro_aggregate_seg() function.

 @{
*//***************************************************************************/

	/** GRO no flags indication. */
#define TCP_GRO_NO_FLAGS		0x00000000
	/** If set, tcp_gro_aggregate_seg() is called for the first time for
	 * this session . */
#define TCP_GRO_NEW_SESSION		0x00000001
	/** If set, extended statistics is enabled.	*/
#define TCP_GRO_EXTENDED_STATS_EN	0x00000002
	/** If set, save the segment sizes in the metadata. */
#define TCP_GRO_METADATA_SEGMENT_SIZES	0x00000004
	/** If set, calculate TCP and IP checksum. */
#define TCP_GRO_CALCULATE_CHECKSUM	0x00000008


/** @} */ /* end of TCP_GRO_AGG_FLAGS */

/** @} */ /* end of TCP_GRO_FLAGS */

/**************************************************************************//**
@Group	TCP_GRO_AGGREGATE_STATUS  TCP GRO Aggregation Statuses

@Description \ref tcp_gro_aggregate_seg() return values

@{
*//***************************************************************************/

	/** A segment was aggregated and the aggregation is not completed. */
#define	TCP_GRO_SEG_AGG_NOT_DONE	(GRO_MODULE_STATUS_ID | 0x1)
	/** A segment was aggregated and the aggregation is completed. */
#define	TCP_GRO_SEG_AGG_DONE		(GRO_MODULE_STATUS_ID | 0x2)
	/** A segment has started new aggregation, and the previous aggregation
	 * is completed. */
#define	TCP_GRO_SEG_AGG_DONE_AGG_OPEN	(GRO_MODULE_STATUS_ID | 0x3)

/** @} */ /* end of TCP_GRO_AGGREGATE_STATUS */

/**************************************************************************//**
@Group	TCP_GRO_FLUSH_STATUS  TCP GRO Flush Statuses

@Description \ref tcp_gro_flush_aggregation() return values

@{
*//***************************************************************************/

	/** The aggregation is flushed. */
#define	TCP_GRO_FLUSH_AGG_DONE	SUCCESS
	/** No aggregation exists for the session. */
#define	TCP_GRO_FLUSH_NO_AGG	(GRO_MODULE_STATUS_ID | 0x1)

/** @} */ /* end of TCP_GRO_FLUSH_STATUS */

/** @} */ /* end of GRO_MACROS */

/**************************************************************************//**
@Group		GRO_STRUCTS GRO Structures

@Description	AIOP GRO Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO statistics counters.
*//***************************************************************************/
struct tcp_gro_stats_cntrs {
		/** Counts the number of aggregated packets. */
	uint32_t	agg_num_cntr;
		/** Counts the number of aggregated segments. */
	uint32_t	seg_num_cntr;
		/** Counts the number of aggregations due to timeout limit.
		 * This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_timeout_cntr;
		/** Counts the number of aggregations due to segment count
		 * limit. This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_max_seg_num_cntr;
		/** Counts the number of aggregations due to max aggregated
		 * packet size limit. This counter is valid when extended
		 * statistics mode is enabled (\ref
		 * TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_max_packet_size_cntr;
		/** Counts the number of segments in which the sequence number
		 * is not expected. This counter is valid when extended
		 * statistics mode is enabled (\ref TCP_GRO_EXTENDED_STATS_EN)*/
	uint32_t	unexpected_seq_num_cntr;
};

/**************************************************************************//**
@Description	TCP GRO packet metadata.
*//***************************************************************************/
struct tcp_gro_context_metadata {
		/** Address (in HW buffers) of the segment sizes. This field
		 * will be updated if \ref TCP_GRO_METADATA_SEGMENT_SIZES is
		 * set. */
	uint64_t seg_sizes_addr;
		/** Number of segments in the aggregation. */
	uint16_t seg_num;
		/** Largest segment size*/
	uint16_t max_seg_size;
		/** Padding */
	uint8_t	pad[4];
};

/**************************************************************************//**
@Description	GRO aggregation limits.
*//***************************************************************************/
struct gro_context_limits {
		/** Timeout per packet aggregation limit. */
	uint16_t timeout_limit;
		/** Maximum aggregated packet size limit. */
	uint16_t packet_size_limit;
		/** Maximum aggregated segments per packet limit. */
	uint8_t	seg_num_limit;
		/** Padding */
	uint8_t	pad[3];

};

/**************************************************************************//**
@Description	GRO aggregation Timeout Parameters.
*//***************************************************************************/
struct gro_context_timeout_params {
		/** Address (in HW buffers) of the callback function parameter
		 * argument on timeout.
		 * On timeout, GRO will call upper layer callback function with
		 * this parameter. */
	uint64_t gro_timeout_cb_arg;
		/** Function to call upon Time Out occurrence.
		 * This function takes one argument. */
	gro_timeout_cb_t *gro_timeout_cb;
		/** TMAN Instance ID. */
	uint8_t tmi_id;
		/** Padding */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	TCP GRO aggregation parameters.
*//***************************************************************************/
struct tcp_gro_context_params {
		/** TCP GRO aggregation metadata. */
	struct tcp_gro_context_metadata metadata;
		/** TCP GRO aggregation timeout parameters. */
	struct gro_context_timeout_params timeout_params;
		/** Aggregated packet limits. */
	struct gro_context_limits limits;
		/** Address (in HW buffers) of the TCP GRO statistics counters
		 *  (\ref tcp_gro_stats_cntrs). 
		 *  The user should zero the statistics once it is allocated. */
	uint64_t stats_addr;
};


/** @} */ /* end of GRO_STRUCTS */

/**************************************************************************//**
@Group		GRO_FUNCTIONS TCP GRO Functions

@Description	GRO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gro_aggregate_seg

@Description	This function aggregates TCP segments to one TCP
		packet.

		When the aggregation is completed the aggregated packet will be
		located at the default frame location in the workspace.

		Pre-condition - The segment to be aggregated should be located
		in the default frame location in workspace.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context. The user should allocate \ref tcp_gro_ctx_t in
		this address.
@Param[in]	params - Pointer to the aggregated packet metadata.
@Param[in]	flags - Please refer to \ref TCP_GRO_AGG_FLAGS.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	None.
*//***************************************************************************/
int32_t tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags);


/**************************************************************************//**
@Function	tcp_gro_flush_aggregation

@Description	This function flush an open TCP packet
		aggregation.

		The aggregated packet will reside at the default frame location
		when this function returns.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context. The user should allocate \ref tcp_gro_ctx_t in
		this address.

@Return		Status, please refer to \ref TCP_GRO_FLUSH_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	No frame should reside at the default frame location in
		workspace before this function is called.
*//***************************************************************************/
int32_t tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr);

/** @} */ /* end of GRO_Functions */
/** @} */ /* end of FSL_AIOP_GRO */


#endif /* __FSL_GRO_H */
