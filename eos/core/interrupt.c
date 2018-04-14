/********************************************************
 * Filename: core/interrupt.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: user interface for handling interrupt.
 ********************************************************/
#include <core/eos.h>

/*
 * The ICB structure.
 * This represents a in-kernel status of an irq
 */
typedef struct icb {
	int8s_t irqnum;		/* the irq number of this ICB. */
	void (*handler)(int8s_t irqnum, void *arg);	/* the handler to handle this interrupt. */
	void *arg;		/* the argument given to the handler when interrupt is occurred. */
} _os_icb_t;

/*
 * Table of ICB for all interrupts
 */
_os_icb_t _os_icb_table[IRQ_MAX];

void _os_init_icb_table() {
	PRINT("initializing interrupt module.\n");
	int8s_t i;
	for (i=0; i<IRQ_MAX; i++) {
		_os_icb_t *p = &_os_icb_table[i];
		p->irqnum = i;
		p->handler = NULL;
	}
}

void _os_common_interrupt_handler() {
	/* get the irq number */
	int32u_t irq_num = eos_get_irq();

	if (irq_num == -1) { return; }
	/* dispatch the handler and call it */
	_os_icb_t *p = &_os_icb_table[irq_num];
	if (p->handler != NULL) {
		//PRINT("entering irq handler 0x%x\n", (int32u_t)(p->handler));
		p->handler(irq_num, p->arg);
		//PRINT("exiting irq handler 0x%x\n", (int32u_t)(p->handler));
	}
	/* acknowledge the irq */
	eos_ack_irq(irq_num);
	/* return from interrupt */
}

int8s_t eos_set_interrupt_handler(int8s_t irqnum, eos_interrupt_handler_t handler, void *arg) {
	PRINT("irqnum: %d, handler: 0x%x, arg: 0x%x\n", (int32u_t)irqnum, (int32u_t)handler, (int32u_t)arg);
	_os_icb_t *p = &_os_icb_table[irqnum];
	p->handler = handler;
	p->arg = arg;
	return 0;
}

eos_interrupt_handler_t eos_get_interrupt_handler(int8s_t irqnum) {
	_os_icb_t *p = &_os_icb_table[irqnum];
	return p->handler;
}	
