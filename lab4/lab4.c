// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "timer.h"
#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "i8042.h"


extern uint8_t data;
extern uint32_t timer_count;
extern state_t st;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int ret = 0;
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;
    int mouseByte = 0;
    uint8_t data3bytes[3];

    
    ret |= mouse_subscribe_int(&irq_set);
    ret |=setMouseIRQ(DISABLED);
    ret |= mouse_write_command(STREAM_MODE);
    ret |= mouse_write_command(ENABLE_DATA_REPORT);
    ret |=setMouseIRQ(ENABLED);

    while((ret == 0) && (cnt > 0))
    { 
       /* Get a request message. */
       if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) 
       { 
           printf("driver_receive failed with: %d", r);
          continue;
       }

      if (is_ipc_notify(ipc_status)) /* received notification */ 
      { 
          switch (_ENDPOINT_P(msg.m_source)) 
          {
              case HARDWARE: /* hardware interrupt notification */
                  if (msg.m_notify.interrupts & irq_set)/* subscribed interrupt */ 
                  { 
                      mouse_ih();
                      switch(mouseByte)
                      {
                        case 0:
                          data3bytes[0] = data;
                          mouseByte = 1;
                          break;
                        case 1:
                          data3bytes[1] = data;
                          mouseByte = 2;
                          break;
                        case 2:
                          data3bytes[2] = data;
                          mouseByte = 0;
                          cnt--;
                          if (!(data3bytes[0] & BIT(3)))
                            continue;
                          struct packet pckt;
                          ret |= createMousePacket(&pckt,data3bytes);
                          mouse_print_packet(&pckt);
                          break;
                      }
                  }

              default:
                  break; /* no other notifications expected: do nothing */ 
          }
      } 
      else 
      { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
    }

    ret |= setMouseIRQ(DISABLED);
    ret |= mouse_write_command(DISABLE_DATA_REPORT);
    ret |= setMouseIRQ(ENABLED);
    ret |= mouse_unsubscribe_int();
    return ret;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    uint8_t data3bytes[3];
    int mouseByte = 0;
    int ret = 0;
    while((ret == 0) && (cnt > 0))
    {
      switch(mouseByte)
      {
        case 0:
          ret |= mouse_write_command(MOUSE_READ_DATA);
          mouse_read_out_buff();
          data3bytes[0] = data;
          mouseByte = 1;
          break;

        case 1:
          mouse_read_out_buff();
          data3bytes[1] = data;
          mouseByte = 2;
          break;

        case 2:
        {
          struct packet pckt;
          mouse_read_out_buff();
          data3bytes[2] = data;
          mouseByte = 0;
          cnt--;
          if (!(data3bytes[0] & BIT(3)))
            continue;
          createMousePacket(&pckt,data3bytes);
          mouse_print_packet(&pckt);
          tickdelay(micros_to_ticks(period * 1000));
          break;
        }
      }
    }

    ret |= mouse_write_command(STREAM_MODE);
    ret |= mouse_write_command(DISABLE_DATA_REPORT);
    ret |= set_kbc_cmd_byte_dflt();

    return ret;
}

int (mouse_test_async)(uint8_t idle_time) {
    int ret = 0;
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;
    int mouseByte = 0;
    uint8_t data3bytes[3];
    uint8_t irq_set_timer;


    timer_subscribe_int(&irq_set_timer);

    ret |= mouse_subscribe_int(&irq_set);
    ret |=setMouseIRQ(DISABLED);
    ret |= mouse_write_command(STREAM_MODE);
    ret |= mouse_write_command(ENABLE_DATA_REPORT);
    ret |=setMouseIRQ(ENABLED);

    while((ret == 0) && timer_count/60 < idle_time)
    {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) /* received notification */
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set)/* subscribed interrupt */
                    {   timer_count=0;
                        mouse_ih();
                        switch(mouseByte)
                        {
                            case 0:
                                /*if((mouseByte == 0) && (data & BIT(3)))
                                  continue;*/
                                data3bytes[0] = data;
                                mouseByte = 1;
                                break;
                            case 1:
                                data3bytes[1] = data;
                                mouseByte = 2;
                                break;
                            case 2:
                                data3bytes[2] = data;
                                mouseByte = 0;
                                if (!(data3bytes[0] & BIT(3)))
                                  continue;
                                struct packet pckt;
                                ret |= createMousePacket(&pckt,data3bytes);
                                mouse_print_packet(&pckt);
                                break;
                        }
                    }
                    else if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                        timer_int_handler(); // adds 1 to counter
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        }
        else
        { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    ret |= setMouseIRQ(DISABLED);
    ret |= mouse_write_command(DISABLE_DATA_REPORT);
    ret |= setMouseIRQ(ENABLED);
    ret |= mouse_unsubscribe_int();
    timer_unsubscribe_int();
    return ret;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    int ret = 0;
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;
    int mouseByte = 0;
    uint8_t data3bytes[3];
    
    ret |= mouse_subscribe_int(&irq_set);
    ret |=setMouseIRQ(DISABLED);
    ret |= mouse_write_command(STREAM_MODE);
    ret |= mouse_write_command(ENABLE_DATA_REPORT);
    ret |=setMouseIRQ(ENABLED);

    while((ret == 0) && (st != CMP))
    { 
       /* Get a request message. */
       if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) 
       { 
           printf("driver_receive failed with: %d", r);
          continue;
       }

      if (is_ipc_notify(ipc_status)) /* received notification */ 
      { 
          switch (_ENDPOINT_P(msg.m_source)) 
          {
              case HARDWARE: /* hardware interrupt notification */
                  if (msg.m_notify.interrupts & irq_set)/* subscribed interrupt */ 
                  { 
                      mouse_ih();
                      switch(mouseByte)
                      {
                        case 0:
                          data3bytes[0] = data;
                          mouseByte = 1;
                          break;
                        case 1:
                          data3bytes[1] = data;
                          mouseByte = 2;
                          break;
                        case 2:
                          data3bytes[2] = data;
                          mouseByte = 0;
                          if (!(data3bytes[0] & BIT(3)))
                            continue;
                          struct packet pckt;
                          ret |= createMousePacket(&pckt,data3bytes);
                          mouse_print_packet(&pckt);
                          state_machine(&pckt,x_len,tolerance);
                          break;
                      }
                  }

              default:
                  break; /* no other notifications expected: do nothing */ 
          }
      } 
      else 
      { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
    }

    ret |= setMouseIRQ(DISABLED);
    ret |= mouse_write_command(DISABLE_DATA_REPORT);
    ret |= setMouseIRQ(ENABLED);
    ret |= mouse_unsubscribe_int();
    return ret;
}
