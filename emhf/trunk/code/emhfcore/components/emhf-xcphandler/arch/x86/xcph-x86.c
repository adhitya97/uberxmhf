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
 * This file is part of the EMHF historical reference
 * codebase, and is released under the terms of the
 * GNU General Public License (GPL) version 2.
 * Please see the LICENSE file for details.
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
 * EMHF exception handler component interface
 * x86 arch. backend
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <emhf.h>

//---function to obtain the vcpu of the currently executing core----------------
//XXX: TODO, move this into baseplatform as backend
//note: this always returns a valid VCPU pointer
static VCPU *_svm_getvcpu(void){
  int i;
  u32 eax, edx, *lapic_reg;
  u32 lapic_id;
  
  //read LAPIC id of this core
  rdmsr(MSR_APIC_BASE, &eax, &edx);
  ASSERT( edx == 0 ); //APIC is below 4G
  eax &= (u32)0xFFFFF000UL;
  lapic_reg = (u32 *)((u32)eax+ (u32)LAPIC_ID);
  lapic_id = *lapic_reg;
  //printf("\n%s: lapic base=0x%08x, id reg=0x%08x", __FUNCTION__, eax, lapic_id);
  lapic_id = lapic_id >> 24;
  //printf("\n%s: lapic_id of core=0x%02x", __FUNCTION__, lapic_id);
  
  for(i=0; i < (int)g_midtable_numentries; i++){
    if(g_midtable[i].cpu_lapic_id == lapic_id)
        return( (VCPU *)g_midtable[i].vcpu_vaddr_ptr );
  }

  printf("\n%s: fatal, unable to retrieve vcpu for id=0x%02x", __FUNCTION__, lapic_id);
  HALT(); return NULL; /* will never return presently */
}

//---function to obtain the vcpu of the currently executing core----------------
//XXX: move this into baseplatform as backend
//note: this always returns a valid VCPU pointer
static VCPU *_vmx_getvcpu(void){
  int i;
  u32 eax, edx, *lapic_reg;
  u32 lapic_id;
  
  //read LAPIC id of this core
  rdmsr(MSR_APIC_BASE, &eax, &edx);
  ASSERT( edx == 0 ); //APIC is below 4G
  eax &= (u32)0xFFFFF000UL;
  lapic_reg = (u32 *)((u32)eax+ (u32)LAPIC_ID);
  lapic_id = *lapic_reg;
  //printf("\n%s: lapic base=0x%08x, id reg=0x%08x", __FUNCTION__, eax, lapic_id);
  lapic_id = lapic_id >> 24;
  //printf("\n%s: lapic_id of core=0x%02x", __FUNCTION__, lapic_id);
  
  for(i=0; i < (int)g_midtable_numentries; i++){
    if(g_midtable[i].cpu_lapic_id == lapic_id)
        return( (VCPU *)g_midtable[i].vcpu_vaddr_ptr );
  }

  printf("\n%s: fatal, unable to retrieve vcpu for id=0x%02x", __FUNCTION__, lapic_id);
  HALT();
  return NULL; /* currently unreachable */
}


//initialize EMHF core exception handlers
void emhf_xcphandler_arch_initialize(void){
	u32 *pexceptionstubs;
	u32 i;

	printf("\n%s: setting up runtime IDT...", __FUNCTION__);
	
	pexceptionstubs=(u32 *)&emhf_xcphandler_exceptionstubs;
	
	for(i=0; i < EMHF_XCPHANDLER_MAXEXCEPTIONS; i++){
		idtentry_t *idtentry=(idtentry_t *)((u32)emhf_xcphandler_arch_get_idt_start()+ (i*8));
		idtentry->isrLow= (u16)pexceptionstubs[i];
		idtentry->isrHigh= (u16) ( (u32)pexceptionstubs[i] >> 16 );
		idtentry->isrSelector = __CS;
		idtentry->count=0x0;
		idtentry->type=0x8E;	//32-bit interrupt gate
								//present=1, DPL=00b, system=0, type=1110b
	}
	
	printf("\n%s: IDT setup done.", __FUNCTION__);
}


//get IDT start address
u8 * emhf_xcphandler_arch_get_idt_start(void){
	return (u8 *)&emhf_xcphandler_idt_start;
}


//EMHF exception handler hub
void emhf_xcphandler_arch_hub(u32 vector, struct regs *r){
	u32 cpu_vendor = get_cpu_vendor_or_die();	//determine CPU vendor
	VCPU *vcpu;
	
	printf("\n%s: exception %u...", __FUNCTION__, vector);

	if(cpu_vendor == CPU_VENDOR_AMD){
		vcpu=_svm_getvcpu();
	}else{	//CPU_VENDOR_INTEL
	    vcpu=_vmx_getvcpu();
	}	
	
	switch(vector){
			case CPU_EXCEPTION_NMI:
				emhf_smpguest_arch_x86_eventhandler_nmiexception(vcpu, r);
				break;

			default:{
				u32 exception_cs, exception_eip;
				
				printf("\n%s: unhandled exception, halting!", __FUNCTION__);
				printf("\n%s: state dump follows...", __FUNCTION__);
				//things to dump
				printf("\nVCPU at 0x%08x, core=0x%02x", (u32)vcpu, vcpu->id);
				printf("\nEAX=0x%08x EBX=0x%08x ECX=0x%08x EDX=0x%08x",
						r->eax, r->ebx, r->ecx, r->edx);
				printf("\nESI=0x%08x EDI=0x%08x EBP=0x%08x ESP=0x%08x",
						r->esi, r->edi, r->ebp, r->esp);
				printf("\nCS=0x%04x, DS=0x%04x, ES=0x%04x, SS=0x%04x",
					(u16)read_segreg_cs(), (u16)read_segreg_ds(),
					(u16)read_segreg_es(), (u16)read_segreg_ss());
				printf("\nFS=0x%04x, GS=0x%04x",
					(u16)read_segreg_fs(), (u16)read_segreg_gs());
				printf("\nTR=0x%04x", (u16)read_tr_sel());
				//CS:EIP of exception
				//EFLAGS
				HALT();
			}
	}
}
