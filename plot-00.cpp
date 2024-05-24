#include "blue-dream.h"

t_byte convert_md_rgba_channel(t_byte x)
{
	return x << 4 | x << 1 | x >> 2;
}

t_byte convert2_md_rgba_channel(double x)
{
	x = (255.0f / 7.0f) * (x / 2);
	return (t_byte)round(x);
}

// Big Endian Conversion (Mega Drive).

void copy_md16bit_md32bit_palette(t_byte *palette, t_byte *md_palette, int total)
{
	int index; for (index = 0; index < total; index++)
	{
		palette[(index * RGBA_32BIT_COLOR) + 0] = (md_palette[(index * MD_16BIT_COLOR) + 1]) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 1] = (md_palette[(index * MD_16BIT_COLOR) + 1] >> 4) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 2] = (md_palette[(index * MD_16BIT_COLOR) + 0]) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 3] = 0xff;
	}
}

// Little Endian Conversion (Intel PC).

void copy2_md16bit_md32bit_palette(t_byte *palette, t_byte *md_palette, int total)
{
	int index; for (index = 0; index < total; index++)
	{
		palette[(index * RGBA_32BIT_COLOR) + 0] = (md_palette[(index * MD_16BIT_COLOR) + 0]) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 1] = (md_palette[(index * MD_16BIT_COLOR) + 0] >> 4) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 2] = (md_palette[(index * MD_16BIT_COLOR) + 1]) & 0x0f;
		palette[(index * RGBA_32BIT_COLOR) + 3] = 0xff;
	}
}

void convert_md32bit_rgba32bit_palette(t_byte *palette, int total)
{
	int index; for (index = 0; index < total; index++)
	{
		palette[(index * RGBA_32BIT_COLOR) + 0] = convert_md_rgba_channel(palette[(index * RGBA_32BIT_COLOR) + 0]);
		palette[(index * RGBA_32BIT_COLOR) + 1] = convert_md_rgba_channel(palette[(index * RGBA_32BIT_COLOR) + 1]);
		palette[(index * RGBA_32BIT_COLOR) + 2] = convert_md_rgba_channel(palette[(index * RGBA_32BIT_COLOR) + 2]);
	}
}

void convert_rgba32bit_bgra32bit_palette(t_byte *palette, int total)
{
	t_byte r, b;

	int index; for (index = 0; index < total; index++)
	{
		r = palette[(index * RGBA_32BIT_COLOR) + 0];
		b = palette[(index * RGBA_32BIT_COLOR) + 2];

		palette[(index * RGBA_32BIT_COLOR) + 0] = b;
		palette[(index * RGBA_32BIT_COLOR) + 2] = r;
	}
}

// Little Endian Conversion (Intel PC).

void copy_md4bpp_16bpp_pattern(t_word *pattern, t_byte *md_pattern, int total)
{
	int index; for (index = 0; index < total; index++)
	{
		pattern[(index * 2) + 0] = (md_pattern[index] >> 4) & 0x0f;
		pattern[(index * 2) + 1] = (md_pattern[index]) & 0x0f;
	}
}

t_region *create_region(int x, int y, int w, int h)
{
	t_region *region = (t_region *)malloc(sizeof(t_region));
	if (region == NULL) return NULL;

	region->x = x;
	region->y = y;
	region->w = w;
	region->h = h;

	return region;
}

void set_region(t_region *region, int x, int y, int w, int h)
{
	if (region == NULL) return;

	region->x = x;
	region->y = y;
	region->w = w;
	region->h = h;
}

void clone_region(t_region *x, t_region *y)
{
	if (x == NULL) return;
	if (y == NULL) return;

	x->x = y->x;
	x->y = y->y;
	x->w = y->w;
	x->h = y->h;
}

t_plot_state *create_plot_state(void)
{
	t_plot_state *ps = (t_plot_state *)malloc(sizeof(t_plot_state));
	if (ps == NULL) return NULL;

	ps->palette = NULL;
	ps->source = NULL;
	ps->destination = NULL;

	ps->id = 0;
	ps->class_id = 0;
	ps->status_id = PS_STATUS_VACANT;

	ps->transparent_id = 0;
	ps->priority_id = 0;
	ps->vertical_flip = 0;
	ps->horizontal_flip = 0;

	ps->palette_id = 0;
	ps->palette_index = 0;
	ps->palette_w = 0;
	ps->palette_total = 0;

	ps->source_id = 0;
	ps->source_index = 0;
	ps->source_w = 0;
	ps->source_h = 0;
	ps->source_size = 0;

	ps->destination_w = 0;
	ps->destination_h = 0;
	ps->destination_size = 0;

	ps->source_clip = create_region(0, 0, 0, 0);
	if (ps->source_clip == NULL)
	{
		free(ps);
		ps = NULL;

		return NULL;
	}

	ps->destination_clip = create_region(0, 0, 0, 0);
	if (ps->destination_clip == NULL)
	{
		free(ps->source_clip);
		ps->source_clip = NULL;

		free(ps);
		ps = NULL;

		return NULL;
	}

	ps->resource = NULL;
	ps->plot = NULL;

	ps->next = NULL;
	ps->_next = NULL;

	return ps;
}

void release_plot_state(t_plot_state *plot_state)
{
	if (plot_state == NULL) return;

	t_plot_state *current = plot_state;
	t_plot_state *next = NULL;

	plot_state->palette = NULL;
	plot_state->source = NULL;
	plot_state->destination = NULL;
	plot_state->plot = NULL;

	if (NOT_NULL(plot_state->source_clip))
	{
		free(plot_state->source_clip);
		plot_state->source_clip = NULL;
	}

	if (NOT_NULL(plot_state->destination_clip))
	{
		free(plot_state->destination_clip);
		plot_state->destination_clip = NULL;
	}

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->_next))
			next = current->_next;
		else
			next = NULL;

		free(current);
		current = next;
	}
}

void clear_plot_state(t_plot_state *ps)
{
	if (ps == NULL) return;

	ps->palette = NULL;
	ps->source = NULL;
	ps->destination = NULL;

	ps->class_id = 0;
	ps->status_id = PS_STATUS_VACANT;

	ps->transparent_id = 0;
	ps->priority_id = 0;
	ps->vertical_flip = 0;
	ps->horizontal_flip = 0;

	ps->palette_id = 0;
	ps->palette_index = 0;
	ps->palette_w = 0;
	ps->palette_total = 0;

	ps->source_id = 0;
	ps->source_index = 0;
	ps->source_w = 0;
	ps->source_h = 0;
	ps->source_size = 0;

	ps->destination_w = 0;
	ps->destination_h = 0;
	ps->destination_size = 0;

	set_region(ps->source_clip, 0, 0, 0, 0);
	set_region(ps->destination_clip, 0, 0, 0, 0);

	ps->resource = NULL;
	ps->plot = NULL;

	ps->next = NULL;
}

void clone_plot_state(t_plot_state *x, t_plot_state *y)
{
	if (x == NULL) return;
	if (y == NULL) return;

	x->class_id = y->class_id;

	x->transparent_id = y->transparent_id;
	x->priority_id = y->priority_id;

	x->vertical_flip = y->vertical_flip;
	x->horizontal_flip = y->horizontal_flip;

	x->palette = y->palette;
	x->source = y->source;
	x->destination = y->destination;

	x->palette_id = y->palette_id;
	x->palette_index = y->palette_index;
	x->palette_w = y->palette_w;
	x->palette_total = y->palette_total;

	x->source_id = y->source_id;
	x->source_index = y->source_index;
	x->source_w = y->source_w;
	x->source_h = y->source_h;
	x->source_size = y->source_size;

	x->destination_w = y->destination_w;
	x->destination_h = y->destination_h;
	x->destination_size = y->destination_size;

	clone_region(x->source_clip, y->source_clip);
	clone_region(x->destination_clip, y->destination_clip);

	x->plot = y->plot;
}

t_plot_state *plot_state_add(t_plot_state *plot_state)
{
	if (plot_state == NULL) return NULL;

	t_plot_state *next = create_plot_state();
	if (next == NULL) return NULL;

	t_plot_state *current = plot_state;

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->_next))
			current = current->_next;
		else
		{
			next->id = current->id + 1;

			current->_next = next;
			current = NULL;
		}
	}

	return next;
}

t_plot_state *plot_state_get_vacant(t_plot_state *plot_state)
{
	if (plot_state == NULL) return NULL;

	t_plot_state *current = plot_state;
	if (current->status_id == PS_STATUS_VACANT)
		return current;

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->_next))
		{
			current = current->_next;
			if (current->status_id == PS_STATUS_VACANT)
				return current;
		}
	}

	return NULL;
}

t_plot_state *plot_state_add_vacant(t_plot_state *plot_state)
{
	if (plot_state == NULL) return NULL;

	t_plot_state *next = plot_state_get_vacant(plot_state);
	if (next == NULL) return NULL;

	t_plot_state *current = plot_state;
	if (current->id == next->id)
	{
		current->status_id = PS_STATUS_OCCUPIED;
		return current;
	}

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->next))
			current = current->next;
		else
		{
			next->status_id = PS_STATUS_OCCUPIED;

			current->next = next;
			current = NULL;
		}
	}

	return next;
}

t_plot_state *plot_state_push_vacant(t_plot_state *plot_state)
{
	if (plot_state == NULL) return NULL;

	t_plot_state *next = plot_state_get_vacant(plot_state);
	if (next == NULL) return NULL;

	t_plot_state *current = plot_state;
	if (current->id == next->id)
	{
		current->status_id = PS_STATUS_OCCUPIED;
		return current;
	}

	clone_plot_state(next, current);
	clear_plot_state(current);

	current->status_id = PS_STATUS_OCCUPIED;
	current->next = next;

	return current;
}

void plot_state_pop_vacant(t_plot_state *plot_state)
{
	if (plot_state == NULL) return;

	t_plot_state *current = plot_state;
	t_plot_state *next =  current->next;

	clone_plot_state(current, next);
	clear_plot_state(next);
}

void plot_state_clear_all(t_plot_state *plot_state)
{
	if (plot_state == NULL) return;

	t_plot_state *current = plot_state;
	t_plot_state *next = NULL;

	while (NOT_NULL(current))
	{
		next = current->next;

		clear_plot_state(current);
		current->status_id = PS_STATUS_VACANT;

		current = next;
	};
}

void set_palette_id(t_plot_state *ps, int id)
{
	ps->palette_id = id;
	ps->palette_index = id * ps->palette_w * RGBA_32BIT_COLOR;
}

int get_palette_color_index(t_plot_state *ps, int color_id)
{
	return ps->palette_index + (color_id * RGBA_32BIT_COLOR);
}

void set_pattern_id(t_plot_state *ps, int id)
{
	ps->source_id = id;
	ps->source_index = ps->source_id * ps->source_w * ps->source_h;
}

int ps_output_hflip(t_plot_state *ps, int x, int y)
{
	int index = ps->source_index + ((ps->source_w - 1) - x) + (y * ps->source_w);
	return index;
}

int ps_output_vflip(t_plot_state *ps, int x, int y)
{
	int index = ps->source_index + x + (ps->source_w * (ps->source_h - 1)) - (y * ps->source_w);
	return index;
}

int ps_output_vhflip(t_plot_state *ps, int x, int y)
{
	int index = ps->source_index + ((ps->source_w - 1) - x) + (ps->source_w * (ps->source_h - 1)) - (y * ps->source_w);
	return index;
}

int ps_plot_default(void *plot_state)
{
	t_plot_state *ps = (t_plot_state *)plot_state;

	int u = ps->source_index + ps->source_clip->x + (ps->source_clip->y * ps->source_w);
	int v = ps->destination_clip->x + (ps->destination_clip->y * ps->destination_w);

	int w = 0, x = 0, y = 0;
	int channel = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
		{
			w = v * RGBA_32BIT_COLOR;

			if (u > ps->source_size) return -1;
			if (w > ps->destination_size) return -2;

			if (NOT_EQUAL(ps->source[u], ps->transparent_id))
			{
				channel = get_palette_color_index(ps, ps->source[u]);

				ps->destination[w + 0] = ps->palette[channel + 0];
				ps->destination[w + 1] = ps->palette[channel + 1];
				ps->destination[w + 2] = ps->palette[channel + 2];
				ps->destination[w + 3] = ps->palette[channel + 3];
			}

			u++;
			v++;
		}

		v += (ps->destination_w - ps->destination_clip->w);
	}

	return 0;
}

int ps_plot_hflip(void *plot_state)
{
	t_plot_state *ps = (t_plot_state *)plot_state;

	int u;
	int v = ps->destination_clip->x + (ps->destination_clip->y * ps->destination_w);

	int w = 0, x = 0, y = 0;
	int channel = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
		{
			w = v * RGBA_32BIT_COLOR;

			u = ps_output_hflip(ps, x, y);

			if (NOT_EQUAL(ps->source[u], ps->transparent_id))
			{
				channel = get_palette_color_index(ps, ps->source[u]);

				ps->destination[w + 0] = ps->palette[channel + 0];
				ps->destination[w + 1] = ps->palette[channel + 1];
				ps->destination[w + 2] = ps->palette[channel + 2];
				ps->destination[w + 3] = ps->palette[channel + 3];
			}

			u++;
			v++;
		}

		v += (ps->destination_w - ps->destination_clip->w);
	}

	return 0;
}

int ps_plot_vflip(void *plot_state)
{
	t_plot_state *ps = (t_plot_state *)plot_state;

	int u;
	int v = ps->destination_clip->x + (ps->destination_clip->y * ps->destination_w);

	int w = 0, x = 0, y = 0;
	int channel = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
		{
			w = v * RGBA_32BIT_COLOR;

			u = ps_output_vflip(ps, x, y);

			if (NOT_EQUAL(ps->source[u], ps->transparent_id))
			{
				channel = get_palette_color_index(ps, ps->source[u]);

				ps->destination[w + 0] = ps->palette[channel + 0];
				ps->destination[w + 1] = ps->palette[channel + 1];
				ps->destination[w + 2] = ps->palette[channel + 2];
				ps->destination[w + 3] = ps->palette[channel + 3];
			}

			u++;
			v++;
		}

		v += (ps->destination_w - ps->destination_clip->w);
	}

	return 0;
}

int ps_plot_vhflip(void *plot_state)
{
	t_plot_state *ps = (t_plot_state *)plot_state;

	int u;
	int v = ps->destination_clip->x + (ps->destination_clip->y * ps->destination_w);

	int w = 0, x = 0, y = 0;
	int channel = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
		{
			w = v * RGBA_32BIT_COLOR;

			u = ps_output_vhflip(ps, x, y);

			if (NOT_EQUAL(ps->source[u], ps->transparent_id))
			{
				channel = get_palette_color_index(ps, ps->source[u]);

				ps->destination[w + 0] = ps->palette[channel + 0];
				ps->destination[w + 1] = ps->palette[channel + 1];
				ps->destination[w + 2] = ps->palette[channel + 2];
				ps->destination[w + 3] = ps->palette[channel + 3];
			}

			u++;
			v++;
		}

		v += (ps->destination_w - ps->destination_clip->w);
	}

	return 0;
}