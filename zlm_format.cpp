#include "blue-dream.h"

// need to initialize values
t_zlm_sprite_cel *create_zlm_sprite_cel(void)
{
	t_zlm_sprite_cel *cel = (t_zlm_sprite_cel *)malloc(
		sizeof(t_zlm_sprite_cel));

	return cel;
}

t_zlm_header *create_zlm_header(void)
{
	t_zlm_header *header = (t_zlm_header *)malloc(
		sizeof(t_zlm_header));

	return header;
}

int zlm_header_set_resource(t_zlm_header *header, t_byte *resource)
{
	t_dword x = 0, y = 0, z = 0;
	int index = 0, total = 0;

	header->id = unpack_dword(resource, 0);

	x = unpack_dword(resource, 4);
	y = unpack_word(resource, 8);
	z = unpack_word(resource, 10);

	x = bswap(wswap(bswap(x)));
	y = bswap(y);
	z = bswap(z);

	header->size = x;
	header->sequence_total = y;
	header->cel_total = z;

	total = header->sequence_total;

	header->sequence = (t_dword *)malloc(
		sizeof(t_dword) * total);

	if (header->sequence == NULL)
		return -1;

	for (index = 0; index < total; index++)
	{
		x = unpack_dword(resource, 12 + (index * 4));
		x = bswap(wswap(bswap(x)));

		header->sequence[index] = x;
	}

	return 0;
}

void zlm_cel_set_resource(t_zlm_sprite_cel *cel, t_byte *resource)
{
	t_dword w = 0, x = 0;
	t_dword y = 0, z = 0;

	w = unpack_word(resource, 0);
	x = unpack_word(resource, 2);
	y = unpack_word(resource, 4);
	z = unpack_word(resource, 6);

	w = bswap(w);
	x = bswap(x);
	y = bswap(y);
	z = bswap(z);

	cel->relative_x = w;
	cel->relative_y = x;
	cel->width = y;
	cel->height = z;

	w = unpack_word(resource, 8);
	x = unpack_word(resource, 10);
	y = unpack_dword(resource, 12);
	z = unpack_dword(resource, 16);

	w = bswap(w);
	x = bswap(x);
	y = bswap(wswap(bswap(y)));
	z = bswap(wswap(bswap(z)));

	cel->mask_width = w;
	cel->mask_column_size = x;
	cel->bitmap_offset = y;
	cel->mask_offset = z;
}

t_dword *plz_palette_create_resource(t_byte *resource)
{
	int x = 0, y = 0, size = 0;
	t_dword *palette = NULL;

	size = PLZ_PALETTE_SIZE;

	palette = (t_dword *)malloc(sizeof(t_dword) * size);
	if (palette == NULL) { return NULL; }

	for (x = 0; x < size; x++)
	{
		y = unpack_dword(resource, x * 4);
		y = bswap(wswap(bswap(y)));

		palette[x] = y | 0xff000000;
	}

	return palette;
}

t_zlm_sprite_cel *zlm_cel_create_resource(
	t_byte *resource,
	int sequence_index,
	int cel_index,
	int *sequence_total,
	int *cel_total,
	int *_result)
{
	int x = 0, y = 0, z = 0;
	int result = 0;

	t_zlm_header *header = NULL;
	t_zlm_sprite_cel *cel = NULL;

	int sequence_cel_total = 0;
	int cel_offset = 0;

	header = create_zlm_header();
	if (header == NULL) { result = -1; goto _ERROR; }

	result = zlm_header_set_resource(header, resource);
	if (result < 0) { result = -2; goto _ERROR; }

	if (sequence_index >= header->sequence_total)
	{
		result = -3; goto _ERROR;
	}

	if (sequence_total != NULL)
		*sequence_total = header->sequence_total;

	x = header->sequence[sequence_index];

	if (sequence_index == (header->sequence_total - 1)) y = header->cel_total;
	else y = header->sequence[sequence_index + 1];

	sequence_cel_total = y - x;
	if (cel_index >= sequence_cel_total)
	{
		result = -4; goto _ERROR;
	}

	if (cel_total != NULL)
		*cel_total = sequence_cel_total;

	x = PC_SPRITE_SEQUENCE;
	y = header->sequence_total * 4;

	z = header->sequence[sequence_index] + cel_index;
	z *= PC_SPRITE_CEL_SIZE;

	cel_offset = x + y + z;

	cel = create_zlm_sprite_cel();
	if (cel == NULL) { result = -5; goto _ERROR; }

	zlm_cel_set_resource(cel, &resource[cel_offset]);
	return cel;

_ERROR:

	if (NOT_NULL(_result))
		*_result = result;

	return NULL;
}

int zlm_plot_sprite_bitmap(
	t_plot_process *pp,
	t_zlm_sprite_cel *cel,
	t_byte *resource)
{
	int index = 0, result = 0;
	int width = 0, height = 0, size = 0;

	t_byte *u = NULL;
	t_dword *bitmap = NULL;

	t_plot_state *ps = NULL;
	t_plot_state *next = NULL;;

	width = cel->width;
	height = cel->height;

	size = (cel->mask_offset - cel->bitmap_offset) + 32;
	bitmap = (t_dword *)malloc(sizeof(t_dword) * size);
	if (bitmap == NULL) { result = -5; goto _ERROR; }

	u = &resource[cel->bitmap_offset];
	for (index = 0; index < size; index++)
		bitmap[index] = u[index];

	ps = pp->ps;
	next = plot_state_add_vacant(ps);

	next->palette = pp->palette;
	next->palette_w = PLZ_PALETTE_SIZE;
	next->palette_total = 1;

	next->source_index = 0;

	plot_state_set_source(
		next,
		bitmap,
		width,
		height);

	plot_state_set_destination(
		next,
		pp->destination,
		width,
		height
	);

	set_region(next->source_clip, 0, 0, width, height);

	set_palette_id(next, 0);
	set_pattern_id(next, 0);

	set_region(next->destination_clip, 0, 0, 0, 0);

	next->input = NULL;
	next->output = NULL;

	next->plot = ps_plot_zlm;

	return 0;

_ERROR:

	return result;
}

int zlm_plot_sprite_mask(
	t_plot_process *pp,
	t_zlm_sprite_cel *cel,
	t_byte *resource)
{
	int index = 0, result = 0;
	int width = 0, height = 0, size = 0;

	t_byte *u = NULL;
	t_dword *mask = NULL;

	t_plot_state *ps = NULL;
	t_plot_state *next = NULL;;

	width = cel->width;
	height = cel->height;

	size = cel->mask_column_size * cel->height;
	mask = (t_dword *)malloc(sizeof(t_dword) * size);
	if (mask == NULL) { result = -6; goto _ERROR; }

	u = &resource[cel->mask_offset];
	for (index = 0; index < size; index++)
		mask[index] = u[index];

	ps = pp->ps;
	next = plot_state_add_vacant(ps);

	next->palette = pp->palette;
	next->palette_w = 512;
	next->palette_total = 1;

	next->source_index = 0;

	plot_state_set_source(
		next,
		mask,
		cel->mask_column_size, height);

	plot_state_set_destination(
		next,
		pp->destination,
		width, height
	);

	set_region(next->source_clip, 0, 0, width, height);
	set_pattern_id(next, 0);
	set_palette_id(next, 0);

	set_region(next->destination_clip, 0, 0, 0, 0);

	next->input = NULL;
	next->output = NULL;

	next->plot = ps_plot_mask;

	return 0;

_ERROR:

	return result;
}