
#include "graphics.h"

static uint16_t h_res;
static uint16_t v_res;
static uint8_t bits_per_pixel;
static uint8_t mem_model;
static uint8_t *video_mem;
static uint8_t *video_mem_buffer;

static uint8_t GreenMask, RedMask, BlueMask ,posgreen, posred, posblue;
uint16_t vd_mode;

int array_cpy(uint8_t* from, uint8_t* to, int size)
{
    for(int i = 0; i < size; i++)
    {
        to[i] = from[i];
    }
    return 0;
}

void (draw_frame)()
{
    array_cpy(video_mem_buffer,video_mem,h_res*v_res);
}

int (vg_enter)(uint16_t mode){
    vd_mode = mode;
    reg86_t r;
    memset(&r, 0, sizeof(r));
    r.ax = 0x4F02;// VBE call, function 02 -- set VBE mode//
    r.bx = 1<<14|mode; // set bit 14: linear framebuffer
    r.intno = 0x10;
    if( sys_int86(&r) != OK )
    {  
         printf("set_vbe_mode: sys_int86() failed \n");
        return 1;
    }

    return 0;
}

void* (vg_init)(uint16_t mode){
    vbe_mode_info_t vbeModeInfo;
    vbe_get_mode_info(mode, &vbeModeInfo);
    h_res = vbeModeInfo.XResolution;
    v_res = vbeModeInfo.YResolution;
    bits_per_pixel = vbeModeInfo.BitsPerPixel;
    mem_model = vbeModeInfo.MemoryModel;
    GreenMask = vbeModeInfo.GreenMaskSize;
    RedMask = vbeModeInfo.RedMaskSize;
    BlueMask = vbeModeInfo.BlueMaskSize;
    posred = vbeModeInfo.RedFieldPosition;
    posblue =  vbeModeInfo.BlueFieldPosition;
    posgreen =  vbeModeInfo.GreenFieldPosition;

    int r;
    struct minix_mem_range mr;/* physical memory range */
    unsigned int vram_base; /* VRAM’s physical addresss */
    unsigned int vram_size; /* VRAM’s size, but you can use the frame-buffer size, instead */
    //void *video_mem; /* frame-buffer VM address */

    vram_base= vbeModeInfo.PhysBasePtr;
    vram_size = v_res * h_res * (bits_per_pixel /8);

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if(video_mem == MAP_FAILED) panic("couldn’t map video memory");
    video_mem_buffer = malloc(sizeof(uint8_t)*h_res*v_res);

    vg_enter(mode);

    array_cpy(video_mem,video_mem_buffer,h_res*v_res);
    return video_mem;

}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){

    if(x >= h_res || y >= v_res)

        return 1;

    uint16_t bytes = ((bits_per_pixel + 1) / 8);

    uint8_t * pixel = video_mem_buffer;

    pixel += (x + h_res * y) * bytes;

    for (char i = 0; i < bytes; i++)

        * (pixel + i) = color >> (i * 8);

    return 0;

}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {

    for (int i = 0 ; i < width; i++)

        vg_draw_pixel(x + i, y, color);

    return 0;

}



int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

    for (int i = 0 ; i < height; i++)

        vg_draw_hline(x, y + i, width, color);

    return 0;

}


int (draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step)
{
    uint8_t width = h_res / no_rectangles;
    uint8_t height = v_res / no_rectangles;

    for(int i = 0; i < no_rectangles; i++)
    {
        for(int j = 0; j < no_rectangles; j++)
        {
            if (mem_model == 0x4) {
                uint32_t index = (first + (i * no_rectangles + j) * step) % (1 << bits_per_pixel) & 0xFF;
                vg_draw_rectangle((j * width), (i * height), width, height, index);
            }
            else{
                uint8_t red, green, blue;
                red = get_color_bits(first, RedMask, posred);
                green = get_color_bits(first, GreenMask, posgreen);
                blue = get_color_bits(first, BlueMask, posblue);
                red = (red + j * step )% (1 << RedMask);
                blue = (blue + (i+j) * step )% (1 << BlueMask);
                green = (green + i * step )% (1 << GreenMask);
                uint32_t joined_color = (red << posred)| (green << posgreen) | (blue << posblue);
                vg_draw_rectangle((j * width), (i * height), width, height, joined_color);
            }
        }
    }
    return 0;
}


uint8_t (get_color_bits)(uint32_t color, uint8_t size, uint8_t pos) {
    uint8_t res;
    uint32_t mask;
    mask = BIT(size+1)-1;
    mask <<= pos;
    res = (color & mask) >> pos;
    return res;

}

int (draw_sprite)(xpm_map_t xpm, uint16_t x, uint16_t y)
{
    xpm_image_t sprite;
    xpm_load(xpm,XPM_INDEXED,&sprite);
    for(int i = 0; i < sprite.height; i++)
    {
        for(int j = 0; j < sprite.width; j++)
        {
            if(sprite.bytes[i*sprite.width+j] != 0)
                vg_draw_pixel(x+j,y+i,sprite.bytes[i*sprite.width+j]);
        }
    }
    return 0;
}

int (fill_bckgnd)()
{
    return vg_draw_rectangle(0,0,h_res,v_res,0);
}
