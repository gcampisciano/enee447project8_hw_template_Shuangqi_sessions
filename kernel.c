//
// timeout queue example
//

#include "os.h"
#include "SDCard.h"

extern struct dev devtab[];
extern char *returns[];

int count;

void
periodic_timer()
{
	if (DEBUG_LOW >= DEBUG_LEVEL && (count++ % 100) == 0) debug(DEBUG_LOW, "periodic timer called", NOVAL);
//	if ((count % 100) == 0) log("periodic timer called", NOVAL);
//	if ((count++ % 100) == 0) dump_processes();

	uint64_t time = get_time();
	time += MAX_BLOCK_TIME;

	do {
		if (!handle_timeoutq_event()) break;
	} while (get_time() <= time);

	scheduler(THREAD_RUN);

	return;
}

void
init_kernel()
{
	struct dev *dp;
	int i;

	count = 0;

    // init functions
    init_io();

	// I/O devices are up and running.
	blink_led_stall(2);

    // init timeoutq
    init_timeoutq();

	// initial console output
    uart_puts("\n\n");
	log("...", NOVAL);
    log("System is booting, kernel cpuid =", cpu_id());
	log(kversion, NOVAL);

    // init SD card
    log("Initializing SD Card ...", NOVAL);
	while ((i = sdInitCard(NULL, NULL, true)) != SD_OK) {
		log(returns[i], i);
	}
    log("  ... SD Card working.", NOVAL);

    // init virtual memory
	log("Starting virtual memory ...", NOVAL);
    init_vm();
    log("  ... VM up and running", NOVAL);

	if (DEBUG_MED >= DEBUG_LEVEL) {
		log("Available devices:", NOVAL);
		for (dp = devtab, i=0; i<MAX_DEVICES; i++, dp++) {
			log(dp->devname, NOVAL);
			log("Device number: ", i);
			log("Device type:   ", dp->devtype);
			log("Init function: ", (long)(dp->init));
			log("Read function: ", (long)(dp->read));
			log("Write function:", (long)(dp->write));
			log("Rck function:  ", (long)(dp->rcheck));
			log("Wck function:  ", (long)(dp->wcheck));
		}
	}

	init_threads();
	scheduler(THREAD_INIT);
	create_thread("shell", "shell.bin");

    // wake up
	log("...", NOVAL);
    log("Init complete. Please hit any key to continue.", NOVAL);

	// everything is up and running.
	blink_led_stall(5);

	// wait on user input
    uart_recv();
    uart_puts("\n");

	return;

}

