#include "blue-dream.h"

int ecco_lsprite_cel_set_resource(
	t_ecco_sprite_cel *cel,
	t_byte *resource,
	int index)
{
	if (cel == NULL) return -1;
	if (resource == NULL) return -2;

	cel->terminate = resource[index + 0] & 0x80;

	cel->column_total = resource[index + 0] >> 4 & 0x3;
	cel->row_total = resource[index + 0] >> 2 & 0x3;

	cel->column_total++;
	cel->row_total++;

	cel->relative_y = unpack_word(resource, index + 0);
	cel->relative_y &= 0x1ff;

	cel->palette_id = resource[index + 2] >> 5 & 0x3;

	cel->vflip = resource[index + 2] >> 4 & 0x1;
	cel->hflip = resource[index + 2] >> 3 & 0x1;

	cel->pattern_id = unpack_word(resource, index + 2);
	cel->pattern_id &= 0x7ff;

	cel->relative_x = unpack_word(resource, index + 4);

	return 0;
}

int get_lsprite_cel_total(t_byte *resource)
{
	int index = 0, result = 0, total = 0;

	for (index = 0; index < SPRITECEL_MAX; index++)
	{
		total++;

		result = resource[(index * ECCO_SPRITE_CEL_SIZE)];
		if (AND_NEQUAL(result, 0x80))
			break;
	}

	return total;
}

t_ecco_sprite_cel **ecco_lsprite_cel_create_all(
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

		ecco_lsprite_cel_set_resource(
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

t_bitmap *plot_ecco_local_sprite(
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

	total = get_lsprite_cel_total(resource);
	cel = ecco_lsprite_cel_create_all(
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