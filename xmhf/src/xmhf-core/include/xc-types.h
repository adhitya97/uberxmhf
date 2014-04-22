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

// XMHF core specific data types
// author: amit vasudevan (amitvasudevan@acm.org)

#ifndef __XC_TYPES_H__
#define __XC_TYPES_H__


#ifndef __ASSEMBLY__

//define XMHF core API aggregate return type
//allows us to return multiple values without using pointers to pointers
typedef struct xmhfcoreapiretval {
	u64 returnval;
	void *returnptr1;
} xmhfcoreapiretval_t;


typedef u8 xc_cpuarchdata_t;

typedef struct {
		void *stack;			//CPU stack
		u32 cpuid;				//unique CPU id
		bool is_bsp;			//true if CPU is the boot-strap processor
		bool is_quiesced;		//true if CPU is quiesced
		xc_cpuarchdata_t *cpuarchdata;
		u32 index_partitiondata;	//index into partition data buffer
} __attribute__ ((packed)) xc_cpu_t;

typedef struct {
		u32 cpuid;				//unique CPU id
		//u32 index_cpudata;		//index into CPU data buffer
		xc_cpu_t *xc_cpu;
} __attribute__((packed)) xc_cputable_t;


typedef u8 xc_partition_hptdata_t;
typedef u8 xc_partition_trapmaskdata_t;

typedef struct {
		u32 partitionid;			//unique partition id
		u32 partitiontype;			//primary or secondary
		//u32 index_hwpagetabledata;	//index into h/w page table data buffer
		xc_partition_hptdata_t *hptdata;
		xc_partition_trapmaskdata_t *trapmaskdata;
		//u32 indices_cpudata[MAX_PLATFORM_CPUS];	//indices into cpu data buffer for all cpus allocated to the partition
		//u32 number_of_cpus;			//number of cpus allocated to the partition
} xc_partition_t;


#define XC_INDEX_INVALID			(0xFFFFFFFFUL)

#define XC_PARTITION_PRIMARY		(1)
#define XC_PARTITION_SECONDARY		(2)

//XMHF CPU descriptor type
typedef struct {
	uint32_t id;
	bool isbsp;
	xc_cpu_t *xc_cpu;
} cpu_desc_t;
	
//XMHF partition descriptor type
typedef struct {
	uint32_t id;
} partition_desc_t;

//XMHF context descriptor type (context = partition + cpu pair)
typedef struct {
	partition_desc_t partition_desc;
	cpu_desc_t cpu_desc;
} context_desc_t;


//revised app parameter block; will replace the above decl. when done
typedef struct {
  u32 runtimephysmembase;
  u32 runtimesize;
} __attribute__((packed)) hypapp_env_block_t;


//hypapp parameter block
typedef struct {
	u64 param1;
	u64 param2;
	u64 param3;
	u64 param4;
	u64 param5;
	u64 param6;
	u64 param7;
	u64 param8;
	u64 result;
	context_desc_t context_desc;
	hypapp_env_block_t hypappenvb;
	xmhfcoreapiretval_t retval;
} __attribute__((packed)) XMHF_HYPAPP_PARAMETERBLOCK;


//application parameter block
//for now it holds the bootsector and optional module info loaded by GRUB
//eventually this will be generic enough for both boot-time and dynamic loading
//capabilities
typedef struct {
  u32 bootsector_ptr;
  u32 bootsector_size;
  u32 optionalmodule_ptr;
  u32 optionalmodule_size;
  u32 runtimephysmembase;
  u32 runtimesize;
  char cmdline[1024];
} __attribute__((packed)) APP_PARAM_BLOCK;


//hypapp binary header 
typedef struct {
  u32 magic;
  u32 addr_hypappfromcore;	//address is hypapp where control is transferred to from the core
  u32 addr_hypapptocore;	//address is where control is transferred to the core when hypapp calls into core
  u32 addr_tos;				//hypapp top-of-stack address
  APP_PARAM_BLOCK apb;		//hypapp parameter block
  void *optionalparam1;
  void *optionalparam2;
} __attribute__((packed)) XMHF_HYPAPP_HEADER;

#define XMHF_HYPAPP_HEADER_MAGIC	0xDEADBEEF
 
#endif //__ASSEMBLY__


#endif //__XC_TYPES_H__
