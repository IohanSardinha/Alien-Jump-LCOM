#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
//-----PARTE 2 ----------------------------------------------------
int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}
//------------------------------------------------------------------
int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  //Returns error code if the timer selected is not in the range[0,2], since uint8_t is unsigned there's no need to check if its lower then zero
  //Also returns error code if the sys_outb() function to set the timer to read the control word is not successfull
  if(timer > 2 || !sys_outb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)))
    return 1;
  //Get the status for the desired timer and returns if it was successfull or not
  return util_sys_inb(TIMER_0+timer,st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val conf;

  switch(field)
  {
    case all:
    break;
    case 
  }


  timer_print_config(timer,field, conf);

  return 1;
}
