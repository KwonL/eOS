#include "emulator.h"
#include <core/eos_internal.h>

/* ack the specified irq */
void eos_ack_irq(int32u_t irq) {
	/* clear the corresponding bit in _irq_pending register */
	_irq_pending &= ~(0x1<<irq);
}

/* get the irq number */
int32s_t eos_get_irq() {
	/* get the highest bit position in the _irq_pending register */
	int i = 31;
	for(; i>=0; i--) {
		if (_irq_pending & (0x1<<i)) {
			return i;
		}
	}
	return -1;
}

/* mask an irq */
void eos_disable_irq_line(int32u_t irq) {
	/* turn on the corresponding bit */
	_irq_mask |= (0x1<<irq);
}

/* unmask an irq */
void eos_enable_irq_line(int32u_t irq) {
	/* turn off the corresponding bit */
	_irq_mask &= ~(0x1<<irq);
}
