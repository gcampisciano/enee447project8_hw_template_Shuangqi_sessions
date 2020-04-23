#include "os.h"


#define SCTLR_THUMB_EXCEPTION		(1<<30)
#define SCTLR_ACCESS_FLAG_ENABLE	(1<<29)
#define SCTLR_TEX_REMAP_ENABLE		(1<<28)
#define SCTLR_NONMASKABLE_FIQ		(1<<27)
#define SCTLR_EXCEPTION_ENDIANESS	(1<<25)
#define SCTLR_IRQ_VECTOR_ENABLE		(1<<24)
#define SCTLR_ALIGNMENT			(1<<22)
#define SCTLR_FAST_INTERRUPT_CONFIG	(1<<21)
#define SCTLR_UWXN			(1<<20)
#define SCTLR_WXN			(1<<19)
#define SCTLR_HARDWARE_ACCESS		(1<<17)
#define SCTLR_ROUND_ROBIT		(1<<14)
#define SCTLR_HIGH_VECTORS		(1<<13)
#define SCTLR_ICACHE_ENABLE		(1<<12)
#define SCTLR_BRANCH_PREDICTOR_ENABLE	(1<<11)
#define SCTLR_SWP_ENABLE		(1<<10)
#define SCTLR_CP15_BARRIER_ENABLE	(1<<5)
#define SCTLR_CACHE_ENABLE		(1<<2)
#define SCTLR_ALIGNMENT_CHECK		(1<<1)
#define SCTLR_MMU_ENABLE		(1<<0)


#define	ONE_MB		0x00100000
#define	PT_START	0x00030000

unsigned long *pagetables;

long next_page = 0;

unsigned long *
vm_pagetable( long asid )
{
	// return the address of the corresponding page table
}

unsigned long
vm_translate( long thread, long addr )
{
	// use the thread's page table to translate the virtual address to physical
}

long
vm_allocate()
{
	next_page += ONE_MB;
	return next_page;
}

void
vm_deallocate( long address )
{
	// normally we would want to implement this, to prevent memory leaks ...
	// but this is a ridiculously simple OS, so we will skip it
}

//
// vaddr and paddr are assumed to be addresses, not page numbers
// (i.e., not already right-shifted)
//
void
map( unsigned int ASID, unsigned int vaddr, unsigned int paddr, int io, int global )
{
	// insert the mapping into the page table
}


void
initialize_table( long asid )
{
	int i;
	for (i=0; i<4096; i++) {	// PTE loop
		pagetables[(asid << 12) + i] = 0;
	}
}


void
init_vm()
{
	unsigned int x, i;

	next_page = ONE_MB;

	pagetables = (unsigned long *)PT_START;
	initialize_table(0);

	// default values
	for (i=0; i<1024; i++) {
		map(0, i * ONE_MB, i * ONE_MB, 0, 0);	// kernel access to code & data
	}
	// override the defaults w/ I/O locations
	map(0, 0x3F000000, 0x3F000000, 1, 1);	// I/O addresses
	map(0, 0x3F100000, 0x3F100000, 1, 1);	// I/O addresses
	map(0, 0x3F200000, 0x3F200000, 1, 1);	// I/O addresses
	map(0, 0x3F300000, 0x3F300000, 1, 1);	// I/O addresses
	map(0, 0x40000000, 0x40000000, 1, 1);	// make sure timer addresses are still okay

	//
	// do all the rest of the initialization required to set of the various control registers
	//

}
