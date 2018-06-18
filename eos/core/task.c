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
	task->period = 0;
	task->pid = task_count;

	// // enqueue task
	// task_node[task_count].priority = task->priority;
	// task_node[task_count].ptr_data = task;

	// set alarm for corresponding alarm node
	task->task_alarm.alarm_queue_node.ptr_data = &task->task_alarm;
	task->task_alarm.handler = entry;
	task->task_alarm.arg = arg;
	// PRINT("PID: %d, alarm: 0x%x\n", task->pid, &task_alarm[task->pid]);
	// PRINT("alarm's timeout: %d\n", task_alarm[task->pid].timeout);

	// initializing ready queue node
	task->ready_queue_node.next = NULL;
	task->ready_queue_node.previous = NULL;
	task->ready_queue_node.priority = priority;
	task->ready_queue_node.ptr_data = task;

	task->sem_wait_queue_node.next = NULL;
	task->sem_wait_queue_node.previous = NULL;
	task->sem_wait_queue_node.priority = priority;
	task->sem_wait_queue_node.ptr_data = task;
    
	// Add to ready queue
	_os_add_node_tail(_os_ready_queue + priority, &task->ready_queue_node);

	// set ready for scheduler
	_os_set_ready(priority);
	// PRINT("hightest priority: %d\n", _os_get_highest_priority());

	// PRINT("node: 0x%x, sp: 0x%x\n", task, task->sp);

	// //debuging 
	// PRINT("task num is %d\n", task_count);
	// PRINT("ready queue : 0x%x\n", _os_ready_queue[0]);
	// ////////////////////////////////////

	task_count++;

	// PRINT("END OF CREATE TASK\n");

	return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

void eos_schedule() {
	// PRINT("SCHEDULE CALLED\n");
	// sp for current_task
	addr_t old_stack_ptr = NULL;
	int8u_t next_priority = 63;

	// for debugging
	eos_tcb_t* in_ready = NULL;
	if (_os_current_task != NULL)
		in_ready = _os_ready_queue[_os_current_task->priority];

	// some task that is running
	if (_os_current_task != NULL) {
		// save context and resotre next context
		old_stack_ptr = _os_save_context();
		// PRINT("context saved\n");
		if (old_stack_ptr != NULL) {
			_os_current_task->sp = old_stack_ptr;
			if (_os_current_task->state == RUNNING) {
				// PRINT("saving context...\n");
				_os_current_task->state = READY;

				// PRINT("_os_current_task's next: 0x%x, prev: 0x%x\n", _os_current_task->next, _os_current_task->previous);
				// enqueu current task into ready queue
				_os_add_node_tail(_os_ready_queue + _os_current_task->priority, &_os_current_task->ready_queue_node);
				_os_set_ready(_os_current_task->priority);
			} else {
				// PRINT("this task goto sleep\n");
				// PRINT("_os_current_task's next: 0x%x, prev: 0x%x\n", _os_current_task->next, _os_current_task->previous);
			}
			// set alarm
			// eos_set_alarm(eos_get_system_timer(), &_os_current_task->task_alarm, _os_current_task->period, &_os_wakeup_sleeping_task, _os_current_task);

			while (next_priority == 63) { 
				next_priority = _os_get_highest_priority();
			}
			// PRINT("Now, priority: %d\n", next_priority);

			// get hightest priority task
			_os_current_task = _os_ready_queue[next_priority]->ptr_data;
			_os_remove_node(_os_ready_queue + _os_current_task->priority, &_os_current_task->ready_queue_node);
			// PRINT("_OS_ready_queue: 0x%x\n", _os_ready_queue[_os_current_task->priority]);

			// for debugging
			// eos_tcb_t* temp_tcb = _os_ready_queue[_os_current_task->priority];
			// do {
			// 	PRINT("curr priority ready queue: 0x%x\n", temp_tcb); 
			// 	temp_tcb = temp_tcb->next;
			// } while (temp_tcb != _os_ready_queue[_os_current_task->priority]);

			if (_os_ready_queue[_os_current_task->priority] == NULL) {
				_os_unset_ready(_os_current_task->priority);
				// PRINT("UNSET READY!: %d\n", _os_current_task->priority);
			}

			_os_current_task->state = RUNNING;

			_os_restore_context(_os_current_task->sp);
		} 
		// after resotring context
		else {
			// PRINT("return from restore\n");
			return;
		}
	}
	// there is no task initially
	else {
		// PRINT("no initial task\n");
		_os_current_task = (*(_os_ready_queue + _os_get_highest_priority()))->ptr_data;
		_os_remove_node(_os_ready_queue + _os_current_task->priority, &_os_current_task->ready_queue_node);

		// PRINT("no initial task\n");
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
	// PRINT("SLEEP CALLED\n");

	eos_counter_t* system_timer = eos_get_system_timer();
	eos_tcb_t* current_task = eos_get_current_task();

	// PRINT("SYSTIMER: 0x%x, timer's queue: 0x%x\n", system_timer, system_timer->alarm_queue);
	// set alarm and call schedule to yeild CPU
	if (tick == 0) 
		eos_set_alarm(system_timer, &_os_current_task->task_alarm, current_task->period, _os_wakeup_sleeping_task, current_task);
	else {
		eos_set_alarm(system_timer, &_os_current_task->task_alarm, tick, _os_wakeup_sleeping_task, current_task);
	}
	
	if (_os_ready_queue[current_task->priority] == NULL) {
		_os_unset_ready(current_task->priority);
		
		// PRINT("UNSET READY!: %d\n", current_task->priority);
	}

	current_task->state = WAITING;
	// PRINT("SLEEP COMPLETE\n");
	eos_schedule();
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
	// task
	eos_tcb_t* task = arg;
	task->state = READY;

	// PRINT("WAKEUP CALLED: task %d\n", task->pid);

	// queueing task into reqdy queue
	_os_add_node_tail(_os_ready_queue + task->priority, &task->ready_queue_node);
	// set ready at bitmap
	_os_set_ready(task->priority);
	// PRINT("NOW PROCESS's priority: 0x%x\n", task->priority);
	// PRINT("WAKEUP: NEXT HIGHTEST: 0x%x\n", _os_get_highest_priority());
}
