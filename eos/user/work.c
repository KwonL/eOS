#include <core/eos.h>

static eos_tcb_t tcb1;
static eos_tcb_t tcb2;
static int8u_t stack1[8096];
static int8u_t stack2[8096];

/* task1 function - print number 1 to 20 repeatedly */
void print_number() {
	int i = 0;
	while(++i) {
		printf("%d\n", i);
		eos_schedule(); // 태스크 1 수행 중단, 태스크 2 수행 재개
		if (i == 4) { i = 0; }
	}
}
/* task2 function - print alphabet a to z repeatedly */
void print_alphabet() {
	int i = 96;
	while(++i) {
		printf("%c\n", i);
		eos_schedule(); // 태스크 2 수행 중단, 태스크 1 수행 재개
		if (i == 100) { i = 96; }
	}
}

void eos_user_main() {
	eos_create_task(&tcb1, (addr_t)stack1, 8096, print_number, NULL, 0);
	eos_create_task(&tcb2, (addr_t)stack2, 8096, print_alphabet, NULL, 0);
}
