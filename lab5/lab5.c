// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include "graphics.h"
#include "timer.h"
#include "keyboard.h"
#include <stdint.h>
#include <stdio.h>
#include "i8042.h"


// Any header files included below this line should have been created by you

extern uint8_t data;
extern bool new_scancode;
extern uint32_t timer_count;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    vg_init(mode);
    sleep(delay);
    if (vg_exit()== 1) return 1;
    return 0;

}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

   vg_init(mode);
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;

    keyboard_subscribe_int(&irq_set);
    vg_draw_rectangle(x,y,width,height,color);
    draw_frame();
    while(data != ESCKEY)
    {

        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                        keyboard_IH();
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


    keyboard_unsubscribe_int();
    if (vg_exit()== 1) return 1;

    return 0;

}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    vg_init(mode);
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;

    keyboard_subscribe_int(&irq_set);
   draw_pattern(no_rectangles,first,step);
   draw_frame();
    while(data != ESCKEY)
    {

        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                        keyboard_IH();
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


    keyboard_unsubscribe_int();
    if (vg_exit()== 1) return 1;

    return 0;

}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    vg_init(0x105);
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_set;

    keyboard_subscribe_int(&irq_set);
    draw_sprite(xpm, x, y);
    draw_frame();
    //draw_sprite(Rainbow, x, y);
    while(data != ESCKEY)
    {

        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                        keyboard_IH();
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


    keyboard_unsubscribe_int();
    if (vg_exit()== 1) return 1;

    return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
 vg_init(0x105);
    message msg;
    int ipc_status;
    unsigned long r;
    int irq_kbd;
    uint8_t irq_timer;
    
    xpm_image_t sprite;
    xpm_load(xpm,XPM_INDEXED,&sprite);

    keyboard_subscribe_int(&irq_kbd);
    timer_subscribe_int(&irq_timer);
    while(data != ESCKEY)
    {

        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_kbd) { /* subscribed interrupt */
                        keyboard_IH();
                    }
                    if (msg.m_notify.interrupts & irq_timer) { /* subscribed interrupt */
                        timer_int_handler();
                        if(timer_count%(60/fr_rate) == 0)
                        {
                          vg_draw_rectangle(xi,yi,sprite.width,sprite.height,0);
                          draw_sprite(xpm, xi += speed, yi);
                          draw_frame();
                        }
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


    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    if (vg_exit()== 1) return 1;

    return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}

