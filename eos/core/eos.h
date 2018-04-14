/********************************************************
 * Filename: core/eos.h
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: 
 ********************************************************/
#ifndef EOS_H
#define EOS_H
#include <core/eos_internal.h>

/********************************************************
 * Define  
 ********************************************************/
/*
 * wait queue type (semaphore, condition variable, message queue) 
 */
#define FIFO 0
#define PRIORITY 1

/********************************************************
 * Debugging Module
 ********************************************************/

void eos_printf(const char *fmt, ...);

#define PRINT(format, a...) eos_printf("[%15s:%30s] ", __FILE__, __FUNCTION__); eos_printf(format, ## a);

/********************************************************
 * Interrupt Management Module
 ********************************************************/

/*
 * Interrupt handler type.
 */
typedef void (*eos_interrupt_handler_t)(int8s_t irq_num, void *arg);

/*
 * Registers interrupt handler with the given irq number.
 * irqnum: irq number to install handler for.
 * handler: pointer to the function that will handle the interrupt.
 *          NULL is you want to unregister handler.
 * arg: the argument that will be delivered to the handler when interrupt
 *          is occurred.
 */
extern int8s_t eos_set_interrupt_handler(int8s_t irqnum, eos_interrupt_handler_t handler, void *arg);

/*
 * Returns interrupt handler installed for an irq.
 */
extern eos_interrupt_handler_t eos_get_interrupt_handler(int8s_t irqnum);


/********************************************************
 * Timer Management Module
 ********************************************************/

typedef struct eos_counter {
	int32u_t tick;
	_os_node_t *alarm_queue;
} eos_counter_t;

typedef struct eos_alarm {
	int32u_t timeout;
	void (*handler)(void *arg);
	void *arg;
	_os_node_t alarm_queue_node;
} eos_alarm_t;

extern int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value);
extern void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg);
extern eos_counter_t* eos_get_system_timer();
extern void eos_trigger_counter(eos_counter_t* counter);


/********************************************************
 * Task Management Module
 ********************************************************/

/* The TCB (task control block) structure */
typedef struct tcb {
} eos_tcb_t;

/*
 * Create a task.
 * User should allocate memory for the tcb structure before calling this function.
 */
extern int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority);

/*
 * Destoroys a task.
 */
extern int32u_t eos_destroy_task(eos_tcb_t *task);

extern void eos_schedule();

/*
 * Returns a tcb of the currently running task.
 */
extern eos_tcb_t *eos_get_current_task();

/*
 * Sets priority of the specified task.
 * task: TCB of the task.
 * priority: new priority
 */
extern void eos_change_priority(eos_tcb_t *task, int32u_t priority);

/*
 * Returns priority of the specified task.
 * task: TCB of the task.
 */
extern int32u_t eos_get_priority(eos_tcb_t *task);

/*
 * Sets period of the specified task.
 *
 * task: the TCB of task
 * period: period of the task in tick unit.
 *         if period is 0, this means the task is non-periodic.
 */
extern void eos_set_period(eos_tcb_t *task, int32u_t period);

extern int32u_t eos_get_period(eos_tcb_t *task);

extern int32u_t eos_suspend_task(eos_tcb_t *task);

extern int32u_t eos_resume_task(eos_tcb_t *task);

extern void eos_sleep(int32u_t tick);


/********************************************************
 * Synchronization Module
 ********************************************************/

/*
 * The Semaphore structure
 */
typedef struct eos_semaphore {
} eos_semaphore_t;

/*
 * Initialized Semaphore.
 * User should allocate memory for the semaphore structure before calling this function
 */
extern void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type);

/*
 * Try to acquire semaphore.
 */
extern int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout);

/*
 * Release semaphore.
 */
extern void eos_release_semaphore(eos_semaphore_t *sem);

/*
 * The Condition structure.
 */
typedef struct eos_condition {
	_os_node_t *wait_queue;
	int8u_t queue_type;
} eos_condition_t;

/*
 * Initialized Condition.
 * User should allocate memory for the semaphore structure before calling this function.
 */
extern void eos_init_condition(eos_condition_t *cond, int32u_t queue_type);

/*
 * Wait, because now is not the condition.
 */
extern void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex);

/*
 * Notify a task. move one task from wait_queue to ready_queue.
 */
extern void eos_notify_condition(eos_condition_t *cond);


/********************************************************
 * Hardware Abstraction Module
 ********************************************************/

/*
 * Returns IRQ number that is currently being serviced.
 * -1 means no IRQ is being serviced.
 */
extern int32s_t eos_get_irq(void);

/*
 * Acknowledges the given irq.
 */
extern void eos_ack_irq(int32u_t irq);

/*
 * Enables global interrupt service.
 * Compared to the _os_interrupt_enable() function, 
 * this fuction always enables interrupt.
 */
extern void eos_enable_interrupt(void);

/*
 * Disables global interrupt service.
 * This function returns status of previous interrupt flag.
 */
extern int32u_t eos_disable_interrupt(void);

/*
 * Enables global interrupt service based on the flag.
 */
extern void eos_restore_interrupt(int32u_t flag);

/*
 * enables specific irq line
 */
extern void eos_enable_irq_line(int32u_t irq);

/*
 * disables specific irq line
 */
extern void eos_disable_irq_line(int32u_t irq);


/********************************************************
 * Message Queue Module 
 ********************************************************/

/*
 * The Message Queue structure
 */
typedef struct eos_mqueue {
} eos_mqueue_t;

/*
 * Initialize message queue.
 * User should allocate memory for the message queue structure before calling this function
 */
extern void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type);

/*
 * Try to send a message.
 */
extern int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout);

/*
 * Try to recieve a message.
 */
extern int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout);

#endif /*EOS_H*/
