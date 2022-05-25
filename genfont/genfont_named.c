#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <string.h>
#include <ctype.h>


FT_Library ft;
FT_Face fontface;

int char_width, char_height, ascender;


void glyph(unsigned char c)
{
    FT_Load_Glyph(fontface, FT_Get_Char_Index(fontface, c), FT_LOAD_RENDER|FT_LOAD_NO_HINTING);
    
    int width = fontface->glyph->bitmap.width;
    int height = fontface->glyph->bitmap.rows;
    int bearing_x = fontface->glyph->bitmap_left;
    int bearing_y = fontface->glyph->bitmap_top;
    unsigned char *buf = fontface->glyph->bitmap.buffer;
    
    printf("\t[%d] = {\n", c);
    for (int y = 0; y < char_height; y++) {
        printf("\t\t");
            
        if (y >= ascender - bearing_y && y < ascender - bearing_y + height)
            for (int x = 0; x < char_width; x++) {
                if (x >= bearing_x && x < bearing_x + width)
                    printf("%d, ", buf[x - bearing_x + (y - ascender + bearing_y) * width]);
                else
                    printf("0, ");
            }
        else
            for (int x = 0; x < char_width; x++)
                printf("0, ");
        
        printf("\n");
    }
    printf("\t},\n");
}

void setFontsize(int size)
{
    float hdpi, vdpi;
    SDL_Window *win;

    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_HIDDEN);
    SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(win), NULL, &hdpi, &vdpi);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    FT_Set_Char_Size(fontface, 0, size * 64, hdpi, vdpi);
}

char *str_to_upper(const char* str)
{
    char *result_str = malloc(strlen(str));
    assert(result_str != NULL);
    for (char *STR = result_str; *STR = toupper(*str); str++, STR++)
        ;
    return result_str;
}

#define ARG_PATH argv[1]
#define ARG_SIZE argv[2]
#define ARG_NAME argv[3]

int main(int argc, char **argv)
{
    FT_Init_FreeType(&ft);
    
    if (argc != 4) {
        fprintf(stderr, "usage: %s font_path font_size_in_pts font_name\n", argv[0]);
        return 1;
    }
    
    assert(access(ARG_PATH, R_OK) == 0);
    FT_New_Face(ft, ARG_PATH, 0, &fontface);
    setFontsize(atoi(ARG_SIZE));    

    FT_Load_Glyph(fontface, FT_Get_Char_Index(fontface, 'A'), FT_LOAD_RENDER|FT_LOAD_NO_HINTING);
    char_width = fontface->glyph->advance.x >> 6;
    char_height = fontface->size->metrics.height >> 6;
    ascender = fontface->size->metrics.ascender >> 6;

    char *upper_name = str_to_upper(ARG_NAME);

    printf("#pragma once\n");
    printf("#define FONT_%s_WIDTH %d\n", upper_name, char_width);
    printf("#define FONT_%s_HEIGHT %d\n", upper_name, char_height);
    printf("unsigned char font_%s[][FONT_%s_HEIGHT][FONT_%s_WIDTH] = {\n", ARG_NAME, upper_name, upper_name);
        
    for (int i = 0; i < 256; i++)
        glyph(i);

    printf("};\n", char_height, char_width);

    return 0;
}
