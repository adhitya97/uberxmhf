/*
	entry stub

	author: amit vasudevan (amitvasudevan@acm.org)
*/

.globl entry
entry:

	/* turn on unaligned memory access */
	mrc p15, #0, r4, c1, c0, #0
	orr r4, #0x400000				/*set U bit (bit-22) */
	mcr p15, #0, r4, c1, c0, #0

	/* load stack and start C land */
	ldr sp, =stack_top
	bl main

halt:
	b halt


.globl mmio_write32
mmio_write32:
    str r1,[r0]
    bx lr

.globl mmio_read32
mmio_read32:
    ldr r0,[r0]
    bx lr


.globl chainload_os
chainload_os:
	ldr r3, =0x00008000
	blx r3


.section ".stack"
	.balign 8
	.global stack
	stack:	.space	256
	.global stack_top
	stack_top:
