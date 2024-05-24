#include "blue-dream.h"

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

void plot_etd_block(
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
			md_pattern_name_set(pn, resource, z);

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

t_bitmap **create_mtile_table(
	t_plot_state *ps,
	t_plot_state *pp,
	t_md_nametable **md_nametable,
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
		if (NOT_NULL(md_nametable[index]))
		{
			w = md_nametable[index]->w;
			h = md_nametable[index]->h;

			bitmap = create_bitmap(w * 8, h * 8);
			if (bitmap == NULL) goto _ERROR;

			plot_state_set_destination(
				pp,
				bitmap->container,
				bitmap->w,
				bitmap->h);

			plot_etd_block(
				ps,
				pp,
				md_nametable[index]->container,
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

t_dword get_ecco_world_property(int id, int type)
{
	t_dword table[] = {

		// PALETTE

		0x728e8, // HOMEBAY
		0xbf69a, // HARDWATER
		0xacc86, // CITYOFFOREVER
		0x9246a, // JURASSICBEACH
		0xa69da  // THEMACHINE
	};

	return table[id + type];
}

t_dword get_ecco_stage_property(int id, int type)
{
	t_dword table[] = {

		// EC_WORLD

		EC_WORLD_HOMEBAY, // HOMEBAY
		EC_WORLD_HOMEBAY, // UNDERCAVES
		EC_WORLD_HOMEBAY, // THEVENTS
		EC_WORLD_HOMEBAY, // THELAGOON
		EC_WORLD_HOMEBAY, // ISLANDZONE
		EC_WORLD_HOMEBAY, // RIDGEWATER
		EC_WORLD_HOMEBAY, // DEEPWATER
		EC_WORLD_HOMEBAY, // MEDUSABAY
		EC_WORLD_HOMEBAY, // OPENOCEAN
		EC_WORLD_HOMEBAY, // SELECTIONSCR
		EC_WORLD_HOMEBAY, // ATTRACTMODE
		EC_WORLD_HOMEBAY, // CELEBRATION

		EC_WORLD_JURASSICBEACH, // JURASSICBEACH
		EC_WORLD_JURASSICBEACH, // PTERANODONPOND
		EC_WORLD_JURASSICBEACH, // ORIGINBEACH
		EC_WORLD_JURASSICBEACH, // TRILOBITECIRCLE
		EC_WORLD_JURASSICBEACH, // DARKWATER

		EC_WORLD_THEMACHINE,    // THEMACHINE
		EC_WORLD_THEMACHINE,    // THETUBE
		EC_WORLD_THEMACHINE,    // RETURNTOEARTH

		EC_WORLD_CITYOFFOREVER, // DEEPCITY
		EC_WORLD_CITYOFFOREVER, // THEMARBLESEA
		EC_WORLD_CITYOFFOREVER, // THELIBRARY
		EC_WORLD_CITYOFFOREVER, // CITYOFFOREVER

		EC_WORLD_HARDWATER, // COLDWATER
		EC_WORLD_HARDWATER,     // HARDWATER
		EC_WORLD_HARDWATER,     // ICEZONE

		EC_WORLD_THEMACHINE,    // THELASTFIGHT

		// EC_NAMETABLE_ATTRIBUTE_OFFSETS

		0x73a72, // HOMEBAY
		0x73a9a, // UNDERCAVES
		0x73ac2, // THEVENTS
		0x73aea, // THELAGOON
		0x73b12, // ISLANDZONE
		0x73b3a, // RIDGEWATER
		0x73b62, // DEEPWATER
		0x73b8a, // MEDUSABAY
		0x73bb2, // OPENOCEAN
		0x73bda, // SELECTIONSCR
		0x73c02, // ATTRACTMODE
		0x73c2a, // CELEBRATION
		0x93528, // JURASSICBEACH
		0x93550, // PTERANODONPOND
		0x93578, // ORIGINBEACH
		0x935a0, // TRILOBITECIRCLE
		0x935c8, // DARKWATER
		0xa6b12, // WELCOMETOTHEMACHINE
		0xa6b3a, // THETUBE
		0xa6b62, // RETURNTOEARTH
		0xadeac, // DEEPCITY
		0xaded4, // THEMARBLESEA
		0xadefc, // THELIBRARY
		0xadf24, // CITYOFFOREVER
		0xc0776, // COLDWATER
		0xc079e, // HARDWATER
		0xc07c6, // ICEZONE
		0xca6fe  // THELASTFIGHT
	};

	return table[id + type];
}

char *get_ecco_stage_save_state(int id)
{
	const char *table[] = {
		"HOMEBAY.gsx",
		"UNDERCAVES.gsx",
		"THEVENTS.gsx",
		"THELAGOON.gsx",
		"ISLANDZONE.gsx",
		"RIDGEWATER.gsx",
		"DEEPWATER.gsx",
		"MEDUSABAY.gsx",
		"OPENOCEAN.gsx",
		"SELECTIONSCR.gsx",
		"ATTRACTMODE.gsx",
		"CELEBRATION.gsx",
		"JURASSICBEACH.gsx",
		"PTERANODONPOND.gsx",
		"ORIGINBEACH.gsx",
		"TRILOBITECIRCLE.gsx",
		"DARKWATER.gsx",
		"WELCOMETOTHEMACHINE.gsx",
		"THETUBE.gsx",
		"RETURNTOEARTH.gsx",
		"DEEPCITY.gsx",
		"THEMARBLESEA.gsx",
		"THELIBRARY.gsx",
		"CITYOFFOREVER.gsx",
		"COLDWATER.gsx",
		"HARDWATER.gsx",
		"ICEZONE.gsx",
		"THELASTFIGHT.gsx"
	};

	return copy_append_string(
		"data/ECCO_US_OCT_1992/", table[id]);
}

t_dword get_ecco_nametable_attribute(
	t_byte *rom,
	int offset,
	int id)
{
	t_dword table[EC_NAMETABLE_ATTRIBUTE_TOTAL];

	table[EC_FOREGROUND_W] = unpack_word(rom, offset + 0x0);
	table[EC_FOREGROUND_H] = unpack_word(rom, offset + 0x2);
	table[EC_FG_NAMETABLE_OFFSET] = unpack_dword(rom, offset + 0x8);

	table[EC_BACKGROUND_W] = unpack_word(rom, offset + 0x10);
	table[EC_BACKGROUND_H] = unpack_word(rom, offset + 0x12);
	table[EC_PALETTEROWATTRIBUTE_OFFSET] = unpack_dword(rom, offset + 0x14);
	table[EC_BG_NAMETABLE_OFFSET] = unpack_dword(rom, offset + 0x18);

	return table[id];
}

void set_etd_level_cell(t_etd_level_cell *x, int y)
{
	x->hflip = (y >> 7) & 0x1;
	x->vflip = (y >> 6) & 0x1;
	x->mtile_id = y & 0x3f;

	if (x->vflip == 1)
		x->vflip = PS_VFLIP;
	if (x->hflip == 1)
		x->hflip = PS_HFLIP;
}

t_bitmap **etd_create_mtile_table(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *ram,
	t_byte *resource,
	t_dword offset,
	int total,
	int flag
)
{
	int index = 0;
	int x = 0, y = 0, z = 0;
	int width = 0, height = 0;
	int size = 0;

	t_byte *u = NULL;

	t_dword *ptable = NULL;
	t_byte *raw = NULL;

	t_bitmap *bitmap = NULL;
	t_bitmap **table = NULL;
	t_md_nametable *block = NULL;
	t_md_nametable **_block = NULL;

	ptable = get_nametable_offsets(ram, offset, total);
	if (ptable == NULL) return NULL;

	_block = m_parray(t_md_nametable, 0x100);
	for (index = 0; index < total; index++)
		_block[index] = NULL;

	u = resource;
	for (index = 0; index < total; index++)
	{
		z = ptable[index];
		if (z == 0) continue;

		if (AND_NEQUAL(z, MD_RAM_ADDRESS))
		{
			_block[index] = get_md_nametable(u, z, 16, 16);
		}
	}

	u = ram;
	for (index = 0; index < total; index++)
	{
		z = ptable[index];
		if (z == 0) continue;

		if (AND_EQUAL(z, MD_RAM_ADDRESS))
		{
			z = z & 0x0000ffff;
			_block[index] = get_md_nametable(u, z, 16, 16);
		}
	}

	table = m_parray(t_bitmap, 0x100);
	if (table == NULL) goto _ERROR;

	for (index = 0; index < 0x100; index++)
		table[index] = NULL;

	for (x = 0; x < total; x++)
	{
		if (NOT_NULL(_block[x]))
		{
			width = _block[x]->w;
			height = _block[x]->h;

			bitmap = create_bitmap(width * 8, height * 8);
			if (bitmap == NULL) goto _ERROR;

			plot_state_set_destination(
				pp,
				bitmap->container,
				bitmap->w,
				bitmap->h);

			plot_etd_block(
				ps,
				pp,
				_block[x]->container,
				0, 0,
				width, height);

			plot_state_plot_all(ps);
			table[x] = bitmap;

		}
		else table[x] = NULL;
	}

	free(ptable);
	free(raw);

	index = 0;

	plot_state_set_destination(
		pp,
		table[index]->container,
		table[index]->w,
		table[index]->h);

	return table;

_ERROR:

	if (NOT_NULL(ptable))
		free(ptable);

	if (NOT_NULL(raw))
		free(raw);

	if (NOT_NULL(table))
	{
		for (index = 0; index < ETD_BLOCK_SIZE; index++)
			if (NOT_NULL(table[index]))
				free(table[index]);
	}

	return NULL;
}

int etd_generate_level_map
(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *sav_resource,
	int level_w,
	int level_h,
	t_bitmap **mtile_table_1
)
{
	int index = 0;
	int w = 0, x = 0, y = 0, z = 0;
	t_byte *u = NULL;

	int level_size = level_w * level_h;
	int table_index = 0;

	t_dword *level_table_1 = (t_dword *)malloc(sizeof(t_dword) * level_size * 2);

	u = &sav_resource[level_size * 0];
	for (index = 0; index < level_size; index++)
		level_table_1[index] = u[index];

	t_bitmap *level_map = create_bitmap(level_w * 0x80, level_h * 0x80);

	for (index = 0; index < level_map->size; index++)
		level_map->container[index] = 0;

	t_bitmap *bitmap = NULL;
	t_plot_state *next = NULL;

	t_etd_level_cell *cell1 = m_alloc(t_etd_level_cell);

	for (y = 0; y < level_h; y++)
	{
		for (x = 0; x < level_w; x++)
		{
			table_index = x + (y * level_w);

			if (table_index >= level_size)
				continue;

			z = level_table_1[table_index];
			if (z == 0xff) continue;

			set_etd_level_cell(cell1, z);

			bitmap = mtile_table_1[cell1->mtile_id];
			if (bitmap == NULL) continue;

			plot_bitmap_direct(
				ps,
				bitmap,
				level_map,
				x * 128, y * 128,
				(cell1->vflip | cell1->hflip));

			plot_state_plot_all(ps);
		}
	}

	plot_state_set_destination(
		pp,
		level_map->container,
		level_map->w,
		level_map->h);

	return 0;
}