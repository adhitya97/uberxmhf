/*
 * @XMHF_LICENSE_HEADER_START@
 *
 * eXtensible, Modular Hypervisor Framework (XMHF)
 * Copyright (c) 2009-2012 Carnegie Mellon University
 * Copyright (c) 2010-2012 VDG Inc.
 * All Rights Reserved.
 *
 * Developed by: XMHF Team
 *               Carnegie Mellon University / CyLab
 *               VDG Inc.
 *               http://xmhf.org
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the names of Carnegie Mellon or VDG Inc, nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @XMHF_LICENSE_HEADER_END@
 */

/*
 * hypervisor untrusted uobj pagetable uAPI
 *
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <xmhf.h>
#include <xmhf-debug.h>
#include <xmhfgeec.h>

#include <uapi_uhmpgtbl.h>


static void _uhmpgtbl_initmempgtbl_pae(uint32_t slabid){
	uint32_t i, j;

	for(i=0; i < PAE_MAXPTRS_PER_PDPT; i++){
		_uhslabmempgtbl_lvl4t[(slabid - XMHFGEEC_UHSLAB_BASE_IDX)][i] = 0;
	}

	//assign 4GB pdpt entries
	for(i=0; i < PAE_PTRS_PER_PDPT; i++){
		_uhslabmempgtbl_lvl4t[(slabid - XMHFGEEC_UHSLAB_BASE_IDX)][i] =
		    pae_make_pdpe(&_uhslabmempgtbl_lvl2t[(slabid - XMHFGEEC_UHSLAB_BASE_IDX)][i * PAE_PTRS_PER_PDT], (uint64_t)(_PAGE_PRESENT));
	}

	//pdt setup
	for(i=0; i < PAE_PTRS_PER_PDPT * PAE_PTRS_PER_PDT; i++){
		_uhslabmempgtbl_lvl2t[(slabid - XMHFGEEC_UHSLAB_BASE_IDX)][i] =
			pae_make_pde(&_uhslabmempgtbl_lvl1t[(slabid - XMHFGEEC_UHSLAB_BASE_IDX)][(i * PAE_PTRS_PER_PT)], (uint64_t)(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER));
	}

}


void _uhmpgtbl_initmempgtbl(uapi_uhmpgtbl_initmempgtbl_params_t *initmempgtblp){

    if( (initmempgtblp->dst_slabid < XMHFGEEC_TOTAL_SLABS) &&
	(initmempgtblp->dst_slabid >= XMHFGEEC_UHSLAB_BASE_IDX && initmempgtblp->dst_slabid <= XMHFGEEC_UHSLAB_MAX_IDX) &&
	(xmhfgeec_slab_info_table[initmempgtblp->dst_slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG ||
	 xmhfgeec_slab_info_table[initmempgtblp->dst_slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG)
      ){

            _uhmpgtbl_initmempgtbl_pae(initmempgtblp->dst_slabid);

            _XDPRINTF_("%s: setup slab %u with PAE\n", __func__,
            		initmempgtblp->dst_slabid);

	}else{
		//_XDPRINTF_("%s: Halting. Unknown slab type %u\n", __func__, slabtype);
	}
}

