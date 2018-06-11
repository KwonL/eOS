/********************************************************
 * Filename: core/sync.c
 * 
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: semaphore, condition variable management.
 ********************************************************/
#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type) {
	/* initialization */
	sem->count = initial_count;
	sem->wait_queue = NULL;
	sem->queue_type = queue_type;
}

int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout) {
	// PRINT("Acquire semaphore!\n");
	// first, disable interrupt to make this function atomic
	eos_disable_interrupt();

	eos_tcb_t* current_task = eos_get_current_task();
	
	// retry until success to get sem
	do {
		// if there is sufficient resource
		if (sem->count > 0) {
			sem->count--;
			eos_enable_interrupt();
			
			return 1;
		}
		// there is no resource
		else {
			// failed to obtain semaphore
			if (timeout == -1) { 
				eos_enable_interrupt();
				return 0;
			}
			// waiting until other task release sem
			else if (timeout == 0) {
				// FIFO queue
				if (sem->queue_type == 0) {
					// queueing task block to wait queue
					_os_add_node_tail(&sem->wait_queue, current_task);
					// yield CPU
					eos_sleep(0);
				}
				// Priority-based queue
				else {
					// queueing task block to wait queue
					_os_add_node_priority(&sem->wait_queue, current_task);
					// yield CPU
					eos_sleep(0);
				}
			}
			// waiting until timeout
			else {
				eos_sleep(0);
			}
		}
	} while (1);
}

void eos_release_semaphore(eos_semaphore_t *sem) {
	// PRINT("Release semaphore!\n");
	// first, disable interrupt to make this function atomic
	eos_disable_interrupt();

	eos_tcb_t* next_task = sem->wait_queue;

	// PRINT("next task: 0x%x\n", next_task);
	sem->count++;
	
	if (next_task != NULL) {
		// remove next task from sem's waiting queue
		_os_remove_node(&sem->wait_queue, next_task);

		_os_wakeup_sleeping_task(next_task);
		PRINT("wake UP task!: %d\n", next_task->pid);
	}
	
	eos_enable_interrupt();
}

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type) {
	/* initialization */
	cond->wait_queue = NULL;
	cond->queue_type = queue_type;
}

void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex) {
	/* release acquired semaphore */
	eos_release_semaphore(mutex);
	/* wait on condition's wait_queue */
	_os_wait(&cond->wait_queue);
	/* acquire semaphore before return */
	eos_acquire_semaphore(mutex, 0);
}

void eos_notify_condition(eos_condition_t *cond) {
	/* select a task that is waiting on this wait_queue */
	_os_wakeup_single(&cond->wait_queue, cond->queue_type);
}
