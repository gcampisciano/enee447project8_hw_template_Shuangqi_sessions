
#include "os.h"

#include <stdbool.h>            // Neede for bool
#include <stdint.h>             // Needed for uint32_t, uint16_t etc
#include "SDCard.h"


extern  long stack_address_runningthread;
extern  long start_address_runningthread;
extern  long * tcb_address_runningthread;
extern  long runningthreadid;

extern int getFileSize( int );

#define NAMESIZE 16

#define THREAD_STACK_START (0x80000000 - 16)
#define THREAD_CODE_START (0x00100000)

struct tcb {
    LL_PTRS;
    char    name[NAMESIZE];
    long    threadid;
    long    regs[TCB_NUMREGS];  // 17th reg is the SPSR saved by context switch
} tcbs[ NUM_THREADS ];

llobject_t TF;
struct tcb *tfree;

struct tcb *active_thread;
struct tcb *null_thread;

llobject_t RQ;
struct tcb *runq;

llobject_t SQ;
struct tcb *sleepq;


long
thread_activeid()
{
    return active_thread->threadid;
}


void
dump_tcb(long threadid)
{
    if (threadid < 0 || threadid > NUM_THREADS) {
        log("dump_tcb: invalid threadid", threadid);
        return;
    }
    if (threadid == NUM_THREADS) {
        threadid = active_thread->threadid;
    }
    log("Dumping TCB for thread", threadid);
    struct tcb *tp = &tcbs[threadid];
    log(tp->name, tp->threadid);
    log(" tcb @", (long)(tp->regs));
    log("  r0  ", tp->regs[REG_r0]);
    log("  r1  ", tp->regs[REG_r1]);
    log("  r2  ", tp->regs[REG_r2]);
    log("  r3  ", tp->regs[REG_r3]);
    log("  r4  ", tp->regs[REG_r4]);
    log("  r5  ", tp->regs[REG_r5]);
    log("  r6  ", tp->regs[REG_r6]);
    log("  r7  ", tp->regs[REG_r7]);
    log("  r8  ", tp->regs[REG_r8]);
    log("  r9  ", tp->regs[REG_r9]);
    log("  r10 ", tp->regs[REG_r10]);
    log("  r11 ", tp->regs[REG_r11]);
    log("  r12 ", tp->regs[REG_r12]);
    log("  sp  ", tp->regs[REG_sp]);
    log("  lr  ", tp->regs[REG_lr]);
    log("  pc  ", tp->regs[REG_pc]-4);
    log("  spsr", tp->regs[REG_spsr]);
    log("  ttbr", tp->regs[REG_ttbr]);
    log("  asid", tp->regs[REG_asid]);

    return;
}

void
create_thread(char *name, char *filename)
{
    struct tcb *tp;
    uint32_t bytesRead;
    long load_address;
    long stack_address;

    log("Calling create_thread",NOVAL);

    if (name == (char *)NULL) {
        // create the NULL thread - the "filename" is actually the text address
        tp = &tcbs[0];
        tp->regs[REG_sp] = 0x20000 - 4;
        tp->regs[REG_pc] = (long)filename;
        tp->regs[REG_ttbr] = (long)vm_pagetable(0) | 0x4a;
        tp->regs[REG_asid] = 0;
        strcpyN(tp->name, "NULL thread", NAMESIZE);
        tp->name[NAMESIZE-1] = '\0';

        //  no need to map it because it's already part of kernel
    } else {
        load_address = vm_allocate();

        // Needs implementation: read the file into load_address

        tp = (struct tcb *)LL_POP(tfree);
        if (tp == (struct tcb *)NULL) {
            log("WARNING: out of processes in create_thread", NOVAL);
            vm_deallocate(load_address);
            return;
        }

        log("new thread from disk:",NOVAL);
        log(filename, load_address);

        if (DEBUG_MED >= DEBUG_LEVEL) idump((int *)load_address);

        tp->regs[REG_sp] = THREAD_STACK_START;
        tp->regs[REG_pc] = THREAD_CODE_START+4;
        tp->regs[REG_ttbr] = (long)vm_pagetable(tp->threadid) | 0x4a;
        tp->regs[REG_asid] = tp->threadid;
        strcpyN(tp->name, name, NAMESIZE);
        tp->name[NAMESIZE-1] = '\0';

        LL_PUSH(runq, tp);

        stack_address = vm_allocate();

        initialize_table(tp->threadid);
        map( tp->threadid, THREAD_CODE_START, load_address, 0, 0 );
        map( tp->threadid, THREAD_STACK_START, stack_address, 0, 0 );

    }

    log(tp->name, tp->threadid);
    log(" tcb   =", (long)tp->regs);
    log(" stack =", tp->regs[REG_sp]);
    log(" start =", tp->regs[REG_pc] - 4);
    log(" ttbr0 =", tp->regs[REG_ttbr]);
    log(" asid  =", tp->regs[REG_asid]);

    unsigned long *pt = vm_pagetable(tp->threadid);
    log(" PTE[0] =",pt[0]);
    log(" PTE[1] =",pt[1]);
    log(" PTE[2] =",pt[2]);

    return;
}

void
scheduler(int new_state)
{
    struct tcb *out;
    struct tcb *in;
    if (new_state < 0 || new_state >= MAX_THREAD_STATES) {
        log("ERROR: scheduler given invalid newstate:", new_state);
        return;
    }
    if (LL_IS_EMPTY(runq)) {
        if (new_state == THREAD_RUN) return;
    }

    if (new_state != THREAD_INIT) {
        if (DEBUG_MED >= DEBUG_LEVEL) dump_tcb(NUM_THREADS);

        out = active_thread;
        active_thread = (struct tcb *)NULL;
        if (out == (struct tcb *)NULL) {
            log("ERROR: no running thread in scheduler()", NOVAL);
        } else {
            debug(DEBUG_HI, "scheduler swapping out", runningthreadid);
            debug(DEBUG_HI, out->name, out->threadid);
            debug(DEBUG_HI, " tcb   =", (long)out->regs);
            debug(DEBUG_HI, " stack =", out->regs[REG_sp]);
            debug(DEBUG_HI, " start =", out->regs[REG_pc]-4);
            debug(DEBUG_HI, " spsr  =", out->regs[REG_spsr]);
            debug(DEBUG_HI, " ttbr  =", out->regs[REG_ttbr]);
            debug(DEBUG_HI, " asid  =", out->regs[REG_asid]);

            // the displaced thread either goes back on runq or it gets put to sleep
            if (out != null_thread && out->threadid > 0) {
                LL_APPEND(((new_state == THREAD_RUN) ? runq : sleepq), out);
            }
        }
    } else {
        out = (struct tcb *)NULL;
    }

    if (LL_IS_EMPTY(runq)) {
        in = null_thread;
        if (in == (struct tcb *)NULL) {
            in = &tcbs[0];
            null_thread = in;
        }
    } else {
        in = (struct tcb *)LL_POP(runq);
        if (in == (struct tcb *)NULL) {
            log("ERROR: failed to get thread from non-empty runq", NOVAL);
            return;
        }
    }

    if (in->regs[REG_spsr] == 0) {
        if (out != (struct tcb *)NULL) {
            in->regs[REG_spsr] = out->regs[REG_spsr] & 0xFFF;
        }
    }

    tcb_address_runningthread = in->regs;
    stack_address_runningthread = in->regs[REG_sp];
    start_address_runningthread = in->regs[REG_pc]-4;
    runningthreadid = in->threadid;

    debug(DEBUG_HI, "scheduler swapping in", in->threadid);
    debug(DEBUG_HI, in->name, in->threadid);
    debug(DEBUG_HI, " stack =", in->regs[REG_sp]);
    debug(DEBUG_HI, " start =", in->regs[REG_pc]-4);
    debug(DEBUG_HI, " tcb   =", (long)in->regs);
    debug(DEBUG_HI, " spsr  =", in->regs[REG_spsr]);
    debug(DEBUG_HI, " ttbr  =", in->regs[REG_ttbr]);
    debug(DEBUG_HI, " asid  =", in->regs[REG_asid]);

    active_thread = in;

    if (DEBUG_MED >= DEBUG_LEVEL) dump_tcb(NUM_THREADS);

    return;
}

void
wake_thread(long threadid, long returnval)
{
    if (threadid < 0 || threadid >= NUM_THREADS) {
        log("ERROR: wake_thread: invalid threadid", threadid);
        return;
    }
    struct tcb *tp = &tcbs[threadid];

    // maybe do an explicit search to ensure it's on the queue?
    if (LL_DETACH(sleepq, tp) == (llobject_t *)NULL) {
        log("WARNING: wake_thread sleepq empty", threadid);
    }

    debug(DEBUG_LOW, "wake thread retun =", returnval);

    tp->regs[REG_r0] = returnval;

    LL_PUSH(runq, tp);

    return;
}

void
init_threads()
{
    int i,j;

    tfree = (struct tcb *)&TF;
    LL_INIT(tfree);

    runq = (struct tcb *)&RQ;
    LL_INIT(runq);

    sleepq = (struct tcb *)&SQ;
    LL_INIT(sleepq);

    for (i=0; i<NUM_THREADS; i++) {
        strcpyN(tcbs[i].name, "[uninitialized]", NAMESIZE);
        tcbs[i].name[NAMESIZE-1] = '\0';
        tcbs[i].threadid = i;
        for (j=0; j<17; j++) {
            tcbs[i].regs[j] = 0;
        }
        if (i > 0) {
            struct tcb *tp = &tcbs[i];
            LL_APPEND(tfree, tp);
        }
    }

    active_thread = (struct tcb *)NULL;
    null_thread = &tcbs[0];

    create_thread((char *)NULL, (char *)hang);
}

void
dump_processes()
{
    struct tcb *tp;
    log("Active processes ...", NOVAL);
    dump_tcb(active_thread->threadid);
    LL_EACH(runq, tp, struct tcb) {
        dump_tcb(tp->threadid);
    }
}
