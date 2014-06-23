/**

 @File          platform_aiop_spec.h

 @Description   LS2100A Pltform external definitions and structures.
*//***************************************************************************/
#ifndef __FSL_PLATFORM_AIOP_SPEC_H
#define __FSL_PLATFORM_AIOP_SPEC_H


/**************************************************************************//**
 @Group         ls2100a_g LS2100A Application Programming Interface

 @Description   LS2100A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define PLATFORM_MAX_MEM_INFO_ENTRIES   8

/**************************************************************************//**
 @Description   Platform Console Types
*//***************************************************************************/
typedef enum platform_console_type {
    PLTFRM_CONSOLE_NONE,        /**< Do not use platform console */
    PLTFRM_CONSOLE_DUART        /**< Use DUART-x as console port */
} e_platform_console_type;

/**************************************************************************//**
 @Description   Platform Memory Regions
*//***************************************************************************/
typedef enum platform_mem_region {
    PLTFRM_MEM_RGN_NONE = 0,
    PLTFRM_MEM_RGN_DP_DDR,
    PLTFRM_MEM_RGN_SYSTEM_DDR,
    PLTFRM_MEM_RGN_SHRAM,
    PLTFRM_MEM_RGN_AIOP,
    PLTFRM_MEM_RGN_CCSR,
    PLTFRM_MEM_RGN_PEB,
    PLTFRM_MEM_RGN_MC_PORTALS
} e_platform_mem_region;


/**************************************************************************//**
 @Description   Platform configuration parameters structure
*//***************************************************************************/
struct platform_param {
    uint32_t                        clock_in_freq_hz;
    enum cache_mode                 l1_cache_mode;
    enum platform_console_type      console_type;
    uint8_t                         console_id;

    struct platform_memory_info     mem_info[PLATFORM_MAX_MEM_INFO_ENTRIES];
};

/** @} */ /* end of ls2100a_g group */



#endif /* __FSL_PLATFORM_AIOP_SPEC_H */

