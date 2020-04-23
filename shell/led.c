
#define GPFSEL2 0x3F200008
#define GPFSEL3 0x3F20000C
#define GPFSEL4 0x3F200010
#define GPSET0  0x3F20001c
#define GPSET1  0x3F200020
#define GPCLR0  0x3F200028
#define GPCLR1  0x3F20002C

void
wait()
{
	int i;
	for (i=0; i<200000; i++) dummy();
}

void
init_led()
{
	unsigned int ra;

	ra=GET32(GPFSEL2);
	ra&=~(7<<27);
	ra|=1<<27;
	PUT32(GPFSEL2,ra);

	PUT32(GPSET0,1<<29);
	PUT32(GPCLR0,1<<29);
}

void
led_on()
{
	PUT32(GPSET0,1<<29);

	return;
}

void
led_off()
{
	PUT32(GPCLR0,1<<29);

	return;
}

void
blink_led_stall(unsigned int reps)
{
	unsigned int ra;

	ra=GET32(GPFSEL2);
	ra&=~(7<<27);
	ra|=1<<27;
	PUT32(GPFSEL2,ra);

	PUT32(GPCLR0,1<<29);

	while(reps--) {
		led_on();
		wait();

		led_off();
		wait();
	}

	return;
}

