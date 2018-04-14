#include "emulator_asm.h"

.data
.global _vector
_vector:
	.long _os_reset_handler
	.long _os_reset_handler
	.long _os_reset_handler
	.long _os_irq_handler

.global _os_init_stack_start
.global _os_init_stack_end
_os_init_stack_start:
	.skip 4096
_os_init_stack_end:



.text
.global _os_reset_handler
_os_reset_handler:
	_CLI
	lea _os_init_stack_end, %esp
	call _os_initialization
	jmp _os_reset_handler

.global _os_irq_handler
_os_irq_handler:
	pusha
	call _os_common_interrupt_handler
	popa
	_IRET
