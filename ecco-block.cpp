#include "blue-dream.h"

// still needs transparent because it can't tell whats transparent and whats not
int plot_md_pattern_name(
	t_plot_state *ps,
	t_bitmap *source,
	t_bitmap *destination,
	t_md_pattern_name *pattern_name,
	int x, int y, int mode)
{
	t_plot_state *next = NULL;
	int flip = 0;

	if ((x + source->w) < 0) return -1;
	if ((y + source->h) < 0) return -1;
	if (x >= destination->w) return -1;
	if (y >= destination->h) return -1;

	next = plot_state_add_vacant(ps);
	if (next == NULL) return -1;

	plot_state_set_palette(
		next,
		NULL,
		16, 64,
		pattern_name->palette_id);

	ps_set_source_bitmap(next, source);
	ps_set_destination_bitmap(next, destination);

	update_clipping_regions(next, x, y);

	next->mode = mode;
	if (pattern_name->priority == 0)
		next->mode |= PS_SHADOW;

	if (NOT_ZERO(pattern_name->hflip))
		flip |= PS_HFLIP;
	if (NOT_ZERO(pattern_name->vflip))
		flip |= PS_VFLIP;

	plot_state_set_flip(next, flip);

	next->output = ps_output_default;
	next->plot = ps_plot_md_indexed;

	return 0;
}

void md_pattern_name_set(
	t_md_pattern_name *pn,
	t_byte *container,
	int index)
{
	int z = unpack_word(container, index * 2);

	pn->priority = (z & 0x8000) >> 15;
	pn->palette_id = (z & 0x7000) >> 13;
	pn->vflip = (z & 0x1000) >> 12;
	pn->hflip = (z & 0x0800) >> 11;
	pn->pattern_id = z & 0x07ff;
}

void _md_pattern_name_set(
	t_md_pattern_name *pn,
	t_byte *container,
	int index)
{
	int z = unpack_word(container, index * 2);

	pn->priority = (z >> 15) & 0x1;
	pn->palette_id = (z >> 13) & 0x3;
	pn->vflip = (z >> 12) & 0x1;
	pn->hflip = (z >> 11) & 0x1;
	pn->pattern_id = z & 0x07ff;
}

int get_nametable_total(t_byte *resource, int offset)
{
	int index = 0, result = 0;

	while (1)
	{
		result = unpack_dword(
			resource, offset + (index * 4));

		result &= 0xff000000;

		if (NOT_EQUAL(result, 0xff000000) && NOT_ZERO(result))
			break;

		index++;
	}

	return index;
}

t_dword *get_nametable_offsets(t_byte *resource, int offset, int total)
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

void release_md_nametable(t_md_nametable *md_nametable)
{
	if (NOT_NULL(md_nametable))
	{
		if (NOT_NULL(md_nametable->container))
		{
			free(md_nametable->container);
			md_nametable->container = NULL;
		}

		free(md_nametable);
	}
}

t_md_nametable *get_md_nametable(
	t_byte *raw,
	int offset,
	int w,
	int h
)
{
	int index = 0, size = 0;
	t_byte *u = 0;

	t_md_nametable *nametable = NULL;
	t_byte *container = NULL;

	u = &raw[offset];
	if (w == 0 || h == 0)
	{
		w = u[0];
		h = u[1];
		u = &raw[offset + 2];
	}

	size = w * h * 2;

	nametable = m_alloc(t_md_nametable);
	if (nametable == NULL) return NULL;

	container = m_array(t_byte, size);
	if (container == NULL)
	{
		release_md_nametable(nametable);
		return NULL;
	}

	for (index = 0; index < size; index++)
		container[index] = 0;

	for (index = 0; index < size; index++)
		container[index] = u[index];

	nametable->w = w;
	nametable->h = h;
	nametable->container = container;

	return nametable;
}

t_md_nametable **create_md_nametable_map(int total)
{
	int index = 0;
	t_md_nametable **table = NULL;

	table = m_parray(t_md_nametable, total);
	if (table == NULL) return NULL;

	for (index = 0; index < total; index++)
		table[index] = NULL;

	return table;
}

void release_md_nametable_map(t_md_nametable **table, int total)
{
	int index = 0;

	if (NOT_NULL(table))
	{
		for (index = 0; index < total; index++)
		{
			if (NOT_NULL(table[index]))
			{
				release_md_nametable(table[index]);
				table[index] = NULL;
			}
		}

		free(table);
		table = NULL;
	}
}

t_md_nametable **get_md_nametable_map(
	t_byte *ram,
	t_byte *rom,
	t_dword *namtable_offsets,
	int total,
	int w,
	int h
)
{
	t_md_nametable **md_nametable_map = NULL;
	int index = 0, z = 0;
	t_byte *u = NULL;

	md_nametable_map = create_md_nametable_map(total);
	if (md_nametable_map == NULL) return NULL;

	for (index = 0; index < total; index++)
	{
		z = namtable_offsets[index];

		if (NOT_ZERO(z))
		{
			u = rom;
			
			if (AND_EQUAL(z, MD_RAM_ADDRESS))
			{
				z = z & MD_RAM_MASK;
				u = ram;
			}

			md_nametable_map[index] = get_md_nametable(u, z, w, h);
			if (md_nametable_map[index] == NULL)
			{
				release_md_nametable_map(md_nametable_map, total);
				return NULL;
			}

		}
	}

	return md_nametable_map;
}

t_bitmap *create_md_tile(
	t_dword *pattern,
	int size,
	int pattern_id)
{
	t_plot_state *ps = NULL;
	t_bitmap *destination = NULL;

	if ((pattern_id * MD_TILE_AREA) >= size)
		return NULL;

	destination = create_bitmap(8, 8);
	if (destination == NULL) return NULL;

	ps = create_plot_state();
	if (ps == NULL) return NULL;
	plot_state_add_vacant(ps);

	plot_state_set_source(ps, pattern, 8, 8);
	ps_set_destination_bitmap(
		ps, destination);

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

t_bitmap **create_md_tileset(
	t_dword *pattern, int size, int *_total)
{
	t_bitmap **table = NULL;
	int x = 0, y = 0, total = 0;

	total = size / MD_TILE_AREA;

	table = m_parray(t_bitmap, total);
	if (table == NULL) return NULL;

	for (x = 0; x < total; x++)
		table[x] = NULL;

	for (x = 0; x < total; x++)
	{
		table[x] = create_tile(
			pattern, MD_TILE_WIDTH, MD_TILE_HEIGHT, 
			size, x);

		if (table[x] == NULL)
		{
			for (y = 0; y < total; y++)
			{
				if (NOT_NULL(table[y]))
				{
					free(table[y]);
					table[y] = NULL;
				}
			}

			free(table);
			table = NULL;

			return NULL;
		}
	}

	if (NOT_NULL(_total))
		*_total = total;

	return table;
}

t_bitmap *create_ecco_block(
	t_bitmap **tileset,
	t_byte *container,
	int w, int h, int mode)
{
	int row = 0, column = 0, index = 0;

	t_plot_state *ps = NULL;
	t_bitmap *destination = NULL;
	t_md_pattern_name *pattern_name = NULL;
	
	ps = create_plot_state();
	if (ps == NULL) return NULL;
	
	for (index = 0; index < (w * h); index++)
		plot_state_add(ps);
	plot_state_clear_all(ps);

	destination = create_bitmap(w * 8, h * 8);
	if (destination == NULL) return NULL;

	pattern_name = m_alloc(t_md_pattern_name, 1);
	if (pattern_name == NULL) return NULL;

	for (index = 0, row = 0; row < h; row++)
	{
		for (column = 0; column < w; column++)
		{
			md_pattern_name_set(pattern_name, container, index);

			plot_md_pattern_name(
				ps,
				tileset[pattern_name->pattern_id],
				destination,
				pattern_name,
				column * 8,
				row * 8,
				mode);

			index++;
		}
	}

	plot_state_plot_all(ps);
	release_plot_state(ps);

	free(pattern_name);
	return destination;
}

t_bitmap **create_ecco_blockset(
	t_md_nametable **md_nametable_map,
	t_bitmap **tileset,
	int total, int mode
)
{
	int index = 0;
	int w = 0, h = 0;

	t_bitmap *bitmap = NULL;
	t_bitmap **table = NULL;

	table = m_parray(t_bitmap, total);
	if (table == NULL) return NULL;

	for (index = 0; index < total; index++)
		table[index] = NULL;

	for (index = 0; index < total; index++)
	{
		if (NOT_NULL(md_nametable_map[index]))
		{
			w = md_nametable_map[index]->w;
			h = md_nametable_map[index]->h;

			bitmap = create_ecco_block(
				tileset,
				md_nametable_map[index]->container,
				w, h, mode);

			if (bitmap == NULL)
			{
				return NULL;
			}

			table[index] = bitmap;
		}

		else table[index] = NULL;
	}

	return table;
}