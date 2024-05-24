#include "blue-dream.h"

void clear_etd_sprite_cel(t_etd_sprite_cel *cel)
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

t_etd_sprite_cel *create_etd_sprite_cel(void)
{
	t_etd_sprite_cel *sp = (t_etd_sprite_cel *)
		malloc(sizeof(t_etd_sprite_cel));

	if (sp == NULL) return NULL;
	clear_etd_sprite_cel(sp);

	return sp;
}

int etd_sprite_cel_set_resource(
	t_etd_sprite_cel *cel,
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

void plot_state_set_etd_sprite(
	t_plot_process *pp,
	t_plot_state *ps,
	t_etd_sprite_cel *cel,

	int column,
	int row,
	int relative_x,
	int relative_y,
	int pattern_index)
{
	int position_x = 0, position_y = 0;
	int flip = 0;

	plot_state_set_palette(
		ps,
		pp->palette,
		16, 64,
		cel->palette_id);

	plot_state_set_source(
		ps,
		pp->source,
		8, 8);

	ps->source_size = MD_PATTERN_TABLE_SIZE;

	set_region(ps->source_clip, 0, 0, 8, 8);
	set_pattern_id(ps, cel->pattern_id + pattern_index);

	plot_state_set_destination(
		ps,
		pp->destination,
		pp->destination_w, pp->destination_h
	);

	position_x = (column * 8) + cel->relative_x + relative_x;
	position_y = (row * 8) + cel->relative_y + relative_y;

	ps->input = ps_input_default;

	if (NOT_ZERO(cel->hflip))
	{
		position_x = (((cel->column_total - 1) - column) * 8)
			+ cel->relative_x + relative_x;

		ps->input = ps_input_hflip;
	}

	if (NOT_ZERO(cel->vflip))
	{
		position_y = (((cel->row_total - 1) - row) * 8)
			+ cel->relative_y + relative_y;

		ps->input = ps_input_vflip;
	}

	if (NOT_ZERO(cel->hflip) && NOT_ZERO(cel->vflip))
	{
		position_x = (((cel->column_total - 1) - column) * 8)
			+ cel->relative_x + relative_x;
		position_y = (((cel->row_total - 1) - row) * 8)
			+ cel->relative_y + relative_y;

		ps->input = ps_input_vhflip;
	}

	set_region(
		ps->destination_clip,
		position_x,
		position_y,
		8, 8);

	ps->mode = PS_TRANSPARENTPIXEL;

	ps->output = ps_output_default;
	ps->plot = ps_plot_md_indexed;
}
