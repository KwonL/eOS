#include "private.h"

/* iflag register. 1: enable, 0: disable */
int32u_t _eflags = 1;

int32u_t _irq_pending = 0x0;

int32u_t _irq_mask = 0xFFFFFFFF;

void _cli() {
	_eflags = 0;
}

void _sti() {
	_eflags = 1;
	_deliver_irq();
}

void _gen_irq(int8u_t irq) {
	_irq_pending |= (0x1 << irq);
	_deliver_irq();
}

