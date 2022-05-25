#include <string.h>
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
int yres, xres, xres_visible;

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

void draw_string_bottom_right(const char *str)
{
    draw_string(str, yres - FONT_HEIGHT, xres_visible - strlen(str) * FONT_WIDTH);
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
    xres_visible = info.xres;
    
    size_t len = 4 * xres * yres;
    buf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    assert(buf != MAP_FAILED);
}

int main()
{
    init_fb();
    
    char *line;
    size_t line_size;
    ssize_t line_len;

    while ((line_len = getline(&line, &line_size, stdin)) > 0) {
        line[line_len - 1] = '\0';  /* remove newline character */
        draw_string_bottom_right(line);
    }
    
    return 0;
}
