#include <core/eos.h>
#include "emulator.h"

/* intialize hardware dependent parts */
void _os_init_hal() {
	PRINT("initializing hal module.\n");

	/* initialize timer interrupt */
	_init_timer_interrupt();

	/* initiate interval timer by unmasking timer interrupt */
	eos_enable_irq_line(IRQ_INTERVAL_TIMER0);
}
