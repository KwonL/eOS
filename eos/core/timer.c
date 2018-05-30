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
	PRINT("SET ALARM CALLED\n");
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
	PRINT("REMOVE ALARM COMPLETE\n");

	// if timeout is 0 or entry is null, just return
	if (timeout == 0 || entry == NULL) return;

	// set alarm
	alarm->timeout = timeout;
	alarm->handler = entry;
	alarm->arg = arg;
	alarm->alarm_queue_node.priority = timeout;
	alarm->alarm_queue_node.ptr_data = alarm;

	// queueing alarm to the counter's queue
	_os_add_node_priority(&(counter->alarm_queue), &(alarm->alarm_queue_node));

	PRINT("SET alarm completely. alarm: 0x%x, queue node: 0x%x\n", alarm, alarm->alarm_queue_node);
}

eos_counter_t* eos_get_system_timer() {
	return &system_timer;
}

void eos_trigger_counter(eos_counter_t* counter) {
	PRINT("tick\n");
	// counter->tick++;

	// // decrease all alarm's timeout
	// _os_node_t* cur_node = counter->alarm_queue;
	// do {
	// 	// queue is empty
	// 	if (cur_node == NULL) break;

	// 	// cur_node->ptr_data->timeout--;
	// 	cur_node = cur_node->next;
	// } while (cur_node != counter->alarm_queue);

	// PRINT("counter->alarm_queue->ptr_data: 0x%x\n", counter->alarm_queue->ptr_data);
	// // // if timeout is zero, then turn off alarm and call entry function
	// // if (counter->alarm_queue->ptr_data->timeout == 0) {
	// // 	// turn off alarm
	// // 	eos_set_alarm(counter, counter->alarm_queue->ptr_data, 0, NULL, NULL);
	// // 	counter->alarm_queue->ptr_data->handler(counter->alarm_queue->ptr_data->arg);
	// // }
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
