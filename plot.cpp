#include "blue-dream.h"

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

void set_horizontal_clip(
	int x,
	int source_w,
	int destination_w,
	t_region *u,
	t_region *v)
{
	if (x < 0)
	{
		u->x = -x;

		if (source_w < destination_w)
			u->w = source_w - u->x;
		else
			u->w = destination_w;

		v->x = 0;
		v->w = u->w;
	}

	else if ((x + source_w) >= destination_w)
	{
		u->x = 0;
		u->w = destination_w - x;

		v->x = x;
		v->w = u->w;
	}

	else
	{
		u->x = 0;

		if (source_w < destination_w)
			u->w = source_w;
		else
			u->w = destination_w - u->x;

		v->x = x;
		v->w = u->w;
	}
}

void set_vertical_clip(
	int y,
	int source_h,
	int destination_h,
	t_region *u,
	t_region *v)
{

	if (y < 0)
	{
		u->y = -y;

		if (source_h < destination_h)
			u->h = source_h - u->y;
		else
			u->h = destination_h;

		v->y = 0;
		v->h = u->h;
	}

	else if ((y + source_h) >= destination_h)
	{
		u->y = 0;
		u->h = destination_h - y;

		v->y = y;
		v->h = u->h;
	}

	else
	{
		u->y = 0;

		if (source_h < destination_h)
			u->h = source_h;
		else
			u->h = destination_h - u->y;

		v->y = y;
		v->h = u->h;
	}
}

void update_clipping_regions(t_plot_state *ps, int x, int y)
{
	set_horizontal_clip(
		x,
		ps->source_w,
		ps->destination_w,
		ps->source_clip,
		ps->destination_clip
	);

	set_vertical_clip(
		y,
		ps->source_h,
		ps->destination_h,
		ps->source_clip,
		ps->destination_clip
	);
}

t_plot_process *create_plot_process(void)
{
	t_plot_process *pp = (t_plot_process *)malloc(
		sizeof(t_plot_process));
	if (pp == NULL) return NULL;

	pp->palette = NULL;
	pp->source = NULL;
	pp->destination = NULL;

	return pp;
}

void plot_process_set_source(
	t_plot_process *pp,
	t_dword *source,
	int w,
	int h)
{
	pp->source = source;
	pp->source_w = w;
	pp->source_h = h;
	pp->source_size = w * h;
}

void plot_process_set_destination(
	t_plot_process *pp,
	t_dword *destination,
	int w,
	int h)
{
	pp->destination = destination;
	pp->destination_w = w;
	pp->destination_h = h;
	pp->destination_size = w * h;
}

void clear_plot_state(t_plot_state *ps)
{
	if (ps == NULL) return;

	ps->class_id = 0;
	ps->status_id = PS_STATUS_VACANT;

	ps->mode = 0;

	ps->parameters = NULL;
	ps->palette = NULL;
	ps->source = NULL;
	ps->destination = NULL;

	ps->parameters_size = 0;
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

	ps->input = NULL;
	ps->output = NULL;
	ps->plot = NULL;
	ps->next = NULL;
}

void release_plot_state(t_plot_state *ps)
{
	if (ps == NULL) return;

	t_plot_state *current = ps;
	t_plot_state *next = NULL;

	clear_plot_state(ps);

	if (NOT_NULL(ps->source_clip))
	{
		free(ps->source_clip);
		ps->source_clip = NULL;
	}

	if (NOT_NULL(ps->destination_clip))
	{
		free(ps->destination_clip);
		ps->destination_clip = NULL;
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

t_plot_state *create_plot_state(void)
{
	t_plot_state *ps = (t_plot_state *)malloc(sizeof(t_plot_state));
	if (ps == NULL) return NULL;

	ps->_next = NULL;

	ps->source_clip = create_region(0, 0, 0, 0);
	if (ps->source_clip == NULL)
	{
		clear_plot_state(ps);
		return NULL;
	}

	ps->destination_clip = create_region(0, 0, 0, 0);
	if (ps->destination_clip == NULL)
	{
		clear_plot_state(ps);
		return NULL;
	}

	clear_plot_state(ps);

	return ps;
}

void clone_plot_state(t_plot_state *x, t_plot_state *y)
{
	if (x == NULL) return;
	if (y == NULL) return;

	x->class_id = y->class_id;
	x->status_id = y->status_id;
	x->mode = y->mode;

	x->parameters = y->parameters;
	x->palette = y->palette;
	x->source = y->source;
	x->destination = y->destination;

	x->parameters_size = y->parameters_size;
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

	x->input = y->input;
	x->output = y->output;

	x->plot = y->plot;
	x->next = y->next;
}

void plot_state_set_palette(
	t_plot_state *ps,
	t_dword *palette,
	int w,
	int total,
	int id)
{
	ps->palette = palette;
	ps->palette_w = w;
	ps->palette_total = total;

	set_palette_id(ps, id);
}

void plot_state_set_source(
	t_plot_state *ps,
	t_dword *source,
	int w,
	int h)
{
	ps->source = source;
	ps->source_w = w;
	ps->source_h = h;
	ps->source_size = w * h;
}

void plot_state_set_destination(
	t_plot_state *ps,
	t_dword *destination,
	int w,
	int h)
{
	ps->destination = destination;
	ps->destination_w = w;
	ps->destination_h = h;
	ps->destination_size = w * h;
}

void plot_state_set_source_region(
	t_plot_state *ps,
	int x, int y, int w, int h,
	int pattern_id)
{
	set_region(ps->source_clip, x, y, w, h);
	set_pattern_id(ps, pattern_id);
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
		else break;
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
	ps->palette_index = id * ps->palette_w;
}

// may want to rename
int get_palette_color_index(t_plot_state *ps, int color_id)
{
	return ps->palette_index + color_id;
}

void set_pattern_id(t_plot_state *ps, int id)
{
	ps->source_id = id;
	ps->source_index = ps->source_id * ps->source_w * ps->source_h;
}

int ps_input_default(t_plot_state *ps, int x, int y)
{
	return (ps->source_index + x + (y * ps->source_w));
}

int ps_input_hflip(t_plot_state *ps, int x, int y)
{
	return (ps->source_index + ((ps->source_w - 1) - x) + (y * ps->source_w));
}

int ps_input_vflip(t_plot_state *ps, int x, int y)
{
	return (ps->source_index + x + (ps->source_w * (ps->source_h - 1)) - (y * ps->source_w));
}

int ps_input_vhflip(t_plot_state *ps, int x, int y)
{
	return (ps->source_index + ((ps->source_w - 1) - x) + (ps->source_w * (ps->source_h - 1)) - (y * ps->source_w));
}

int ps_output_default(t_plot_state *ps, int x, int y)
{
	return x + (y * ps->destination_w);
}

int ps_priority_shadow(int color)
{
	int b = 0, g = 0, r = 0, a = 0;

	b = (color & 0x000000ff) >> 1;
	g = (color & 0x0000ff00) >> 1;
	r = (color & 0x00ff0000) >> 1;
	a = (color & 0xff000000);

	b = b & 0x000000ff;
	g = g & 0x0000ff00;
	r = r & 0x00ff0000;

	return b | g | r | a;
}

int ps_plot_md_indexed_pixel(t_plot_state *ps, int x, int y)
{
	int u = 0, v = 0;
	int channel = 0, mode = 0;

	u = ps->input(ps, ps->source_clip->x + x, ps->source_clip->y + y);
	v = ps->output(ps, ps->destination_clip->x + x, ps->destination_clip->y + y);

	if (u > ps->source_size) return 0;

	channel = ps->source[u];

	mode = (ps->mode & PS_MASK_PRIORITY);
	if (channel == 0) mode |= PS_TRANSPARENTPIXEL;

	channel = get_palette_color_index(ps, channel);
	channel |= mode;

	ps->destination[v] = channel;
	return 0;
}

int ps_plot_md_indexed(t_plot_state *ps)
{
	int x = 0, y = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
			ps_plot_md_indexed_pixel(ps, x, y);
	}

	return 0;
}

int ps_transparentpixel_clear(t_plot_state *ps, int pixel_mode, int pixel_index)
{
	if (AND_EQUAL(ps->mode, PS_PRIORITY))
	{
		if (AND_EQUAL(pixel_mode, PS_SHADOW))
		{
			ps->destination[pixel_index] = PS_TRANSPARENTPIXEL_SHADOW;
			return 0;
		}
	}

	ps->destination[pixel_index] = 0;
	return 0;
}

int ps_transparentpixel_fill(t_plot_state *ps, int pixel_mode, int pixel_index)
{
	if (AND_EQUAL(ps->mode, PS_PRIORITY))
	{
		if (AND_EQUAL(pixel_mode, PS_SHADOW))
		{
			ps->destination[pixel_index] = ps_priority_shadow(ps->palette[ps->palette_index]);
			return 0;
		}
	}

	ps->destination[pixel_index] = ps->palette[ps->palette_index];
	return 0;
}

int ps_plot_default_pixel(t_plot_state *ps, int x, int y)
{
	int u = 0, v = 0;
	int pixel_mode = 0, color_index = 0, color = 0;

	u = ps->input(ps, ps->source_clip->x + x, ps->source_clip->y + y);
	v = ps->output(ps, ps->destination_clip->x + x, ps->destination_clip->y + y);

	color_index = ps->source[u];
	pixel_mode = color_index & PS_MASK_MODE;
	color_index &= PS_MASK_COLORINDEX;
	
	if (AND_EQUAL(pixel_mode, PS_TRANSPARENTPIXEL))
	{
		if (AND_EQUAL(ps->mode, PS_TRANSPARENTPIXEL_CLEAR))
			return ps_transparentpixel_clear(ps, pixel_mode, v);

		else if (AND_EQUAL(ps->mode, PS_TRANSPARENTPIXEL_FILL))
			return ps_transparentpixel_fill(ps, pixel_mode, v);
	}
	
	color_index += ps->palette_index;
	color = ps->palette[color_index];

	if (AND_EQUAL(ps->mode, PS_PRIORITY))
		if (AND_EQUAL(pixel_mode, PS_SHADOW))
			color = ps_priority_shadow(color);

	ps->destination[v] = color;
	return 0;
}

int ps_plot_default(t_plot_state *ps)
{
	int x = 0, y = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
			ps_plot_default_pixel(ps, x, y);
	}

	return 0;
}

int ps_plot_zlm(t_plot_state *ps)
{
	int x = 0, y = 0;
	int u = 0, v = 0;

	int index = 0;
	int offset_x = 0;
	int pixel_total = 0;
	int channel = 0;

	for (y = 0; y < ps->source_clip->h; y++)
	{
		offset_x = ps->source[index + 0];
		offset_x |= (ps->source[index + 1] << 8);

		pixel_total = ps->source[index + 2];
		pixel_total |= (ps->source[index + 3] << 8);

		pixel_total -= 2;

		for (x = 0; x < pixel_total; x++)
		{
			u = (index + 4) + x;
			v = offset_x + x + (y * ps->destination_w);

			channel = get_palette_color_index(ps, ps->source[u]);
			
			ps->destination[v] = ps->palette[channel] | 0xff000000;
		}

		index+= pixel_total + 2;
	}

	return 0;
}

// fix! needs to add stop pixel.
int ps_plot_mask(t_plot_state *ps)
{
	int w = 0, x = 0, y = 0, z = 0;
	int index = 0;

	int data = 0;
	int pixel = 0;

	for (y = 0; y < ps->source_clip->h; y++)
	{
		for (x = 0; x < ps->source_w; x++)
		{
			data = ps->source[(ps->source_w * y) + x];

			for (z = 0; z < 8; z++)
			{
				if (z < 7)
					pixel = (data >> (7 - z)) & 0x00000001;
				else
					pixel = data & 0x00000001;

				w = (x * 8) + z;
				if (pixel == 0 && w < ps->destination_w)
				   ps->destination[(ps->destination_w * y) + w] = 0x00000000;
			}
		}
	}

	return 0;
}

int ps_plot_direct_pixel(t_plot_state *ps, int x, int y)
{
	int u = 0, v = 0;

	u = ps->input(ps, ps->source_clip->x + x, ps->source_clip->y + y);
	v = ps->output(ps, ps->destination_clip->x + x, ps->destination_clip->y + y);

	if (u >= ps->source_size) return 0;
	if (v >= ps->destination_size) return 0;

	ps->destination[v] = ps->source[u];

	return 0;
}

int ps_plot_direct(t_plot_state *ps)
{
	int x = 0, y = 0;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		for (x = 0; x < ps->destination_clip->w; x++)
			ps_plot_direct_pixel(ps, x, y);
	}

	return 0;
}

int ps_plot_update_palette(t_plot_state *ps)
{
	int x = 0, y = 0;
	int u = 0, v = 0;

	int palette_index = 0;
	int palette_column = 0;

	int mode = 0;
	int pixel_index = 0;

	for (y = 0; y < ps->destination_h; y++)
	{
		u = ps->source[(palette_index * 2)];
		palette_column = ps->source[(palette_index * 2) + 1];
		
		if (y >= ((palette_column + 1) * 0x80)) // set to 2 in case
			if (palette_column != 0)
				palette_index++;

		for (x = 0; x < ps->destination_w; x++)
		{
			v = ps->output(ps, x, y);
			
			// disable for backgrounds.
			// if (ps->destination[v] == 0) continue;

			ps->palette_index = u * 64;

			mode = ps->destination[v] & PS_MASK_MODE;

			pixel_index = ps->destination[v] & PS_MASK_COLORINDEX;
			pixel_index += ps->palette_index;

			ps->destination[v] = mode | pixel_index;
		}
	}

	return 0;
}

void plot_state_set_flip(t_plot_state *ps, int flags)
{
	switch (flags & PS_MASK_FLIP)
	{
		case PS_VHFLIP:
		{
			ps->input = ps_input_vhflip;
			break;
		}

		case PS_VFLIP:
		{
			ps->input = ps_input_vflip;
			break;
		}

		case PS_HFLIP:
		{
			ps->input = ps_input_hflip;
			break;
		}

		default:
		{
			ps->input = ps_input_default;
			break;
		}
	}
}

int plot_state_plot_all(t_plot_state *ps)
{
	t_plot_state *current = ps;

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->plot))
			current->plot(current);

		current = current->next;
	}

	plot_state_clear_all(ps);

	return 0;
}