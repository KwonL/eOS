/********************************************************
 * Filename: core/eos_internal.h
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description:
 ********************************************************/
#ifndef EOS_INTERNAL_H
#define EOS_INTERNAL_H
#include <stdio.h>
#include <stdarg.h>
#include <hal/current/type.h>
#include <hal/current/include.h>

/********************************************************
 * Initialization Module
 ********************************************************/

bool_t _os_multitasking;
extern void _os_initialization();	// Start to initialize eOS
extern void _os_init_hal();			// Initialize HAL
extern void _os_init_icb_table();	// Initialize ICB table structure
extern void _os_init_scheduler();	// Initialize bitmap scheduler module
extern void _os_init_task();		// Initialize task management module
extern void _os_init_timer();		// Initialize timer management module


/********************************************************
 * Common Module
 ********************************************************/

/* Common structure for list element */
typedef struct _os_node {
	struct _os_node *previous;
	struct _os_node *next;
	void *ptr_data;
	int32u_t priority;
} _os_node_t;

/* Add the specified node at the end (tail) of the list */
extern void _os_add_node_tail(_os_node_t **head, _os_node_t *new_node);

/* Add the specified node by its priority */
extern void _os_add_node_priority(_os_node_t **head, _os_node_t *new_node);

/* Removes a node from the list */
extern int32u_t _os_remove_node(_os_node_t **head, _os_node_t *node);

/* Formatted output conversion */
int32s_t vsprintf(char *buf, const char *fmt, va_list args);

extern void _os_serial_puts(const char *s);


/********************************************************
 * Interrupt Management Module
 ********************************************************/

/* maximum number of IRQs */
#define IRQ_MAX 32

/*
 * The common interrupt handler.
 * This handler will be invoked by HAL at everytime when an interrupt occurrs.
 */
extern void _os_common_interrupt_handler(void);


/********************************************************
 * Timer Management Module
 ********************************************************/
extern void _os_init_timer();


/********************************************************
 * Task Management Module
 ********************************************************/
extern void _os_wait(_os_node_t **wait_queue);
extern void _os_wakeup_single(_os_node_t **wait_queue, int32u_t suspend_type);
extern void _os_wakeup_all(_os_node_t **wait_queue, int32u_t suspend_type);
extern void _os_wakeup_sleeping_task(void *arg);


/********************************************************
 * Scheduler Module
 ********************************************************/

#define LOWEST_PRIORITY		63
#define MEDIUM_PRIORITY		32
#define READY_TABLE_SIZE	(LOWEST_PRIORITY / 8 + 1)
#define LOCKED			0
#define UNLOCKED		1

/* ready list */
int8u_t _os_ready_group;
int8u_t _os_ready_table[READY_TABLE_SIZE];

/* scheduler lock */
int8u_t _os_scheduler_lock;
int8u_t _os_lock_scheduler(void);
void _os_restore_scheduler(int8u_t);

/* Get the highest prioity from ready list */
int32u_t _os_get_highest_priority();

/* Set priority bit in ready list to 0 */
void _os_unset_ready(int8u_t priority);

/* Set priority bit in ready list to 1 */
void _os_set_ready(int8u_t priority);


/********************************************************
 * Hardware Abstraction Module
 ********************************************************/

/*
 * Create an initial context on the stack.
 * The stack_pointer is assumed as a lowest memory of the stack area.
 * The stack_size is a size of a stack.
 * 
 * If this context is resumed, the entry function will be called
 * with the argument given by the arg.
 * 
 * This function returns task context that it created.
 */
extern addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *arg), void *arg);

/*
 * Saves context of current task to stack.
 * The address of the saved context is returned after saving context.
 *
 * This method returns two times.
 * First it returns after it performs saving context to the stack.
 * In this case, return value is the address of the saved context.
 *
 * Second it returns by other task.
 * In this case, return valus is 0 (NULL).
 */
extern addr_t _os_save_context();

/*
 * Replaces CPU registers with the given context.
 */
extern void _os_restore_context(addr_t sp);

#endif /* EOS_INTERNAL_H */
