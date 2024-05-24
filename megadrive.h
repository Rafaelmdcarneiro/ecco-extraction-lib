#ifndef _MAIN_HEADER
    #include "blue-dream.h"
#endif

#define MD_ROMSIGNATURE_OFFSET 0x180

void copy_md16_md32_palette(t_dword *palette, t_dword *md_palette, int total);

void convert32_md_rgba_palette(t_dword *palette, int color_total);
void convert32_rgba_bgra_palette(t_dword *palette, int color_total);
t_dword *create_md16_bgra32_palette(t_byte *md16_palette, int color_total);

t_dword *create_md4_md32_pattern(t_byte *md4_pattern, int size);
void unpack_md4_md32_pattern(t_byte *md4_pattern, t_dword *md32_pattern, int size);