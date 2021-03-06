
#include <lcom/lcf.h>
//#include <liblm.a>
#include <machine/int86.h>
#include <string.h>

#define AH_VBE 0x4F

#define MODE_I 0x105
#define MODE_II 0x110
#define MODE_III 0x115
#define MODE_IV 0x11A
#define MODE_V 0x14C




int (vg_enter)(uint16_t mode);
int (draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint8_t (get_color_bits)(uint32_t color, uint8_t size, uint8_t pos);
int (draw_sprite)(xpm_map_t xpm, uint16_t x, uint16_t y);
int (fill_bckgnd)();
void (draw_frame)();
