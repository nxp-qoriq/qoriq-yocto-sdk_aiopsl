/**************************************************************************//**
@File		fsl_ipsec.h

@Description	This file contains the AIOP IPSec API.
*//***************************************************************************/

#ifndef __FSL_IPSEC_H
#define __FSL_IPSEC_H

#include "common/types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_IPSEC IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		IPSEC_ENUM IPsec Enumerations

 @Description	IPsec Enumerations

 @{
*//***************************************************************************/

/**************************************************************************//**
 @enum ipsec_direction
 
 @Description   IPSEC Inbound/Outbound (Decap/Encap) Direction  
 
*************************************************************************/
enum ipsec_direction {
	IPSEC_DIRECTION_INBOUND = 1, 	/*!< Inbound Direction */
	IPSEC_DIRECTION_OUTBOUND = 2 	/*!< Outbound Direction */
};


/* @} end of IPSEC_ENUM */

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale IPsec Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPSec handle Type definition

*//***************************************************************************/
typedef uint64_t ipsec_handle_t;

/**************************************************************************//**
@Description	ipsec general flags

		Use for ipsec_descriptor_params.flags
*//***************************************************************************/

/** IPsec transport mode (default = tunnel mode) */
#define IPSEC_FLG_TRANSPORT_MODE		0x00000001

/** Enable Transport mode ESP pad check (default = no check) */
#define IPSEC_FLG_TRANSPORT_PAD_CHECK	0x00000002 

/** Frames do not include a L2 Header */
#define IPSEC_FLG_NO_L2_HEADER		0x00000010 

/** Lifetime KiloByte Counter Enable */
#define IPSEC_FLG_LIFETIME_KB_CNTR_EN	0x00000100 
/** Lifetime Packet counter Enable */
#define IPSEC_FLG_LIFETIME_PKT_CNTR_EN	0x00000200 
/** Lifetime Seconds counter Enable */
#define IPSEC_FLG_LIFETIME_SEC_CNTR_EN	0x00000400 

/** Preserve the ASA (Accelerator Specific Annotation) */
#define IPSEC_FLG_PRESERVE_ASA		0x00020000

/**************************************************************************//**
@Description	General IPSec ESP encap/decap options 

		Use for ipsec_encap/decap_params.options
*//***************************************************************************/
#define IPSEC_OPTS_ESP_ESN	0x10   /**< extended sequence included */

#define IPSEC_OPTS_ESP_IPVSN	0x02   /**< process an IPv6 header 
 	 	 	 	 	 Valid for transport mode only */

/**************************************************************************//**
@Description	IPSec ESP Encapsulation options 

		Use for ipsec_encap_params.options
*//***************************************************************************/
/** Perform computations to update header checksum for IPv4 header. 
 * Not valid for tunnel mode */
#define IPSEC_ENC_OPTS_UPDATE_CSUM 0x80   

/** Copy TOS field (IPv4) or Traffic-Class field (IPv6) from outer
 * IP header to inner IP header. Not valid for tunnel mode */
#define IPSEC_ENC_OPTS_DIFFSERV	0x40   

/** Generate random initial vector before starting encapsulation 
 * If set, the IV comes from an internal random generator */
#define IPSEC_ENC_OPTS_IVSRC	0x20   

/* Output IP header source options. Use one of the following options. 
 * Relevant for tunnel mode only */
#define IPSEC_ENC_OPTS_IPHDR_SRC_NONE	0x00 /* IP header not included */
#define IPSEC_ENC_OPTS_IPHDR_SRC_FRAME 	0x04 /* IP header from input frame */
#define IPSEC_ENC_OPTS_IPHDR_SRC_ADDR	0x08 /* IP header ref from parameters */
#define IPSEC_ENC_OPTS_IPHDR_SRC_PARAM	0x0c /* IP header from parameters */

/** NAT UDP Encapsulation enable. Tunnel mode only */
#define IPSEC_ENC_OPTS_NAT_EN        0x02 
/** NAT UDP checksum enable. Tunnel mode only */
#define IPSEC_ENC_OPTS_NUC_EN        0x01

/**************************************************************************//**
@Description	IPSec ESP Decapsulation options 

		Use for ipsec_decap_params.options
*//***************************************************************************/

/* Anti-replay window size. Use one of the following options. */
/* Note: the 128-entry size is for tunnel mode only */
#define IPSEC_DEC_OPTS_ARSNONE	0x00   /**< no anti-replay window */
#define IPSEC_DEC_OPTS_ARS32	0x40   /**< 32-entry anti-replay window */
#define IPSEC_DEC_OPTS_ARS128	0x80   /**< 128-entry anti-replay window */
#define IPSEC_DEC_OPTS_ARS64	0xc0   /**< 64-entry anti-replay window */

/** Perform checksum verification to IPv4 header in Transport mode. 
 * Not valid for tunnel mode */
#define IPSEC_DEC_OPTS_VERIFY_CSUM 0x20   /**< validate IP header checksum */ 

/** Enable Tunnel ECN according to RFC 6040
 * Valid for Tunnel mode*/
#define IPSEC_DEC_OPTS_TECN          0x20

/**************************************************************************//**
@Description	IPSec ESP Encapsulation HMO field  

		Use for ipsec_encap_params.hmo
*//***************************************************************************/

/** Copy the DF bit from the inner IP header to the outer IP header. */
#define IPSEC_HMO_ENCAP_DFC 	0x04

/** Decrement TTL field (IPv4) or Hop-Limit field (IPv6) within inner
 * IP header */
#define IPSEC_HMO_ENCAP_DTTL 	0x02                            

/* SNR: Sequence Number Rollover control 
 * If not set, a Sequence Number Rollover causes an error
 * if set, Sequence Number Rollover is permitted*/
#define IPSEC_HMO_ENCAP_SNR	0x01                             

/**************************************************************************//**
@Description	IPSec ESP Decapsulation HMO field  

		Use for ipsec_decap_params.hmo
*//***************************************************************************/

/* ODF: the DF bit in the IPv4 header in the output frame is replaced 
 * with the DFV value as shown below. 
 * Note: * Must not be set for IPv6 */
#define IPSEC_HMO_DECAP_ODF	0x08

/* DFV -- DF bit Value */
#define IPSEC_HMO_DECAP_DFV	0x04

/** Decrement TTL field (IPv4) or Hop-Limit field (IPv6) within inner
 * IP header */
#define IPSEC_HMO_DECAP_DTTL	0x02

/* DiffServ Copy
 * Copy the IPv4 TOS or IPv6 Traffic Class byte from the outer IP header 
 * to the inner IP header. */
#define IPSEC_HMO_DECAP_DSC	0x01

/**************************************************************************//**
@Description	IPSec Cipher Algorithms  

 To be set to the ipsec_descriptor_params.cipherdata.algtype field

*//***************************************************************************/
#define IPSEC_CIPHER_DES_IV64			0x0100
#define IPSEC_CIPHER_DES			0x0200
#define IPSEC_CIPHER_3DES			0x0300
#define IPSEC_CIPHER_NULL			0x0B00
#define IPSEC_CIPHER_AES_CBC			0x0c00
#define IPSEC_CIPHER_AES_CTR			0x0d00
#define IPSEC_CIPHER_AES_XTS			0x1600
#define IPSEC_CIPHER_AES_CCM8			0x0e00
#define IPSEC_CIPHER_AES_CCM12			0x0f00
#define IPSEC_CIPHER_AES_CCM16			0x1000
#define IPSEC_CIPHER_AES_GCM8			0x1200
#define IPSEC_CIPHER_AES_GCM12			0x1300
#define IPSEC_CIPHER_AES_GCM16			0x1400
#define IPSEC_CIPHER_AES_NULL_WITH_GMAC		0x1500

/**************************************************************************//**
@Description	IPSec Authentication Algorithms  

 To be set to the ipsec_descriptor_params.authdata.algtype field

*//***************************************************************************/
#define IPSEC_AUTH_HMAC_NULL			0x0000
#define IPSEC_AUTH_HMAC_MD5_96		 	0x0001
#define IPSEC_AUTH_HMAC_SHA1_96			0x0002
#define IPSEC_AUTH_AES_XCBC_MAC_96		0x0005
#define IPSEC_AUTH_HMAC_MD5_128			0x0006
#define IPSEC_AUTH_HMAC_SHA1_160		0x0007
#define IPSEC_AUTH_AES_CMAC_96			0x0008
#define IPSEC_AUTH_HMAC_SHA2_256_128		0x000c
#define IPSEC_AUTH_HMAC_SHA2_384_192		0x000d
#define IPSEC_AUTH_HMAC_SHA2_512_256		0x000e

/**************************************************************************//**
@Description	IPSec Key Encryption Flags  

 To be set to the alginfo.key_enc_flags field

*//***************************************************************************/

/* ENC: Encrypted - Key is encrypted either with the KEK, or
 * 	with the TDKEK if this descriptor is trusted */
#define IPSEC_KEY_ENC			0x00400000
/* NWB: No Write Back - Do not allow key to be FIFO STOREd */
#define IPSEC_KEY_NWB			0x00200000
/* EKT: Enhanced Encryption of Key */
#define IPSEC_KEY_EKT			0x00100000
/* TK: Encrypted with Trusted Key */
#define IPSEC_KEY_TK			0x00008000


/** @} */ /* end of FSL_IPSEC_MACROS */

/**************************************************************************//**
@Group		FSL_IPSEC_STRUCTS IPsec Structures

@Description	Freescale IPsec Structures

@{
*//***************************************************************************/

/**************************************************************************//**
 * @struct    ipsec_encap_cbc fsl_ipsec.h
 * @ingroup   ipsec_encap_params
 * @details   IV field for IPsec CBC encapsulation
*//***************************************************************************/
struct ipsec_encap_cbc {
	uint32_t iv[4];
};

/**************************************************************************//**
 * @struct    ipsec_encap_ctr fsl_ipsec.h
 * @ingroup   ipsec_encap_params
 * @details   Nonce and IV fields for IPsec CTR encapsulation
*//***************************************************************************/
struct ipsec_encap_ctr {
	uint32_t ctr_nonce;
	uint32_t ctr_initial;
	uint32_t iv[2];
};

/**************************************************************************//**
 * @struct    ipsec_encap_ccm fsl_ipsec.h
 * @ingroup   ipsec_encap_params
 * @details   Salt and IV fields for IPsec CCM encapsulation
*//***************************************************************************/
struct ipsec_encap_ccm {
	uint32_t salt; /* lower 24 bits */
	uint8_t b0_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial; /* Initial Counter */
	uint32_t iv[2];
};

/**************************************************************************//**
 * @struct    ipsec_encap_gcm fsl_ipsec.h
 * @ingroup   ipsec_encap_params
 * @details   Salt and IV fields for IPsec GCM encapsulation
*//***************************************************************************/
struct ipsec_encap_gcm {
	uint32_t salt; /* lower 24 bits */
	uint32_t iv[2];
};

/**************************************************************************//**
 * @struct    ipsec_encap_params fsl_ipsec.h
 * @ingroup   ipsec_encap_params
 * @details   Container for encapsulation parameters
*//***************************************************************************/
struct ipsec_encap_params {
	uint8_t hmo; /* Header Modification Options */
	uint8_t ip_nh; /** Next header value used for transport mode */
	uint8_t ip_nh_offset; /** Next header offset used for transport mode */
	uint8_t options;
	uint32_t seq_num_ext_hi;
	uint32_t seq_num;	/** Initial sequence number */
	union {
		struct ipsec_encap_cbc cbc;
		struct ipsec_encap_ctr ctr;
		struct ipsec_encap_ccm ccm;
		struct ipsec_encap_gcm gcm;
	};
	uint32_t spi; 	/** Security Parameter Index */
	uint16_t ip_hdr_len; /** IP header length */
	uint32_t *ip_hdr; /** optional IP Header content */
};


/**************************************************************************//**
 * @struct    ipsec_decap_ctr fsl_ipsec.h
 * @ingroup   ipsec_decap_params
 * @details   Salt and counter fields for IPsec CTR decapsulation
*//***************************************************************************/
struct ipsec_decap_ctr {
	uint32_t salt;
	uint32_t ctr_initial;
};

/**************************************************************************//**
 * @struct    ipsec_decap_ctr fsl_ipsec.h
 * @ingroup   ipsec_decap_params
 * @details   Salt, counter and flag fields for IPsec CCM decapsulation
*//***************************************************************************/
struct ipsec_decap_ccm {
	uint32_t salt;
	uint8_t iv_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
};

/**************************************************************************//**
 * @struct    ipsec_decap_gcm fsl_ipsec.h
 * @ingroup   ipsec_decap_params
 * @details   Salt field for IPsec GCM decapsulation
*//***************************************************************************/
struct ipsec_decap_gcm {
	uint32_t salt;
};

/**************************************************************************//**
 * @struct    ipsec_decap_params fsl_ipsec.h
 * @ingroup   ipsec_decap_params
 * @details   Container for decapsulation parameters
*//***************************************************************************/
struct ipsec_decap_params {
	uint16_t ip_hdr_len; /* The length, in bytes, of the portion of the 
				IP header that is not encrypted. */ 
	uint8_t ip_nh_offset; /* The location of the next header field within 
				the IP header of the transport mode packet. 
				The location is indicated by the number of 
				bytes from the beginning of the IP header. */
	uint8_t options;
	union {
		struct ipsec_decap_ctr ctr;
		struct ipsec_decap_ccm ccm;
		struct ipsec_decap_gcm gcm;
	};
	uint32_t seq_num_ext_hi; /* Extended sequence number */
	uint32_t seq_num; /* Sequence number */
};

/**************************************************************************//**
 * @struct    alginfo
 * @ingroup   alginfo
 * @details   Container for IPsec algorithm details
*//***************************************************************************/
struct alginfo {
	uint32_t algtype;  /**< Algorithm selector. */
	uint64_t key;      /**< Address where algorithm key resides */
	uint32_t keylen;   /**< Length of the provided key, in bytes */
	uint32_t key_enc_flags; /**< Key encryption flags 
				ENC, EKT, TK, NWB */
};

/**************************************************************************//**
 * @struct    ipsec_storage_params
 * @ingroup   ipsec_storage_params
 * @details   Container for IPsec descriptor storage parameters
*//***************************************************************************/
struct ipsec_storage_params {
	uint16_t sdid; /** Security domain ID */
	uint16_t spid; /** Storage profile ID of SEC output frame */
	uint8_t rife; /** Return input frame on error */
	uint8_t crid; /** Critical resource ID */
};

/**************************************************************************//**
 @Description   IPsec Descriptor Parameters
*//***************************************************************************/
struct ipsec_descriptor_params {
	
	/* Descriptor direction. Use IPSEC_DIRECTION_<INBOUND/OUTBOUND> */
	enum ipsec_direction ipsec_direction; 
	
	uint32_t flags; /* Miscellaneous control flags flags */
	
	union {
		struct ipsec_encap_params *encparams;
		struct ipsec_decap_params *decparams;
	};

	struct alginfo *cipherdata; /* cipher algorithm information */
	struct alginfo *authdata; /* authentication algorithm information */
			
	/* Lifetime Limits */
	/* Set to NULL to disable specific limits */
	uint32_t soft_kilobytes_limit;	/*!< Soft Kilobytes limit. */
	uint32_t hard_kilobytes_limit; 	/*!< Hard Kilobytes limit. */
	uint64_t soft_packet_limit; 	/*!< Soft Packet count limit. */
	uint64_t hard_packet_limit;	/*!< Hard Packet count limit. */
	uint32_t soft_seconds_limit;	/*!< Soft Seconds limit. */
	uint32_t hard_seconds_limit; 	/*!< Hard Second limit. */
	
	/* Callback function. 
	 * Invoked when the Soft Seconds timer reaches the limit value */
	/* Set to NULL to disable this option*/
	int (*soft_seconds_callback)(uint64_t); 

	struct ipsec_storage_params ipsec_storage_params;
};


/** @} */ /* end of FSL_IPSEC_STRUCTS */


/**************************************************************************//**
@Group		FSL_IPSEC_Functions IPsec Functions

@Description	Freescale AIOP IPsec Functions

@{
*//***************************************************************************/


/**************************************************************************//**
@Function	ipsec_add_sa_descriptor

@Description	This function performs add SA for encapsulation:
		creating the IPsec flow context and the Shared Descriptor.

		Implicit Input: BPID in the SRAM.
@Param[in]	ipsec_descriptor_params - descriptor parameters 

@Param[out]	ipsec_handle - IPsec handle to the descriptor datbase 
		
@Return		pass/error status. //TODO: add codes

*//****************************************************************************/
int32_t ipsec_add_sa_descriptor(
		struct ipsec_descriptor_params *ipsec_descriptor_params,
		ipsec_handle_t *ipsec_handle);


/**************************************************************************//**
@Function	ipsec_remove_sa_descriptor

@Description	This function performs buffer deallocation of the IPsec handler.

		Implicit Input: BPID in the SRAM.

@Param[in]	ipsec_handle - descriptor handle.

@Return		TODO

*//****************************************************************************/
int32_t ipsec_remove_sa_descriptor(ipsec_handle_t ipsec_handle);

/**************************************************************************//**
@Function	ipsec_get_lifetime_stats

@Description	This function returns SA lifetime counters:
		kilobyte, packets and seconds.

@Param[in]	ipsec_handle - IPsec handle.
@Param[out]	kilobytes - number of bytes processed by this SA.
@Param[out]	packets - number of packets processed by this SA.
@Param[out]	sec - number of seconds passed since this SA was created.

@Return		Status

*//****************************************************************************/
int32_t ipsec_get_lifetime_stats(
		ipsec_handle_t ipsec_handle,
		uint32_t *kilobytes,
		uint64_t *packets,
		uint32_t *sec);

/**************************************************************************//**
@Function	ipsec_decr_lifetime_counters

@Description	This function returns SA lifetime counters:
		kilobyte, packets and seconds.

@Param[in]	ipsec_handle - IPsec handle.
@Param[in]	kilobytes_decr_val - number of bytes to decrement from 
		the kilobytes counter of this SA.
@Param[in]	packets_decr_val - number of packets to decrement from
		the packets counter of this SA.

@Return		Status

*//****************************************************************************/
int32_t ipsec_decr_lifetime_counters(
		ipsec_handle_t ipsec_handle,
		uint32_t kilobytes_decr_val,
		uint32_t packets_decr_val
		);

/**************************************************************************//**
@Function	ipsec_get_seq_num

@Description	This function returns the following information:
		- Sequence number.
		- Extended sequence number (if exists).
		- Anti-replay bitmap (scorecard) (if exists).
		

@Param[in]	ipsec_handle - IPsec handle.
@Param[out]	sequence_number - Sequence number.
@Param[out]	extended_sequence_number - Extended sequence number.
@Param[out]	anti_replay_bitmap - Anti-replay bitmap. 4 words. 
		* For 32-entry only the first 32 bit word is valid.
		* For 64-entry only the first two 32 bit words are valid.
		* For 128-entry all four words are valid.

@Return		Status

*//****************************************************************************/
int32_t ipsec_get_seq_num(
		ipsec_handle_t ipsec_handle,
		uint32_t *sequence_number,
		uint32_t *extended_sequence_number,
		uint32_t anti_replay_bitmap[4]);

/**************************************************************************//**
@Function	ipsec_frame_decrypt

@Description	This function performs the decryption and the required IPsec
		protocol changes (according to RFC4303). This function also
		handles UDP encapsulated IPsec packets according to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the decrypted frame parser result and
		checks the inner UDP checksum (if available).

@Param[in]	ipsec_handle - IPsec handle.

@Return		Status 

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - It is assumed that IPv6 ESP extension is the last IPv6
		extension in the packet.
		 - This function does not support input frames which are IPv6
		jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_decrypt(ipsec_handle_t ipsec_handle);

/**************************************************************************//**
@Function	ipsec_frame_encrypt

@Description	This function performs the encryption and the required IPsec
		protocol changes (according to RFC4303).  This function
		enables UDP encapsulation of the IPsec packets according
		to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the encrypted frame parser result.

@Param[in]	ipsec_handle - IPsec handle.

@Return		Status

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - This function does not support encrypted frames which are
		IPv6 jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_encrypt(ipsec_handle_t ipsec_handle);

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


#endif /* __FSL_IPSEC_H */
