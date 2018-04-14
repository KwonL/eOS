/********************************************************
 * Filename: hal/linux/emulator/timer.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: interval timer hardware emulation.
 ********************************************************/
#include <core/eos.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "private.h"

/*
 * ID of The timer to emulate h/w interval timer.
 */
static timer_t* _interval_timer;

/*
 * signal handler.
 */
static void sighandler(int signum, siginfo_t *info, void *context) {
	//PRINT("generate irq (signum: %d, siginfo_t: %d, context: 0x%x)\n", signum, info->si_timerid, context);
	/* generate irq. irq number of timer interrupt is 0 */
	_gen_irq(IRQ_INTERVAL_TIMER0);
}

/*
 * Initializes the interval timer.
 */
void _init_timer_interrupt() {
	/* set handler for SIGALRM */
	struct sigaction action;	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = sighandler;
	if (sigaction(SIGALRM, &action, NULL) != 0) {
		return;
	}

	/* create interval timer */
	if (timer_create(CLOCK_REALTIME, NULL, &_interval_timer) != 0) {
		return;
	}
	/* run timer */
	struct timespec timer_period = {1, 0};
	//struct timespec timer_period1 = {0, 500000000};
	struct itimerspec timer_value = {timer_period, timer_period};
	if (timer_settime(_interval_timer, 0, &timer_value, NULL) != 0) {
		return;
	}
}
