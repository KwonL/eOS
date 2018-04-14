#ifndef HAL_LINUX_EMUL_ASM_H
#define HAL_LINUX_EMUL_ASM_H

/* clear interrupt */
#define _CLI \
	movl $0, _eflags;

/* set interrupt */
#define _STI \
	movl $1, _eflags;\
	call _deliver_irq;

/* push eflags into stack */
#define _PUSHF \
	push _eflags;

/* pop eflags from stack */
#define _POPF \
	pop _eflags;\
	call _deliver_irq;	

/* interrupt return. */
#define _IRET \
	pop _eflags;\
	call _deliver_irq;\
	ret;

#endif
