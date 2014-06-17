#include "common/types.h"
#include "common/fsl_stdio.h"
#include "dpni/drv.h"
#include "fsl_ip.h"
#include "kernel/platform.h"
#include "io.h"
#include "aiop_common.h"
#include "fsl_parser.h"
#include "general.h"
#include "dbg.h"
#include "fsl_cmdif_server.h"
#include "dplib/fsl_cdma.h"

int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */


__HOT_CODE static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0;
	const uint16_t ipv4hdr_length = sizeof(struct ipv4hdr);
	uint16_t ipv4hdr_offset = 0;
	uint8_t *p_ipv4hdr = 0;
	uint32_t src_addr = 0x10203040;// new ipv4 src_addr
	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_os_print
		("app_process_packet:Core %d received packet with ipv4 header:\n",
	    core_get_id());
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		p_ipv4hdr = UINT_TO_PTR((ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS()));
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");
	}
	err = ip_set_nw_src(src_addr);
	if (err) {
		fsl_os_print("ERROR = %d: ip_set_nw_src(src_addr)\n", err);
	}
	if (!err && PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_os_print
		("app_process_packet: Core %d will send a modified packet with ipv4 header:\n"
		, core_get_id());
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");
	}

	dpni_drv_send(APP_NI_GET(arg));
}

#ifdef AIOP_STANDALONE
/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
static void epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* EPID = 0 is saved for cmdif, need to set it for stand alone demo */
	iowrite32(0, &wrks_addr->epas);
	iowrite32((uint32_t)receive_cb, &wrks_addr->ep_pc);
}
#endif /* AIOP_STANDALONE */

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	fsl_os_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_os_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	fsl_os_print("ctrl_cb cmd = 0x%x, size = %d, data high= 0x%x data low= 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)((data & 0xFF00000000) >> 32),
	             (uint32_t)(data & 0xFFFFFFFF));
	return 0;
}

static struct cmdif_module_ops ops = {
                               .open_cb = open_cb,
                               .close_cb = close_cb,
                               .ctrl_cb = ctrl_cb
};

int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;

	fsl_os_print("Running app_init()\n");

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */

	for (ni = 0; ni < 6; ni++)
	{
		/* Every ni will have 1 flow */
		uint32_t flow_id = 0;
		err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/,
		                              (uint16_t)flow_id/*flow_id*/,
		                              app_process_packet_flow0, /* callback for flow_id*/
		                              (ni | (flow_id << 16)) /*arg, nic number*/);
		if (err) return err;
	}

	err = cmdif_register_module("TEST0", &ops);
	if (err)
		fsl_os_print("FAILED cmdif_register_module\n!");

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}
