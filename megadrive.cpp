#include "blue-dream.h"

t_dword convert_md_rgba_channel(t_dword x)
{
	x = lbyte(x);
	return x << 4 | x << 1 | x >> 2;
}

t_dword convert2_md_rgba_channel(double x)
{
	x = (double)lbyte((t_dword)x);
	x = (255.0f / 7.0f) * (x / 2);

	return round(x);
}

void copy_md16_md32_palette(t_dword *md16_palette, t_dword *md32_palette, int color_total)
{
	int index;
	int r, g, b, a;

	for (index = 0; index < color_total; index++)
	{
		md16_palette[index] = bswap(md16_palette[index]);

		r = md16_palette[index] & 0x000f;
		g = md16_palette[index] & 0x00f0;
		b = md16_palette[index] & 0x0f00;
		a = md16_palette[index] & 0xf000;

		g = g << 4;
		b = b << 8;
		a = a << 16;

		md32_palette[index] = r | g | b | a;
	}
}

void convert32_md_rgba_palette(t_dword *palette, int color_total)
{
	int index;
	t_dword r, g, b, a;

	for (index = 0; index < color_total; index++)
	{
		r = palette[index];
		g = palette[index] >> 8;
		b = palette[index] >> 16;
		a = palette[index] >> 24;

		r = lbyte(r);
		g = lbyte(g);
		b = lbyte(b);
		a = lbyte(a);

		r = convert_md_rgba_channel(r);
		g = convert_md_rgba_channel(g);
		b = convert_md_rgba_channel(b);
		a = convert_md_rgba_channel(a);

		palette[index] = r | g << 8 | b << 16 | 0xff000000;
	}
}

void convert32_rgba_bgra_palette(t_dword *palette, int color_total)
{
	int index;
	t_byte r, b;

	for (index = 0; index < color_total; index++)
	{
		r = palette[index];
		b = palette[index] >> 16;

		r = lbyte(r);
		b = lbyte(b);

		palette[index] &= 0xffffff00;
		palette[index] &= 0xff00ffff;

		palette[index] |= b;
		palette[index] |= r << 16;
	}
}

t_dword *create_md16_bgra32_palette(t_byte *md16_palette, int color_total)
{
	t_dword *md32_palette = m_array(t_dword, color_total);
	if (md32_palette == NULL) return NULL;

	int index = 0, z = 0;
	for (index = 0; index < color_total; index++)
	{
		z = unpack_word(md16_palette, index * 2);
		md32_palette[index] = bswap(z);
	}

	t_dword *palette = m_array(t_dword, color_total);
	if (palette == NULL) return NULL;

	copy_md16_md32_palette(&md32_palette[0], &palette[0], color_total);
	convert32_md_rgba_palette(&palette[0], color_total);
	convert32_rgba_bgra_palette(&palette[0], color_total);

	free(md32_palette);
	md32_palette = NULL;

	return palette;
}

void unpack_md4_md32_pattern(t_byte *md4_pattern, t_dword *md32_pattern, int size)
{
	int x, y;

	for (x = 0; x < size; x++)
	{
		y = x * 2;

		md32_pattern[y + 0] = (md4_pattern[x] >> 4) & 0x0f;
		md32_pattern[y + 1] = (md4_pattern[x]) & 0x0f;
	}
}

t_dword *create_md4_md32_pattern(t_byte *md4_pattern, int size)
{
	t_dword *pattern = m_array(t_dword, size * MD_4BIT_PATTERN);
	if (pattern == NULL) return NULL;

	unpack_md4_md32_pattern((t_byte *)&md4_pattern[0], &pattern[0], size);

	return pattern;
}