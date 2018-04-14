#include <core/eos.h>

static eos_tcb_t tcb1;
static eos_tcb_t tcb2;
static int8u_t stack1[8096];
static int8u_t stack2[8096];

static void print_number(void *arg) {
	while (1) {
	}
}

static void print_alphabet(void *arg) {
	while (1) {
	}
}

void eos_user_main() {
	eos_create_task(&tcb1, (addr_t)stack1, 8096, print_number, NULL, 0);
	eos_create_task(&tcb2, (addr_t)stack2, 8096, print_alphabet, NULL, 0);
}
