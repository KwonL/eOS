/********************************************************
 * Filename: core/comm.c
 *  
 * Author: jtlim, RTOSLab. SNU.
 * 
 * Description: message queue management. 
 ********************************************************/
#include <core/eos.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type) {
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_start = queue_start;
    mq->queue_type = queue_type;
    mq->front = queue_start;
    mq->rear = queue_start;

    // initializing semaphore
    eos_init_semaphore(&mq->putsem, queue_size, queue_type);
    eos_init_semaphore(&mq->getsem, 0, queue_type);
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
    // PRINT("Send message called!\n");
    // if failed to acquireing semaphore, just return
    if (eos_acquire_semaphore(&mq->putsem, timeout) == 0) {
        return 0;
    }
    int i;
    /*
     * critical section for message queue
     */
    
    // copy message to mq's rear
    for (i = 0; i < mq->msg_size; i++) {
        *(char *)(mq->rear + i) = *(char *)(message + i);
        // PRINT("message: %c\n", *(char*)(message + i));
        mq->rear++;
        // PRINT("queue_start: 0x%x, rear: 0x%x\n", mq->queue_start, mq->rear);

        // if reach to end of queue, back to start of queue
        if (mq->rear - mq->queue_start == mq->msg_size * mq->queue_size) {
            mq->rear = mq->queue_start;
        }
    }
    // end of critcial section

    eos_release_semaphore(&mq->getsem);
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
    // PRINT("Recieve message called!\n");
    // if failed to acquiring semaphore, just return
    if (eos_acquire_semaphore(&mq->getsem, timeout) == 0) {
        return 0;
    }

    int i;

    /*
     * critical section for message queue
     */
    // copy message from queue
    for (i = 0; i < mq->msg_size; i++) {
        *(char *)(message + i) = *(char *)(mq->front + i);
        // PRINT("message: %c\n", *(char*)(message + i));
        mq->front++;
        // PRINT("queue_start: 0x%x, front: 0x%x\n", mq->queue_start, mq->front);

        if (mq->front - mq->queue_start == mq->msg_size * mq->queue_size) {
            mq->front = mq->queue_start;
        }
    }
    // end of cirtical section

    eos_release_semaphore(&mq->putsem);
}
