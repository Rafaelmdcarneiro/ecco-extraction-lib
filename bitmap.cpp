#include "blue-dream.h"

t_bitmap *create_bitmap(int w, int h)
{
	int index = 0;

	t_bitmap *bitmap = (t_bitmap *)malloc(
		sizeof(t_bitmap));
	if (bitmap == NULL) return NULL;

	bitmap->w = w;
	bitmap->h = h;
	bitmap->size = w * h;

	bitmap->container = (t_dword *)malloc(
		sizeof(t_dword) * bitmap->size);

	if (bitmap->container == NULL)
		return NULL;

	for (index = 0; index < bitmap->size; index++)
		bitmap->container[index] = 0;

	return bitmap;
}

void release_bitmap(t_bitmap *bitmap)
{
	if (bitmap == NULL) return;

	free(bitmap->container);
	free(bitmap);
}

void release_bitmap_array(t_bitmap **bitmap_array, int bitmap_total)
{
	int index = 0;

	for (index = 0; index < bitmap_total; index++)
		release_bitmap(bitmap_array[index]);
	free(bitmap_array);
}

// change to not use palette
void set_background_color(t_bitmap *bitmap, int color)
{
	int index = 0;
	for (index = 0; index < bitmap->size; index++)
		bitmap->container[index] = color;
}

void ps_set_source_bitmap(
	t_plot_state *ps,
	t_bitmap *source)
{
	plot_state_set_source(
		ps,
		source->container,
		source->w,
		source->h);
}

void ps_set_destination_bitmap(
	t_plot_state *ps,
	t_bitmap *destination)
{
	plot_state_set_destination(
		ps,
		destination->container,
		destination->w,
		destination->h);
}

/* begin added from plot.cpp */

int plot_bitmap_indexed(
	t_plot_state *ps,
	t_dword *palette,
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int palette_index,
	int mode)
{
	t_plot_state *next = NULL;

	if ((x + source->w) < 0) return -1;
	if ((y + source->h) < 0) return -1;
	if (x >= destination->w) return -1;
	if (y >= destination->h) return -1;

	next = plot_state_add_vacant(ps);
	if (next == NULL) return -1;

	next->mode = mode;

	plot_state_set_palette(next, palette, 0, 0, 0);
	next->palette_index = palette_index;

	set_pattern_id(next, 0);

	plot_state_set_source(
		next,
		source->container,
		source->w,
		source->h);

	plot_state_set_destination(
		next,
		destination->container,
		destination->w,
		destination->h);

	update_clipping_regions(next, x, y);
	plot_state_set_flip(next, mode);

	next->output = ps_output_default;
	next->plot = ps_plot_default;

	return 0;
}

int plot_bitmap_direct(
	t_plot_state *ps,
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int mode)
{
	t_plot_state *next = NULL;

	if ((x + source->w) < 0) return -1;
	if ((y + source->h) < 0) return -1;
	if (x >= destination->w) return -1;
	if (y >= destination->h) return -1;

	next = plot_state_add_vacant(ps);
	if (next == NULL) return -1;

	next->mode = mode;

	plot_state_set_palette(next, NULL, 0, 0, 0);
	set_pattern_id(next, 0);

	// change to ps_bitmap_set_source
	plot_state_set_source(
		next,
		source->container,
		source->w,
		source->h);

	plot_state_set_destination(
		next,
		destination->container,
		destination->w,
		destination->h);

	update_clipping_regions(next, x, y);
	plot_state_set_flip(next, mode);

	next->output = ps_output_default;
	next->plot = ps_plot_direct;

	return 0;
}

int _plot_bitmap_direct(
	t_plot_state *ps,
	t_dword *source,
	t_bitmap *destination,
	int x, int y,
	int mode)
{
	t_plot_state *next = NULL;
	int result = 0;

	// if (x >= destination->w) return -1;
	// if (y >= destination->h) return -1;

	next = plot_state_add_vacant(ps);
	if (next == NULL) return -1;

	next->mode = mode;

	plot_state_set_palette(ps, NULL, 0, 0, 0);
	set_pattern_id(next, 0);

	plot_state_set_source(
		next,
		source,
		0,
		0);

	plot_state_set_destination(
		next,
		destination->container,
		destination->w,
		destination->h);

	next->output = ps_output_default;
	next->plot = ps_plot_update_palette;

	return 0;
}

/* end added from plot.cpp */

t_bitmap *create_tile(
	t_dword *pattern,
	int w,
	int h,
	int size,
	int pattern_id)
{
	t_plot_state *ps = NULL;
	t_bitmap *destination = NULL;

	if ((pattern_id * w * h) >= size)
		return NULL;

	destination = create_bitmap(w, h);
	if (destination == NULL) return NULL;

	ps = create_plot_state();
	if (ps == NULL) return NULL;
	plot_state_clear_all(ps);

	ps = plot_state_add_vacant(ps);
	plot_state_set_palette(ps, NULL, 0, 0, 0);

	plot_state_set_source(ps, pattern, w, h);
	ps_set_destination_bitmap(ps, destination);
	update_clipping_regions(ps, 0, 0);

	ps->source_size = size;
	set_pattern_id(ps, pattern_id);

	ps->input = ps_input_default;
	ps->output = ps_output_default;
	ps->plot = ps_plot_direct;

	plot_state_plot_all(ps);
	release_plot_state(ps);

	return destination;
}

t_bitmap **create_tileset(
	t_dword *pattern, 
	int w, int h,
	int size,
	int *_total)
{
	t_bitmap **table = NULL;
	int index = 0, total = 0;

	total = size / (w * h);

	table = m_parray(t_bitmap, total);
	if (table == NULL) return NULL;

	for (index = 0; index < total; index++)
		table[index] = NULL;

	for (index = 0; index < total; index++)
	{
		table[index] = create_tile(
			pattern, w, h,
			size, index);

		if (table[index] == NULL)
			return NULL;
	}

	if (NOT_NULL(_total))
		*_total = total;

	return table;
}

int draw_bitmap_indexed(
	t_dword *palette,
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int palette_index,
	int mode)
{
	t_plot_state *ps = NULL;

	if ((x + source->w) < 0) return -1;
	if ((y + source->h) < 0) return -1;
	if (x >= destination->w) return -1;
	if (y >= destination->h) return -1;

	ps = create_plot_state();
	if (ps == NULL) return -1;
	plot_state_clear_all(ps);

	ps = plot_state_add_vacant(ps);
	if (ps == NULL) return -1;

	ps->mode = mode;

	plot_state_set_palette(ps, palette, 0, 0, 0);
	ps->palette_index = palette_index;

	set_pattern_id(ps, 0);

	plot_state_set_source(
		ps,
		source->container,
		source->w,
		source->h);

	plot_state_set_destination(
		ps,
		destination->container,
		destination->w,
		destination->h);

	update_clipping_regions(ps, x, y);
	plot_state_set_flip(ps, mode);

	ps->output = ps_output_default;
	ps->plot = ps_plot_default;

	plot_state_plot_all(ps);
	release_plot_state(ps);

	return 0;
}

int draw_bitmap_direct(
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int mode)
{
	t_plot_state *ps = NULL;

	if ((x + source->w) < 0) return -1;
	if ((y + source->h) < 0) return -1;
	if (x >= destination->w) return -1;
	if (y >= destination->h) return -1;

	ps = create_plot_state();
	if (ps == NULL) return NULL;
	plot_state_clear_all(ps);

	ps = plot_state_add_vacant(ps);
	if (ps == NULL) return -1;

	ps->mode = mode;

	plot_state_set_palette(ps, NULL, 0, 0, 0);
	set_pattern_id(ps, 0);

	plot_state_set_source(
		ps,
		source->container,
		source->w,
		source->h);

	plot_state_set_destination(
		ps,
		destination->container,
		destination->w,
		destination->h);

	update_clipping_regions(ps, x, y);
	plot_state_set_flip(ps, mode);

	ps->output = ps_output_default;
	ps->plot = ps_plot_direct;

	plot_state_plot_all(ps);
	release_plot_state(ps);

	return 0;
}
