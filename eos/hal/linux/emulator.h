#ifndef HAL_LINUX_EMUL_H
#define HAL_LINUX_EMUL_H
#include "type.h"

/* start address of vector table 
   _vector[0]: reset vector
   _vector[1]: reserved
   _vector[2]: reserved
   _vector[3]: irq vector */
extern int32u_t _vector[];

/* Description for reset vector. 
  At reset, followings are happen. 
  
  1. interrupt is disabled.
  2. ESP := 0x0
  3. jump to _vector[0]
*/

/* Description for irq vector.
  At irq, followings are heppen.

  1. interrupt is disable.
  2. EIP is pushed.
  3. EFLAGS is pushed.
  4. jump to _vector[3]
*/

/* irq pending register. 
   if n-th bit is 1, then irq n is pending. 
   If user resets n-th bit to 0, then corresponding irq is 
   acknowledged. */
extern int32u_t _irq_pending;

/* irq mask register.
   if n th bit is set to 1, then irq n is masked.
   if n th bit is set to 0, then irq n is unmasked.
   Intially, this register masks all irqs */
extern int32u_t _irq_mask;

extern void _init_timer_interrupt();

#endif
