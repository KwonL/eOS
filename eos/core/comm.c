/********************************************************
 * Filename: core/comm.c
 *  
 * Author: jtlim, RTOSLab. SNU.
 * 
 * Description: message queue management. 
 ********************************************************/
#include <core/eos.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type) {
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
}
