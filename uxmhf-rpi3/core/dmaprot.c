/*
	Pi3 DMA protection implementation

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#include <types.h>
#include <arm8-32.h>
#include <bcm2837.h>
#include <miniuart.h>
#include <debug.h>

//activate DMA protection mechanism
void dmaprot_activate(void){
	u64 attrs_dev = (LDESC_S2_MC_DEVnGnRnE << LDESC_S2_MEMATTR_MC_SHIFT) |
			(LDESC_S2_S2AP_READ_ONLY << LDESC_S2_MEMATTR_S2AP_SHIFT) |
			(MEM_INNER_SHAREABLE << LDESC_S2_MEMATTR_SH_SHIFT) |
			LDESC_S2_MEMATTR_AF_MASK;

	uapi_s2pgtbl_setprot(BCM2837_DMA0_REGS_BASE, attrs_dev);
	sysreg_tlbiallis();
	uapi_s2pgtbl_setprot(BCM2837_DMA15_REGS_BASE, attrs_dev);
	sysreg_tlbiallis();
}

_XDPRINTFSMP_("%s: s2pgtbl DATA ABORT va=0x%08x, pa=0x%08x\n", __func__,
		fault_va, fault_pa);
_XDPRINTFSMP_("%s: s2pgtbl DATA ABORT: sas=%u, srt=%u, wnr=%u\n", __func__,
		da_iss_sas, da_iss_srt, da_iss_wnr);

//handle DMA controller accesses
//va = virtual address of DMA controller register
//pa = physical address of DMA controller regiter
//sas = size of access
//srt = general purpose register number
//wnr = write/read indicator
//il = instruction length
void dmaprot_handle_dmacontroller_access(u32 va, u32 pa, u32 sas, u32 srt, u32 wnr, u32 il){


}
