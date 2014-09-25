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

// XMHF rich guest (xcrichguest)
// author: amit vasudevan (amitvasudevan@acm.org)

//---includes-------------------------------------------------------------------
#include <xmhf.h>
#include <xmhf-core.h>
#include <xmhf-debug.h>

#include <xcrichguest.h>

#include <xcapi.h>
#include <xhhyperdep.h>	//TODO: remove this hardcoded hypapp dependency

slab_retval_t xcrichguest_interface(u32 src_slabid, u32 dst_slabid, u32 fn_id, u32 fn_paramsize, ...){
	slab_retval_t srval;
	va_list args;

	_XDPRINTF_("%s: Got control: src_slabid=%u, dst_slabid=%u, fn_id=%u, fn_paramsize=%u\n", __FUNCTION__, src_slabid, dst_slabid, fn_id, fn_paramsize);

	switch(fn_id){
			/*case XMHF_SLAB_XCRICHGUEST_FNENTRY:{
				u32 cpuid;
				bool is_bsp;
				va_start(args, fn_paramsize);
				cpuid = va_arg(args, u32);
				is_bsp = va_arg(args, bool);
				srval.retval_bool = xcrichguest_entry(cpuid, is_bsp);
				va_end(args);
			}
			break;*/

			case XMHF_SLAB_XCRICHGUEST_FNGUESTMEMORYREPORTING:{
				context_desc_t context_desc;
				struct regs r;
				va_start(args, fn_paramsize);
				context_desc = va_arg(args, context_desc_t);
				r = va_arg(args, struct regs);
				srval.retval_regs = xcrichguest_arch_handle_guestmemoryreporting(context_desc, r);
				va_end(args);
			}
			break;

			default:
				_XDPRINTF_("%s: unhandled subinterface %u. Halting\n", __FUNCTION__, fn_id);
				HALT();
	}

	return srval;
}




//*
//add given cpu to the rich guest partition
static context_desc_t _xcrichguest_addcpu(u32 partition_index, u32 cpuid, bool is_bsp){
	context_desc_t context_desc;
    slab_retval_t srval;

	//add cpu to the richguest partition
    srval = XMHF_SLAB_CALL_P2P(xcapi, XMHF_SLAB_XCRICHGUEST_INDEX, XMHF_SLAB_XCAPI_INDEX, XMHF_SLAB_XCAPI_FNXCAPIPARTITIONADDCPU, XMHF_SLAB_XCAPI_FNXCAPIPARTITIONADDCPU_SIZE, partition_index, cpuid, is_bsp);
    context_desc = srval.retval_context_desc;
	//bail out if we could not add cpu to the rich guest partition
	if(context_desc.cpu_desc.cpu_index == XC_PARTITION_INDEX_INVALID || context_desc.partition_desc.partition_index == XC_PARTITION_INDEX_INVALID){
		_XDPRINTF_("%s(%u): could not add cpu to rich guest partition. Halting!\n", __FUNCTION__, cpuid);
		return context_desc;
	}

	//setup guest OS state for partition
	xcrichguest_arch_setupguestOSstate(context_desc);

	return context_desc;
}

void xcrichguest_initialize(u32 partition_index){
	xcrichguest_arch_initialize(partition_index);
}

context_desc_t xcrichguest_addcpu(u32 partition_index, u32 cpuid, bool is_bsp){
	context_desc_t context_desc;
	context_desc=_xcrichguest_addcpu(partition_index, cpuid, is_bsp);

	if(context_desc.cpu_desc.cpu_index == XC_PARTITION_INDEX_INVALID || context_desc.partition_desc.partition_index == XC_PARTITION_INDEX_INVALID){
		_XDPRINTF_("%s(%u): Fatal error, could not add cpu to rich guest. Halting!\n", __FUNCTION__, cpuid);
		HALT();
	}
	return context_desc;
}

///////
XMHF_SLAB_DEF(xcrichguest)
