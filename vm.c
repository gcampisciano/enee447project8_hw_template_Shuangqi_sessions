#include "os.h"

#define ONE_MB      0x00100000
#define PAGE_TABLES_START   0x00030000
#define NUM_OF_ENTRIES_IN_ONE_PAGE_TABLE 4096

unsigned int (*pagetables)[NUM_OF_ENTRIES_IN_ONE_PAGE_TABLE];

long next_available_physical_page = PAGE_TABLES_START;


/*
    \brief Return a thread's page table 

    You should make use of variable `pagetables` here.
*/
unsigned int*
vm_pagetable( long asid )
{
    return pagetables[asid];
}

/*
    \brief Return the physical address of a thread's virtual address 
*/
unsigned int
vm_translate( long ASID, unsigned int addr )
{
    return pagetables[asid][addr];
}

/*
    \brief Return the address of a free physical page that can be used for VM mapping

    You should make use of variable `next_available_physical_page` here.
*/
unsigned int
vm_allocate()
{
    // Set address to next_available_physical_page (initial value = 0x00030000)
    int addr = next_available_physical_page;
    
    // Increment next_available_physical_page to next page location (One MB)
    next_available_physical_page = next_available_physical_page + ONE_MB;
    
    // return address
    return addr;
}

void
vm_deallocate( unsigned int address )
{
    // normally we would want to implement this, to prevent memory leaks ...
    // but this is a ridiculously simple OS, so we will skip it
}

/*
    \brief Set up a page table entry of *section* format and put it into page table.

    The format of an entry can be found in Figure B3-4 in the offcial ARM doc

    \param ASID The id of the thread whose page table is gonna be modified.

    \param vaddr The virtual address to be mapped from 

    \param paddr The physical address to be mapped to

    \param io This is boolean value that indicates whether the
            addresses contained in this virtual page are used 
            for memmory-mapped IO. 
    
            If io == 1, you can initalize TEX, C, and B bits to 
                make the entry a "Non-shareable Device".

            If io == 0, you can initalize TEX, C, and B bits to 
                make the entry a "Cacheable memory".

    \param global This is a boolean value that indicates whether
            this virtual-to-physical mapping belongs to all threads.

            You should initialize the nG bit accordingly.
*/
void map( unsigned int ASID, unsigned int vaddr, unsigned int paddr, int io, int global )
{
    // Set pagetable so vaddr maps to paddr
    pagetables[ASID][vaddr] = paddr;
}


void
initialize_table( long asid )
{
    int i;
    for (i=0; i<NUM_OF_ENTRIES_IN_ONE_PAGE_TABLE; i++) {
        pagetables[asid][i] = 0;
    }
}


void
init_vm()
{
    long kernel_asid = 0;

    next_available_physical_page = ONE_MB;

    pagetables = PAGE_TABLES_START;
    initialize_table(0);

    // default values
    for (int i=0; i<1024; i++) {
        map(0, i * ONE_MB, i * ONE_MB, 0, 0);   // kernel access to code & data
    }




    // override the defaults w/ I/O locations
    map(0, 0x3F000000, 0x3F000000, 1, 1);   // I/O addresses
    map(0, 0x3F100000, 0x3F100000, 1, 1);   // I/O addresses
    map(0, 0x3F200000, 0x3F200000, 1, 1);   // I/O addresses
    map(0, 0x3F300000, 0x3F300000, 1, 1);   // I/O addresses
    map(0, 0x40000000, 0x40000000, 1, 1);   // make sure timer addresses are still okay


    unsigned int *kernel_page_table = vm_pagetable(kernel_asid);
    log("kernel page table[0] =",kernel_page_table[0]);
    log("kernel page table[1] =",kernel_page_table[1]);
    log("kernel page table[2] =",kernel_page_table[2]);


    // Make sure all pre-MMU memory accesses are done
    sync();


    // Initialize system registers to set up VM and enable MMU
    // set up ASID
    writeCONTEXTIDR(kernel_asid);

    // set up page table. Since currently we are in kernel,
    // we are gonna use its page table.
    writeTTBR0((unsigned int)kernel_page_table|0x4a);

    long value = readTTBCR();
    // set EAE=0, Use the 32-bit translation system, 
    // with the Short-descriptor translation table format
    value &= 0x7FFFFFFF;
    // set N=0, disables use of a second set of translation tables, i.e. TTBR1.
    value &= 0xFFFFFFF8;
    writeTTBCR(value);

    value = readDACR();
    // set manager mode b11 for all domains so permission bits
    // are not used.
    value |= 0xFFFFFFFF;
    writeDACR(value);

    value = readSCTLR();
    // set TRE=0, disable TEX remap
    value &= 0xEFFFFFFF;
    // set M=1, enable MMU
    value |= 0x00000001;
    writeSCTLR(value);

}
