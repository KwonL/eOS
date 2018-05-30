/********************************************************
 * Filename: core/task.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: task management.
 ********************************************************/
#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3

/*
 * Queue (list) of tasks that are ready to run.
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/*
 * Pointer to TCB of running task
 */
static eos_tcb_t *_os_current_task;
// static _os_node_t task_node[4];
static int32u_t task_count = 0;

int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority) {
	PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);
	
	// create context and return its context_t pointer
    addr_t context_ptr = _os_create_context(sblock_start, sblock_size, entry, arg);
	// print_context(context_ptr);

	// initializing TCB
	task->sp = context_ptr;
	task->state = READY;
	task->priority = priority;
	task->preiod = 0;
	task->pid = task_count;

	// // enqueue task
	// task_node[task_count].priority = task->priority;
	// task_node[task_count].ptr_data = task;
	
	_os_add_node_tail(_os_ready_queue + priority, task);
	// PRINT("node: 0x%x, sp: 0x%x\n", task, task->sp);

	// //debuging 
	// PRINT("task num is %d\n", task_count);
	// PRINT("ready queue : 0x%x\n", _os_ready_queue[0]);
	// ////////////////////////////////////

	task_count++;

	return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

void eos_schedule() {

	// sp for current_task
	addr_t old_stack_ptr;

	// // for debuging, print all node
	// int i;
	// eos_tcb_t* cur = _os_ready_queue[0];
	// for (i= 0; i < 2; i++) {
	// 	PRINT("current : 0x%x\n", cur);
	// 	PRINT("sp: 0x%x\n", cur->sp);
	// 	cur = cur->next;
	// }

	// some task that is running
	if (_os_current_task != NULL) {
		// save context and resotre next context
		old_stack_ptr = _os_save_context();
		if (old_stack_ptr != NULL) {
			_os_current_task->sp = old_stack_ptr;
			_os_current_task->state = READY;
			// enqueu current task into ready queue
			_os_add_node_tail(_os_ready_queue + _os_current_task->priority, _os_current_task);
			// reallocate current task on ready_queue
			_os_current_task = _os_ready_queue[0];
			_os_remove_node(_os_ready_queue, _os_current_task);

			_os_current_task->state = RUNNING;

			_os_restore_context(_os_current_task->sp);
		} 
		// after resotring context
		else {
			return;
		}
	}
	// there is no task initially
	else {
		_os_current_task = _os_ready_queue[0];
		_os_remove_node(_os_ready_queue, _os_current_task);

		// set status as RUNNING
		_os_current_task->state = RUNNING;
		
		_os_restore_context(_os_current_task->sp);
	}

}

eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}

void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
}

int32u_t eos_get_priority(eos_tcb_t *task) {
}

void eos_set_period(eos_tcb_t *task, int32u_t period){
	task->period = period;
}

int32u_t eos_get_period(eos_tcb_t *task) {
}

int32u_t eos_suspend_task(eos_tcb_t *task) {
}

int32u_t eos_resume_task(eos_tcb_t *task) {
}

void eos_sleep(int32u_t tick) {
}

void _os_init_task() {
	PRINT("initializing task module.\n");

	/* init current_task */
	_os_current_task = NULL;

	/* init multi-level ready_queue */
	int32u_t i;
	for (i = 0; i < LOWEST_PRIORITY; i++) {
		_os_ready_queue[i] = NULL;
	}
}

void _os_wait(_os_node_t **wait_queue) {
}

void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_sleeping_task(void *arg) {
}
