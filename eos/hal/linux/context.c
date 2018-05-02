#include <core/eos.h>
#include <core/eos_internal.h>

typedef struct _os_context {
	/* low address */
	int32u_t reg_edi;
	int32u_t reg_esi;
	int32u_t reg_ebp;
	int32u_t reg_esp;
	int32s_t reg_ebx;
	int32s_t reg_edx;
	int32s_t reg_ecx;
	int32s_t reg_eax;
	int32s_t reg_eflags;
	int32s_t reg_eip;
	/* high address */	
} _os_context_t;

void print_context(addr_t context) {
	if(context == NULL) return;
	_os_context_t *ctx = (_os_context_t *)context;
	PRINT("edi  =0x%x\n", ctx->reg_edi);
	PRINT("esi  =0x%x\n", ctx->reg_esi);
	PRINT("ebp  =0x%x\n", ctx->reg_ebp);
	PRINT("esp  =0x%x\n", ctx->reg_esp);
	PRINT("ebx  =0x%x\n", ctx->reg_ebx);
	PRINT("edx  =0x%x\n", ctx->reg_edx);
	PRINT("ecx  =0x%x\n", ctx->reg_ecx);
	PRINT("eax  =0x%x\n", ctx->reg_eax);
	PRINT("eflags  =0x%x\n", ctx->reg_eflags);
	PRINT("eip  =0x%x\n", ctx->reg_eip);
}

addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg) {

	int i;
	int32u_t* stack_high = (int32u_t*)(stack_base + stack_size);

	// push argument and ret addr of entry
	*(stack_high - 1) = arg;
	*(stack_high - 2) = NULL;

	// push entry pointer: this will be return address
	*(stack_high - 3) = (int32u_t*)entry;
	// push all registers
	for (i = 4; i < 12; i++) {
		*(stack_high - i) = (int32u_t)0;	
	}

	// for debuging
	// *(stack_high - 12) = 3;
	// PRINT("context address  =0x%x\n", stack_high - 12);
	// PRINT("And content is  =0x%x\n", *(stack_high - 12));
	//////////////////////////////

	// stack top - sizeof(arg) - sizeof(addr) - sizeof(registers)
	return (addr_t)stack_high - 48;
}

void _os_restore_context(addr_t sp) {

	__asm__ __volatile__(
		"movl %0, %%esp;"
		"popa;"
		"popf;"
		"ret;"
		:: "m"(sp)
	);
	
}

addr_t _os_save_context() {

	__asm__ __volatile__(
		"push $resume_point;"
		"pushf;"
		"movl $0, %eax;"
		"pusha;"
		"movl  %esp, %eax;"
		"push 4(%ebp);"
		"push (%ebp);"
		"movl %esp, %ebp;"
		"resume_point:;"
		"leave;"
		"ret;"
	);

	return NULL;
}
