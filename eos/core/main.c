/********************************************************
 * Filename: core/main.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: main task is OS.
 * After initialization, this task enters idle loop and never returns.
 ********************************************************/
#include <core/eos.h>
#include <core/eos_internal.h>

static void _os_idle_task(void *arg);	// idle task
static eos_tcb_t idle_task;				// tcb for the idle task
static int8u_t idle_stack[8096];		// stack for the idle task

/*
 * This function is called by HAL after it finished initializing H/W.
 */
void _os_initialization() {
	_os_multitasking = 0;
	eos_disable_interrupt();
	
	// Initialize subsystems.
	_os_init_hal();
	_os_init_icb_table();
	_os_init_scheduler();
	_os_init_task();
	_os_init_timer();

	// Create idle task.
	PRINT("creating idle task.\n");
	eos_create_task(&idle_task, (int32u_t *)idle_stack, 8096, _os_idle_task, NULL, LOWEST_PRIORITY);

	// After finishing initializations by kernel,
	// give users a chance to do application specific initializations.
	extern void eos_user_main();
	eos_user_main();

	// Start multitasking.
	PRINT("finishing initialization. starting multitasking.\n");
	_os_multitasking = 1;
	eos_enable_interrupt();

	eos_schedule();

	// After finishing all initializations, OS enters loop.
	while(1) {}
}

static void _os_idle_task(void *arg) {
	while(1) {}
}
