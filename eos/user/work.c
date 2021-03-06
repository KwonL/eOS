#include <core/eos.h>

#define STACK_SIZE 8096
static eos_tcb_t tcb1;
static eos_tcb_t tcb2;
static eos_tcb_t tcb3;
static int8u_t stack1[STACK_SIZE];
static int8u_t stack2[STACK_SIZE];
static int8u_t stack3[STACK_SIZE];
static int8u_t queue1[10];
static int8u_t queue2[10];
eos_mqueue_t mq1;
eos_mqueue_t mq2;

/*
 * these tasks are for project 2
 */
/* task1 function - print number 1 to 20 repeatedly */
void print_number() {
	int i = 0;
	while(++i) {
		printf("%d\n", i);
		eos_schedule(); // 태스크 1 수행 중단, 태스크 2 수행 재개
		if (i == 20) { i = 0; }
	}
}
/* task2 function - print alphabet a to z repeatedly */
void print_alphabet() {
	int i = 96;
	while(++i) {
		printf("%c\n", i);
		eos_schedule(); // 태스크 2 수행 중단, 태스크 1 수행 재개
		if (i == 122) { i = 96; }
	}
}
/////////////////////////////////////////////////

/*
 * these tasks are for project 3
 */
void task1() {
	while (1) { PRINT("A\n"); eos_sleep(0); }
}

void task2() {
	while (1) { PRINT("B\n"); eos_sleep(0); }
}

void task3() {
	while (1) { PRINT("C\n"); eos_sleep(0); }
}

/*
 * these tasks are for project 4
 */
static void sender_task(void *arg) {
	int8u_t *data = "xy";
	while (1) {
		PRINT("send message to mq1\n");
		eos_send_message(&mq1, data, 0) ;
		PRINT("send message to mq2\n");
		eos_send_message(&mq2, data, 0) ;
		eos_sleep(0);
	}
}
static void receiver_task1(void *arg) {
	int8u_t data[2];
	while (1) {
		PRINT("receive message from mq1\n");
		eos_receive_message(&mq1, data, 0);
		PRINT("received message: %s\n", data);
		eos_sleep(0);
	}
}
static void receiver_task2(void *arg) {
	int8u_t data[2];
	while (1) {
		PRINT("receive message from mq2\n");
		eos_receive_message(&mq2, data, 0);
		PRINT("received message: %s\n", data);
		eos_sleep(0);
	}
}

void eos_user_main() {
	// project 2
	// eos_create_task(&tcb1, (addr_t)stack1, 8096, print_number, NULL, 0);
	// eos_create_task(&tcb2, (addr_t)stack2, 8096, print_alphabet, NULL, 0);

	// project 3
	// eos_create_task(&tcb1, stack1, STACK_SIZE, task1, NULL, 1);
	// eos_set_period(&tcb1, 2);
	// eos_create_task(&tcb2, stack2, STACK_SIZE, task2, NULL, 10);
	// eos_set_period(&tcb2, 4);
	// eos_create_task(&tcb3, stack3, STACK_SIZE, task3, NULL, 50);
	// eos_set_period(&tcb3, 8);

	// project 4
	eos_create_task(&tcb1, (addr_t)stack1, 8096, sender_task, NULL, 50);
	eos_create_task(&tcb2, (addr_t)stack2, 8096, receiver_task1, NULL, 10);
	eos_create_task(&tcb3, (addr_t)stack3, 8096, receiver_task2, NULL, 10);
	eos_set_period(&tcb1, 2);
	eos_set_period(&tcb2, 4);
	eos_set_period(&tcb3, 5);
	eos_init_mqueue(&mq1, queue1, 5, 2, FIFO);
	eos_init_mqueue(&mq2, queue2, 5, 2, FIFO);
}
