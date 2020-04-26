#include "os.h"

#define ONE_MB      0x00100000
#define PAGE_TABLES_START   0x00030000
#define NUM_OF_ENTRIES_IN_ONE_PAGE_TABLE 4096

unsigned int (*pagetables)[NUM_OF_ENTRIES_IN_ONE_PAGE_TABLE];

long next_available_physical_page;


/*
    \brief Return a thread's page table 

    You should make use of variable `pagetables` here.
*/
unsigned int*
vm_pagetable( long asid )
{
    // Needs implementation 
}

/*
    \brief Return the physical address of a thread's virtual address 
*/
unsigned int
vm_translate( long ASID, unsigned int addr )
{
    // Needs implementation 
}

/*
    \brief Return the address of a free physical page that can be used for VM mapping

    You should make use of variable `next_available_physical_page` here.
*/
unsigned int
vm_allocate()
{
    // Needs implementation 
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

            If io == 1, you can initalize TEX, C, and B bits to 
                make the entry a "Cacheable memory".

    \param global This is a boolean value that indicates whether
            this virtual-to-physical mapping belongs to all threads.

            You should initialize the nG bit accordingly.
*/
void map( unsigned int ASID, unsigned int vaddr, unsigned int paddr, int io, int global )
{

    // Needs implementation 
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




    /*  Needs implementation: initialize system registers to set up VM 
        
        With the help of official ARM doc and the read/write APIs in mmu.s, 
            you need to initialize the following system registers:

            - CONTEXTIDR and TTBR0
                - Have a look at irq_handler to figure out why we want to
                    initialize these two registers here
                - The idea is that once we start VM, the kernel will be the first
                    program to be running in it. So we need to set up kernel's
                    VM context here.
                - NOTE: when you assign a value to TTBR0, you should
                    OR it with 0x4a just like we did in create_thread()
            - TTBCR 
                - 32-bit translation system with short-descrptor format is used
                - only TTBR0 is used ( TTBR1 is not used)
                - NOTE: if you don't want to intialize other bits of the register,
                    just levave them as they are.
            - DACR
                - Since in this project we don't make use of permission bits,
                    you can intialize all domains to be manager mode
            - SCTLR
                - disable TEX remap
                - enable MMU
    */
}
