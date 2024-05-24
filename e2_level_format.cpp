#include "blue-dream.h"

void md_pattern_name_set_resource(
	t_md_pattern_name *pn,
	t_byte *resource,
	int index)
{
	int z = unpack_word(resource, index * 2);

	pn->priority = (z & 0x8000) >> 15;
	pn->palette_id = (z & 0x7000) >> 13;
	pn->vflip = (z & 0x1000) >> 12;
	pn->hflip = (z & 0x0800) >> 11;
	pn->pattern_id = z & 0x07ff;
}

void plot_etd_tile(
	t_plot_state *ps,
	t_plot_state *pp,
	t_md_pattern_name *pn,
	int x, int y)
{
	if ((x + pp->source_w) < 0) return;
	if ((y + pp->source_h) < 0) return;
	if (x >= pp->destination_w) return;
	if (y >= pp->destination_h) return;

	t_plot_state *next = plot_state_add_vacant(ps);

	plot_state_set_palette(
		next,
		pp->palette,
		16,
		64,
		pn->palette_id);

	plot_state_set_source(
		next,
		pp->source,
		pp->source_w,
		pp->source_h);

	next->source_size = pp->source_size;

	plot_state_set_destination(
		next,
		pp->destination,
		pp->destination_w,
		pp->destination_h);
	
	set_horizontal_clip(
		x,
		next->source_w,
		next->destination_w,
		next->source_clip,
		next->destination_clip
	);

	set_vertical_clip(
		y,
		next->source_h,
		next->destination_h,
		next->source_clip,
		next->destination_clip
	);
	
	plot_state_set_source_region(
		next,
		0, 0, 8, 8,
		pn->pattern_id);
	
	next->mode = pp->mode;
	if (pn->priority == 0)
		next->mode |= PS_SHADOW;
		
	if (NOT_ZERO(pn->vflip) && NOT_ZERO(pn->hflip))
		next->input = ps_input_vhflip;
	else  if (NOT_ZERO(pn->vflip))
		next->input = ps_input_vflip;
	else if (NOT_ZERO(pn->hflip))
		next->input = ps_input_hflip;
	else
		next->input = ps_input_default;

	next->output = ps_output_default;
	next->plot = ps_plot_md_indexed;
}

void plot_etd_metatile(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *resource,
	int position_x, int position_y,
	int w, int h)
{
	int x = 0, y = 0, z = 0;

	t_md_pattern_name *pn = m_alloc(t_md_pattern_name, 1);

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			md_pattern_name_set_resource(pn, resource, z);

			plot_etd_tile(
				ps,
				pp,
				pn,
				position_x + (x * 8),
				position_y + (y * 8));

			z++;
		}
	}

	free(pn);
}

t_dword *get_metatile_offset_table(t_byte *resource, int offset, int total)
{
	int index = 0;
	t_dword *table = NULL;
		
	table = m_array(t_dword, total);
	if (table == NULL) return NULL;

	for (index = 0; index < total; index++)
		table[index] = unpack_word(
			resource, offset + (index * 4));

	return table;
}

t_dword *get_metatile_ptable(t_byte *resource, int offset, int total)
{
	int index = 0;
	t_dword *ptable = NULL;

	ptable = m_array(t_dword, total);
	if (ptable == NULL) return NULL;

	for (index = 0; index < total; index++)
		ptable[index] = unpack_dword(
			resource, offset + (index * 4));

	return ptable;
}

t_md_metatile *get_md_metatile(
	t_byte *resource,
	int offset,
	int w,
	int h
)
{
	int index = 0, size = 0;
	t_byte *u = 0;

	t_byte *raw = NULL;
	t_md_metatile *metatile = NULL;

	u = &resource[offset];
	if (w == 0 || h == 0)
	{
		w = u[0];
		h = u[1];
		u = &resource[offset + 2];
	}

	size = w * h * 2;
	raw = m_array(t_byte, size);
	if (raw == NULL) return NULL;

	for (index = 0; index < size; index++)
		raw[index] = 0;

	for (index = 0; index < size; index++)
		raw[index] = u[index];

	metatile = m_alloc(t_md_metatile);
	if (metatile == NULL) goto _RELEASE;

	metatile->w = w;
	metatile->h = h;
	metatile->raw = raw;

	return metatile;

_RELEASE:

	if (NOT_NULL(raw))
		free(raw);

	return NULL;
}

void release_md_metatile(t_md_metatile *md_metatile)
{
	if (NOT_NULL(md_metatile))
	{
		if (NOT_NULL(md_metatile->raw))
		{
			free(md_metatile->raw);
			md_metatile->raw = NULL;
		}

		free(md_metatile);
	}
}

t_md_metatile **create_md_metatile_table(int total)
{
	int index = 0;
	t_md_metatile **table = NULL;

	table = m_parray(t_md_metatile, total);
	if (table == NULL) return NULL;

	for (index = 0; index < total; index++)
		table[index] = NULL;

	return table;
}

void release_md_metatile_table(t_md_metatile **table, int total)
{
	int index = 0;

	if (NOT_NULL(table))
	{
		for (index = 0; index < total; index++)
		{
			if (NOT_NULL(table[index]))
			{
				release_md_metatile(table[index]);
				table[index] = NULL;
			}
		}

		free(table);
		table = NULL;
	}
}

int get_ram_md_metatile_table(
	t_md_metatile **md_metatile,
	t_byte *ram,
	t_dword *ptable,
	int total,
	int w,
	int h
)
{
	int index = 0, z = 0;

	for (index = 0; index < total; index++)
	{
		z = ptable[index];

		if (NOT_ZERO(z) && AND_EQUAL(z, MD_RAM_ADDRESS))
		{
			z = z & MD_RAM_MASK;

			md_metatile[index] = get_md_metatile(ram, z, w, h);
			if (md_metatile[index] == NULL)
			{
					release_md_metatile_table(md_metatile, total);
					return -1;
			}
		}
	}

	return 0;
}

int get_rom_md_metatile_table(
	t_md_metatile **md_metatile,
	t_byte *rom,
	t_dword *ptable,
	int total,
	int w,
	int h
)
{
	int index = 0, z = 0;

	for (index = 0; index < total; index++)
	{
		z = ptable[index];

		if (NOT_ZERO(z) && AND_NEQUAL(z, MD_RAM_ADDRESS))
		{
			md_metatile[index] = get_md_metatile(rom, z, w, h);
			if (md_metatile[index] == NULL)
			{
				release_md_metatile_table(md_metatile, total);
				return -1;
			}
		
		}
	}

	return 0;
}

t_bitmap **create_mtile_table(
	t_plot_state *ps,
	t_plot_state *pp,
	t_md_metatile **md_metatile,
	int total
)
{
	int index = 0, z = 0;
	int w = 0, h = 0;

	t_byte *u = NULL;

	t_bitmap *bitmap = NULL;
	t_bitmap **table = NULL;
	
	table = m_parray(t_bitmap, total);
	if (table == NULL) goto _ERROR;

	for (index = 0; index < total; index++)
		table[index] = NULL;

	for (index = 0; index < total; index++)
	{
		if (NOT_NULL(md_metatile[index]))
		{
			w = md_metatile[index]->w;
			h = md_metatile[index]->h;

			bitmap = create_bitmap(w * 8, h * 8);
			if (bitmap == NULL) goto _ERROR;

			plot_state_set_destination(
				pp,
				bitmap->resource,
				bitmap->w,
				bitmap->h);

			plot_etd_metatile(
				ps,
				pp,
				md_metatile[index]->raw,
				0, 0,
				w, h);

			plot_state_plot_all(ps);
			table[index] = bitmap;
			
		}
		else table[index] = NULL;
	}

	return table;

_ERROR:

	return NULL;
}

