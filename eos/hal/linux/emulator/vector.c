#include "private.h"
#include "../emulator.h"
#include <core/eos.h>

/* jump to reset vector. esp := 0 */
int main(int argc, char **argv) {
	PRINT("reset\n");
	_eflags = 0;
	__asm__ __volatile__ ("\
		mov $0x0, %%esp;\
		jmp *%0"
		:: "r"(_vector[0]));
	/* never return here */
	return 0;
}

int32u_t _eflags_saved;

/* deliver an irq to CPU */
void _deliver_irq() {
	//PRINT("_eflags: %d, _irq_pending: 0x%x, _irq_mask: 0x%x\n", _eflags, _irq_pending, _irq_mask);
	_eflags_saved = _eflags;
	if (_irq_pending & ~_irq_mask) {
		if (_eflags == 1) {
			_eflags = 0;
			//PRINT("interrupted\n");
			__asm__ __volatile__ ("\
				push $resume_eip;\
				push %1;\
				jmp *%0;\
			resume_eip:"
				:: "r"(_vector[3]), "r"(_eflags_saved));
		}
	}
}

