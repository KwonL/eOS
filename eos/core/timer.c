/********************************************************
 * Filename: core/timer.c
 *
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: 
 ********************************************************/
#include <core/eos.h>

static eos_counter_t system_timer;

int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value) {
	counter->tick = init_value;
	counter->alarm_queue = NULL;
	return 0;
}

void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg) {
	// PRINT("SET ALARM CALLED\n");
	// remove alarm from queue
	_os_node_t* cur_node = counter->alarm_queue;
	do {
		if (cur_node == NULL) break;
		if (cur_node->ptr_data == alarm) {
			_os_remove_node(&counter->alarm_queue, &alarm->alarm_queue_node);
			break;
		}
		cur_node = cur_node->next;
	} while (cur_node != counter->alarm_queue); 
	// there is no matching alarm in queue

	// if timeout is 0 or entry is null, just return
	if (timeout == 0 || entry == NULL) {
		// PRINT("REMOVE ALARM COMPLETE\n");
		return;
	} 

	// set alarm
	alarm->timeout = timeout;
	alarm->handler = entry;
	alarm->arg = arg;
	alarm->alarm_queue_node.priority = timeout;
	alarm->alarm_queue_node.ptr_data = alarm;

	// queueing alarm to the counter's queue
	_os_add_node_priority(&(counter->alarm_queue), &(alarm->alarm_queue_node));

	// PRINT("SET alarm completely. alarm: 0x%x, queue node: 0x%x\n", alarm, alarm->alarm_queue_node);
}

eos_counter_t* eos_get_system_timer() {
	return &system_timer;
}

void eos_trigger_counter(eos_counter_t* counter) {
	PRINT("tick %d\n", counter->tick);
	counter->tick++;

	// for debugging
	_os_node_t* cur_node = counter->alarm_queue;
	eos_alarm_t* cur_alarm = cur_node->ptr_data;
	// PRINT("alarm queue: 0x%x, first alarm: 0x%x\n", cur_node, cur_node->ptr_data);
	// PRINT("alarm's timeout: %d\n", cur_alarm->timeout);

	// decrease all alarm's timeout
	do {
		// queue is empty
		if (cur_node == NULL) break;
		
		// decrease timeout
		cur_alarm->timeout--;
		// if timeout occur, enqueueing task
		if (cur_alarm->timeout == 0) {
			cur_alarm->handler(cur_alarm->arg);
		}

		// move cussor to next block
		cur_node = cur_node->next;
		cur_alarm = cur_node->ptr_data;
	} while (cur_node != counter->alarm_queue);
}

/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg) {
	/* trigger alarms */
	eos_trigger_counter(&system_timer);
}

void _os_init_timer() {
	eos_init_counter(&system_timer, 0);

	/* register timer interrupt handler */
	eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0, timer_interrupt_handler, NULL);
}
