#include <inttypes.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>

#include <time.h>
#include <unistd.h>

#include "unifont24.h"

uint32_t *buf;
int yres, xres;

#define COLOR(clr, alpha)                                               \
    (((((clr) & 0xff) * (alpha)) / 255)                                 \
     + ((((((clr) & 0xff00)  >> 8) * (alpha)) / 255) << 8)              \
     + ((((((clr) & 0xff0000) >> 16) * (alpha)) / 255) << 16))

uint32_t drawing_color = 0xee11fe;
void draw_char(unsigned char c, int top, int left)
{
    for (int y = top; y < top + FONT_HEIGHT; y++)
        for (int x = left; x < left + FONT_WIDTH; x++)
            buf[x + xres * y] = COLOR(drawing_color, font[c][y - top][x - left]);
}

void draw_string(const char *str, int top, int left)
{
    for (const char *s = str; *s; s++)
        draw_char(*s, top, left), left += FONT_WIDTH;
}

void draw_current_time()
{
    static char str[9];
    time_t t = time(NULL);
    struct tm *tt = localtime(&t);
    strftime(str, sizeof(str), "%H:%M:%S", tt);

    draw_string(str, yres - FONT_HEIGHT, xres - 9 * FONT_WIDTH);
}

void init_fb()
{
    int fb = open("/dev/fb0", O_RDWR);
    assert(fb > 0);
    
    struct fb_var_screeninfo info;
    assert(0 == ioctl(fb, FBIOGET_VSCREENINFO, &info));
    struct fb_fix_screeninfo f_info;
    assert(0 == ioctl(fb, FBIOGET_FSCREENINFO, &f_info));

    yres = info.yres;
    xres = f_info.line_length / 4;
    
    size_t len = 4 * xres * yres;
    buf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    assert(buf != MAP_FAILED);
}

int main()
{
    init_fb();

    while (1) {
        draw_current_time();
        sleep(1);
    }
    
    return 0;
}
