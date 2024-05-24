#include "blue-dream.h"

void clear_ecco_sprite_cel(t_ecco_sprite_cel *cel)
{
	cel->palette_id = 0;
	cel->vflip = 0;
	cel->hflip = 0;

	cel->terminate = 0;
	cel->duplicate = 0;
	cel->pattern_id = 0;

	cel->row_total = 0;
	cel->column_total = 0;

	cel->relative_x = 0;
	cel->relative_y = 0;
}

t_ecco_sprite_cel *create_ecco_sprite_cel(void)
{
	t_ecco_sprite_cel *sp = (t_ecco_sprite_cel *)
		malloc(sizeof(t_ecco_sprite_cel));

	if (sp == NULL) return NULL;
	clear_ecco_sprite_cel(sp);

	return sp;
}

int ecco_sprite_cel_set_resource(
	t_ecco_sprite_cel *cel,
	t_byte *resource,
	int index)
{
	if (cel == NULL) return -1;
	if (resource == NULL) return -2;

	cel->palette_id = resource[index + 0] >> 5 & 0x3;
	cel->vflip = resource[index + 0] >> 4 & 0x1;
	cel->hflip = resource[index + 0] >> 3 & 0x1;

	cel->terminate = resource[index + 1] & 0x80;
	cel->duplicate = resource[index + 1] & 0x1;

	cel->pattern_id = resource[index + 2] << 4 & 0xfff0;
	cel->pattern_id |= resource[index + 3] >> 4 & 0xf;

	cel->row_total = resource[index + 3] & 0x3;
	cel->column_total = resource[index + 3] >> 2 & 0x3;

	cel->row_total++;
	cel->column_total++;

	cel->relative_x = resource[index + 4] & 0xff;
	cel->relative_y = resource[index + 5] & 0xff;

	if (cel->relative_x & 0x80)
		cel->relative_x = SIGNX(cel->relative_x);
	if (cel->relative_y & 0x80)
		cel->relative_y = SIGNX(cel->relative_y);

	return 0;
}

void plot_ecco_sprite_tile(
	t_plot_state *ps,
	t_dword *source,
	t_bitmap *destination,
	t_ecco_sprite_cel *cel,

	int column,
	int row,
	int pattern_index)
{
	int position_x = 0, position_y = 0;
	int flip = 0;

	plot_state_set_palette(
		ps, NULL, 16, 64, cel->palette_id);

	plot_state_set_source(ps, source, 8, 8);
	ps->source_size = MD_PATTERN_TABLE_SIZE;

	set_pattern_id(ps, cel->pattern_id + pattern_index);

	plot_state_set_destination(
		ps,
		destination->container,
		destination->w, destination->h
	);

	position_x = (column * 8) + cel->relative_x;
	position_y = (row * 8) + cel->relative_y;

	ps->input = ps_input_default;

	if (NOT_ZERO(cel->hflip))
	{
		position_x = (((cel->column_total - 1) - column) * 8)
			+ cel->relative_x;

		ps->input = ps_input_hflip;
	}

	if (NOT_ZERO(cel->vflip))
	{
		position_y = (((cel->row_total - 1) - row) * 8)
			+ cel->relative_y;

		ps->input = ps_input_vflip;
	}

	if (NOT_ZERO(cel->hflip) && NOT_ZERO(cel->vflip))
		ps->input = ps_input_vhflip;

	set_horizontal_clip(
		position_x,
		ps->source_w,
		ps->destination_w,
		ps->source_clip,
		ps->destination_clip
	);

	set_vertical_clip(
		position_y,
		ps->source_h,
		ps->destination_h,
		ps->source_clip,
		ps->destination_clip
	);

	ps->mode = PS_TRANSPARENTPIXEL_IGNORE;
	ps->output = ps_output_default;
	ps->plot = ps_plot_md_indexed;
}

int ecco_gsprite_cel_set_resource(
	t_ecco_sprite_cel *cel,
	t_byte *resource,
	int index)
{
	if (cel == NULL) return -1;
	if (resource == NULL) return -2;

	cel->palette_id = resource[index + 0] >> 5 & 0x3;
	cel->vflip = resource[index + 0] >> 4 & 0x1;
	cel->hflip = resource[index + 0] >> 3 & 0x1;

	cel->terminate = resource[index + 1] & 0x80;
	cel->duplicate = resource[index + 1] & 0x1;

	cel->pattern_id = resource[index + 2] << 4 & 0xfff0;
	cel->pattern_id |= resource[index + 3] >> 4 & 0xf;

	cel->row_total = resource[index + 3] & 0x3;
	cel->column_total = resource[index + 3] >> 2 & 0x3;

	cel->row_total++;
	cel->column_total++;

	cel->relative_x = resource[index + 4] & 0xff;
	cel->relative_y = resource[index + 5] & 0xff;

	if (cel->relative_x & 0x80)
		cel->relative_x = SIGNX(cel->relative_x);
	if (cel->relative_y & 0x80)
		cel->relative_y = SIGNX(cel->relative_y);

	return 0;
}

int get_gsprite_cel_total(t_byte *resource)
{
	int index = 0, result = 0, total = 0;

	for (index = 0; index < SPRITECEL_MAX; index++)
	{
		total++;

		result = resource[(index * ECCO_SPRITE_CEL_SIZE) + 1];
		if (AND_EQUAL(result, 0x80))
			break;
	}

	return total;
}

t_ecco_sprite_cel **ecco_gsprite_cel_create_all(
	t_byte *resource,
	int total,
	int *_width,
	int *_height)
{
	int index = 0, result = 0;
	int x = 0, y = 0;

	int relative_x = 0, relative_y = 0;
	int width = 0, height = 0;

	t_ecco_sprite_cel **cel = m_parray(t_ecco_sprite_cel, total);
	if (cel == NULL) return NULL;

	for (index = 0; index < total; index++)
	{
		cel[index] = create_ecco_sprite_cel();

		ecco_gsprite_cel_set_resource(
			cel[index],
			resource,
			index * ECCO_SPRITE_CEL_SIZE);

		if ((relative_x == 0) || (relative_x > cel[index]->relative_x))
			relative_x = cel[index]->relative_x;

		if ((relative_y == 0) || (relative_y > cel[index]->relative_y))
			relative_y = cel[index]->relative_y;
	}

	relative_x = -relative_x;
	relative_y = -relative_y;

	for (index = 0; index < total; index++)
	{
		cel[index]->relative_x+= relative_x;
		cel[index]->relative_y+= relative_y;

		x = (cel[index]->column_total * 8) +
			cel[index]->relative_x;

		y = (cel[index]->row_total * 8) +
			cel[index]->relative_y;

		if (x > width) width = x;
		if (y > height) height = y;
	}

	*_width = width;
	*_height = height;

	return cel;
}

t_bitmap *plot_ecco_global_sprite(
	t_plot_state *ps,
	t_dword *source,
	t_byte *resource)
{
	int index = 0;

	int column = 0, row = 0;
	int column_total = 0, row_total = 0;
	int pattern_index = 0;

	int relative_x = 0, relative_y = 0;
	int width = 0, height = 0;
	int total = 0;

	t_ecco_sprite_cel **cel = NULL;
	t_plot_state *next = NULL;
	t_bitmap *destination = NULL;

	total = get_gsprite_cel_total(resource);
	cel = ecco_gsprite_cel_create_all(
		resource,
		total,
		&width,
		&height);

	if (cel == NULL) return NULL;

	destination = create_bitmap(width, height);
	if (destination == NULL) return NULL;

	for (index = 0; index < total; index++)
	{
		pattern_index = 0;

		column_total = cel[index]->column_total;
		row_total = cel[index]->row_total;

		for (column = 0; column < column_total; column++)
			for (row = 0; row < row_total; row++)
			{
				next = plot_state_push_vacant(ps);

				plot_ecco_sprite_tile(
					next,
					source,
					destination,
					cel[index],

					column,
					row,
					pattern_index);

				pattern_index++;
			}
	}

	plot_state_plot_all(ps);
	return destination;
}