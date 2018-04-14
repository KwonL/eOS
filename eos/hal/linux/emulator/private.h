#ifndef HAL_LINUX_EMUL_PRIVATE_H
#define HAL_LINUX_EMUL_PRIVATE_H

#include "../type.h"

extern int32u_t _eflags;
extern void _gen_irq(int8u_t irq);
extern void _deliver_irq();

#endif
