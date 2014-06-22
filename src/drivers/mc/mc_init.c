#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "errors.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpci.h"
#include "fsl_mc_init.h"

int mc_obj_init();
void mc_obj_free();

static int aiop_container_init()
{
	void *p_vaddr;
	int err = 0;
	int i = 0;
	int container_id;
	struct dprc *dprc = fsl_os_xmalloc(sizeof(struct dprc), 
	                                   MEM_PART_SH_RAM, 
	                                   1);
	if (dprc == NULL) {
		pr_err("No memory for AIOP Root Container \n");
		return -ENOMEM;		
	}
	memset(dprc, 0, sizeof(struct dprc));
	
	/* TODO: replace hard-coded portal address 1 with configured value */
	/* TODO : layout file must contain portal ID 1 in order to work. */
	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	p_vaddr = \
	UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
    	                             (uint32_t)1, E_MAPPED_MEM_TYPE_MC_PORTAL));

	/* Open root container in order to create and query for devices */
	dprc->regs = p_vaddr;
	if ((err = dprc_get_container_id(dprc, &container_id)) != 0) {
		pr_err("Failed to get AIOP root container ID.\n");
		return err;
	}
	if ((err = dprc_open(dprc, container_id)) != 0) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n", 
		       container_id);
		return err;
	}
	
	err = sys_add_handle(dprc, FSL_OS_MOD_AIOP_RC, 1, 0);
	return err;
}

static void aiop_container_free()
{
	void *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	
	sys_remove_handle(FSL_OS_MOD_AIOP_RC, 0);
	
	if (dprc != NULL)
		fsl_os_xfree(dprc);		
}


static int dpci_discovery()
{
	int dev_count;
	struct dprc_obj_desc dev_desc;
	int err = 0;
	int i = 0;
	uint8_t p = 0;;
	struct dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpci *dpci;
	struct dpci_dest_cfg dest_cfg;
	struct dpci_obj *dpci_tbl = NULL;
	int ind = 0;
	int link_up = 0;
	
	if ((err = dprc_get_obj_count(dprc, &dev_count)) != 0) {
	    pr_err("Failed to get device count for RC auth_d = %d\n", 
	           dprc->auth);
	    return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {			
			pr_debug(" Found DPCI device\n");
			pr_debug("***********\n");
			pr_debug("vendor - %x\n", dev_desc.vendor);
			pr_debug("type - %s\n", dev_desc.type);
			pr_debug("id - %d\n", dev_desc.id);
			pr_debug("region_count - %d\n", dev_desc.region_count);
			pr_debug("state - %d\n", dev_desc.state);
			pr_debug("ver_major - %d\n", dev_desc.ver_major);
			pr_debug("ver_minor - %d\n", dev_desc.ver_minor);
			pr_debug("irq_count - %d\n\n", dev_desc.irq_count);
			
#ifndef OLD_DPCI
			if (dpci_tbl == NULL) {
				dpci_tbl = fsl_os_xmalloc(sizeof(struct dpci_obj), 
				                          MEM_PART_SH_RAM, 
					                  1);
				if (dpci_tbl == NULL) {
					pr_err("No memory for %d DPCIs\n", 
					       DPCI_OBJ_MN);
					return -ENOMEM;
				}
				err = sys_add_handle(dpci_tbl, 
				                     FSL_OS_MOD_DPCI_TBL, 
				                     1, 
				                     0);
				if (err != 0) {
					pr_err("No FSL_OS_MOD_DPCI_TBL\n");
					return err;					
				}
					
			}
			
			dpci = &dpci_tbl->dpci[ind];			
			dpci->regs = dprc->regs;
			err |= dpci_open(dpci, dev_desc.id);
			/* Set priorities 0 and 1 */
			dest_cfg.type = DPCI_DEST_NONE;
			for (p = 0; p < 2; p++) {
				dest_cfg.priority = p + 1;
				err |= dpci_set_rx_queue(dpci, 
				                         p, 
				                         &dest_cfg, 
				                         (ind << 1) | p);				
			}
			err |= dpci_enable(dpci);
			err |= dpci_get_attributes(dpci, &dpci_tbl->attr[ind]);
			if (err) {
				pr_err("Failed dpci initialization \n");
				return -ENODEV;
			}
			err = dpci_get_link_state(&dpci_tbl->dpci[ind], 
			                          &link_up);
			if (err || !link_up) {
				pr_warn("Failed to get DPCI link is not up\n");
				/* TODO return -EACCES; 
				 * There is bug in get link state therefore 
				 * meanwhile just notify about it */
			}

			dpci_tbl->count++;			
#endif			
			ind++;
		}
	}

	return err;
}

static void dpci_discovery_free()
{
	void *dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	
	sys_remove_handle(FSL_OS_MOD_DPCI_TBL, 0);
	
	if (dpci_tbl != NULL)
		fsl_os_xfree(dpci_tbl);		
}

int mc_obj_init()
{
	int err = 0;

#ifndef AIOP_STANDALONE
	err |= aiop_container_init();	
	err |= dpci_discovery(); /* must be after aiop_container_init */ 
#endif
	return err;
	
}

void mc_obj_free()
{
#ifndef AIOP_STANDALONE
	aiop_container_free();
	dpci_discovery_free();
	/* TODO DPCI close ??? */
#endif
}
