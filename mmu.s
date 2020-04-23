
.global	invalidate_tlb
invalidate_tlb:
	mov		r2,#0
    mcr		p15,0,r2,c8,c7,0 @ invalidate tlb
	mcr		p15,0,r2,c7,c10,4 @ DSB ??
	bx		lr

.globl sync
sync:
	dmb
	dsb
	isb
	bx lr

.global readTTBCR
readTTBCR:
	MRC		p15, 0, r0, c2, c0, 2 		@ Read TTBCR into r0 
	bx		lr

.global writeTTBCR
writeTTBCR:
	MCR		p15, 0, r0, c2, c0, 2 		@ Write r0 to TTBCR
	bx		lr


.global readTTBR0
readTTBR0:
	MRC		p15, 0, r0, c2, c0, 0 		@ Read 32-bit TTBR0 into r0
	bx		lr

.global writeTTBR0
writeTTBR0:
	MCR		p15, 0, r0, c2, c0, 0 		@ Write r0 to 32-bit TTBR0
	bx		lr

.global readTTBR1
readTTBR1:
	MRC		p15, 0, r0, c2, c0, 1 		@ Read 32-bit TTBR1 into r0
	bx		lr

.global writeTTBR1
writeTTBR1:
	MCR		p15, 0, r0, c2, c0, 1 		@ Write r0 to 32-bit TTBR1
	bx		lr


.global readCONTEXTIDR
readCONTEXTIDR:
	MRC		p15, 0, r0, c13, c0, 1 		@ Read CONTEXTIDR into Rt
	bx		lr

.global writeCONTEXTIDR
writeCONTEXTIDR:
	MCR		p15, 0, r0, c13, c0, 1 		@ Write Rt to CONTEXTIDR
	bx		lr


.global readDACR
readDACR:
	MRC		p15, 0, r0, c3, c0, 0 		@ Read DACR into Rt 
	bx		lr

.global writeDACR
writeDACR:
	MCR		p15, 0, r0, c3, c0, 0 		@ Write Rt to DACR
	bx		lr


.global readSCTLR
readSCTLR:
	MRC		p15, 0, r0, c1, c0, 0 		@ Read SCTLR into Rt 
	bx		lr

.global writeSCTLR
writeSCTLR:
	MCR		p15, 0, r0, c1, c0, 0 		@ Write Rt to SCTLR
	bx		lr


.global setSMP
setSMP:
	MRRC	p15, 1, r0, r1, c15 		// read CPUECTLR
	ORR		r0, r0, #(0x1 << 6)			// set SMPEN
	MCRR	p15, 1, r0, r1, c15 		// write CPUECTLR
	bx		lr

