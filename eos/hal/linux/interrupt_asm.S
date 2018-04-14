#include "emulator_asm.h"

.text

/* disable irq and return previous status */
.global eos_disable_interrupt
eos_disable_interrupt:
	mov _eflags, %eax
	_CLI
	ret

/* enable irq by force */
.global eos_enable_interrupt
eos_enable_interrupt:
	_STI
	ret

/* restore irq status */
.global eos_restore_interrupt
eos_restore_interrupt:
	mov 0x4(%esp), %eax
	mov %eax, _eflags
	ret
