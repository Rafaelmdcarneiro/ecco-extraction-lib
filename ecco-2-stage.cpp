#include "w_interface.h"

int ecco_get_row_palette_index(t_dword *row_attributes, int row)
{
	int index = 0, palette_row = 0, palette_id = 0;

	if (row == 0) return row_attributes[1];

	while (row >= palette_row + 1)
	{
		palette_row = row_attributes[(index * 2)];
		palette_id = row_attributes[(index * 2) + 1];

		if (NOT_ZERO(palette_row))
			index++;
		else break;
	}

	return palette_id;
}

int ps_plot_ecco_indexed(t_plot_state *ps)
{
	int x = 0, y = 0;
	int palette_row = 0, palette_id = 0;
	t_dword *row_attributes = NULL;

	if (NOT_NULL(ps->parameters))
		row_attributes = (t_dword *)ps->parameters;

	for (y = 0; y < ps->destination_clip->h; y++)
	{
		if (NOT_NULL(row_attributes))
		{
			// fix 0x80
			palette_id = ecco_get_row_palette_index(
				row_attributes, (y / 0x80));

			set_palette_id(ps, palette_id);
		}

		for (x = 0; x < ps->destination_clip->w; x++)
			ps_plot_default_pixel(ps, x, y);
	}

	return 0;
}

int plot_bitmap_ecco_indexed(
	t_plot_state *ps,
	t_dword *row_attributes,
	t_dword *palette,
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
	next->parameters = row_attributes;

	plot_state_set_palette(next, palette, 64, 0, 0);
	next->palette_index = 0;

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
	next->plot = ps_plot_ecco_indexed;

	return 0;
}

int draw_bitmap_ecco_indexed(
	t_dword *row_attributes,
	t_dword *palette,
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int mode)
{
	t_plot_state *ps = NULL;

	ps = create_plot_state();
	if (ps == NULL) return -1;
	plot_state_clear_all(ps);

	if (plot_bitmap_ecco_indexed(
		ps,
		row_attributes,
		palette,
		source,
		destination,
		x, y,
		mode) == -1)
		return -1;

	plot_state_plot_all(ps);
	release_plot_state(ps);

	return 0;
}

t_dword get_ecco2_domain_header_offset(int id)
{
	// if (id > E2_DOMAIN_TOTAL) return -1;

	t_dword table[] = {

		// domain header offset locations 0xa55c (ecco 2), 0xa456 (paradox)

		// ECCO2
		0x1bd7fc, // PROLOGUE
		0x10af8c, // HOMEBAY
		0x135a48, // TRELLIASBAY
		0x1558e8, // AQUATUBEWAY
		0xecbd0,  // SEAOFGREEN
	    0,        // ASTERITESHOME
		0x193920, // LUNARBAY
		0x180b00, // VORTEXFUTURE
		0x106bc0, // GLOBEHOLDER
		0x1b3ecc, // VORTEXQUEEN
		0x1d7a44, // BIGWATER
		0x16537c, // ATLANTIS
		0x1dbf70, // TMACHINE

		// ECCOJR
		0xce198, // HOMESEA
		0xae290, // AQUA MAZE
		0xbe3ec, // VENTSOFPEARL
		0xe3594  // THE ENDLESS SEA
	};

	return table[id];
}

char *get_ecco2_stage_save_state(int id)
{
	const char *table[] = {
		"PROLOGUE.gsx",
		"SELECTIONSCR.gsx",
		"TWOTIDES.gsx",
		"THELOSTORCAS.gsx",
		"VENTSOFMEDUSA.gsx",
		"FOURISLANDS.gsx",
		"MAZEOFSTONE.gsx",
		"HOMEBAY.gsx",
		"SEAOFDARKNESS.gsx",
		"CRYSTALSPRINGS.gsx",
		"FAULTZONE.gsx",
		"GATEWAY.gsx",
		"TRELLIASBAY.gsx",
		"SKYWAY.gsx",
		"ASTERITESCAVE.gsx",
		"EAGLESBAY.gsx",
		"FINTOFEATHER.gsx",
		"SKYLANDS.gsx",
		"TUBEOFMEDUSA.gsx",
		"VORTEXARRIVED.gsx",
		"AQUATUBEWAY.gsx",
		"SKYTIDES.gsx",
		"SEAOFGREEN.gsx",
		"DEEPRIDGE.gsx",
		"THEEYE.gsx",
		"SEAOFBIRDS.gsx",
		"SECRETCAVE.gsx",
		"THEHUNGRYONES.gsx",
		"CONVERGENCE.gsx",
		"MORAYABYSS.gsx",
		"ASTERITESHOME.gsx",
		"EPILOGUE.gsx",
		"LUNARBAY.gsx",
		"DARKSEA.gsx",
		"NEWMACHINE.gsx",
		"INSIDE.gsx",
		"INTER.gsx",
		"INNUENDO.gsx",
		"TRANS.gsx",
		"BLACKCLOUDS.gsx",
		"VORTEXFUTURE.gsx",
		"GRAVITORBOX.gsx",
		"GLOBEHOLDER.gsx",
		"VORTEXQUEEN.gsx",
		"BIGWATER.gsx",
		"THEPOD.gsx",
		"ATLANTIS.gsx",
		"FISHCITY.gsx",
		"CITYOFFOREVER.gsx",
		"TMACHINE_.gsx",

		// ECCO JR
		"HOMESEA.gsx",
		"BAYOFSONGS.gsx",
		"AQUAMAZE.gsx",
		"VENTSOFPEARL.gsx",
		"THEENDLESSSEA.gsx"
	};

	return copy_append_string(
		"data/ECCO_2_US_JUNE_1994/", table[id]);
}

int get_palette_total(int *row_attribute_palette)
{
	int index = 0;
	int palette_total = 0;

	while (index < EC_ROW_PALETTE_TOTAL)
	{
		if (palette_total < row_attribute_palette[index])
			palette_total = row_attribute_palette[index];;

		index++;
	}

	return palette_total;
}

t_ecco2_stage_properties *ecco2_read_stage_properties(
	t_byte *rom, int stage_id)
{
	t_ecco2_stage_properties *properties = NULL;

	int domain_id = 0;
	int domain_stage_id = 0;
	int domain_header_offset = 0;
	int stage_list_offset = 0;
	int stage_header_offset = 0;
	int row_attributes_offset = 0;
	int stage_name = 0;

	properties = m_alloc(t_ecco2_stage_properties);
	if (properties == NULL) return NULL;

	memset(properties, 0, sizeof(t_ecco2_stage_properties));

	// domain header

	domain_id = lword(bswap(stage_id & E2_STAGE_DOMAIN_MASK));
	domain_stage_id = lword(stage_id & E2_DOMAIN_STAGE_MASK);

	domain_header_offset = get_ecco2_domain_header_offset(domain_id);
	if (domain_header_offset == -1) return NULL;

	

	properties->stage_index = wswap(stage_id & E2_STAGE_INDEX_MASK);

	properties->palette_offset = unpack_dword(rom, domain_header_offset + 0xe);
	properties->local_sprite_offset = unpack_dword(rom, domain_header_offset + 0x1a);
	properties->blockset2_offset = unpack_dword(rom, domain_header_offset + 0x1e);
	properties->stage_list_offset = unpack_dword(rom, domain_header_offset + 0x2e);

	// stage header list

	stage_list_offset = properties->stage_list_offset;

	properties->stage_total = unpack_word(rom, stage_list_offset);
	properties->blockset1_offset = unpack_dword(rom, stage_list_offset + 0x2);

	properties->blockset1_total = get_nametable_total(rom, properties->blockset1_offset);
	properties->blockset2_total = get_nametable_total(rom, properties->blockset2_offset);

	// stage properties

	if (domain_stage_id >= properties->stage_total)
		return NULL;

	properties->domain_stage_id = domain_stage_id;
	stage_header_offset = (stage_list_offset + 0x6) + (domain_stage_id * E2_STAGE_HEADER_SIZE);

	properties->objectmap2_offset = unpack_dword(rom, stage_header_offset);
	properties->objectmap1_offset = unpack_dword(rom, stage_header_offset + 0x4);

	properties->foreground_w = unpack_word(rom, stage_header_offset + 0x8);
	properties->foreground_h = unpack_word(rom, stage_header_offset + 0xa);

	properties->origin_x = unpack_word(rom, stage_header_offset + 0xc);
	properties->origin_y = unpack_word(rom, stage_header_offset + 0xe);

	properties->foreground_offset = unpack_dword(rom, stage_header_offset + 0x10);
	if (properties->foreground_offset == 0xe5ff0000)
		properties->foreground_offset &= MD_RAM_MASK;

	properties->background_w = unpack_word(rom, stage_header_offset + 0x18);
	properties->background_h = unpack_word(rom, stage_header_offset + 0x1a);

	properties->attributes_offset = unpack_dword(rom, stage_header_offset + 0x1c);
	properties->background_offset = unpack_dword(rom, stage_header_offset + 0x20);

	properties->objectmap_w = unpack_word(rom, stage_header_offset + 0x24);
	properties->objectmap_h = unpack_word(rom, stage_header_offset + 0x26);

	row_attributes_offset = properties->attributes_offset + 0x11;

	properties->row_attribute_row[0] = rom[row_attributes_offset + 1];
	properties->row_attribute_row[1] = rom[row_attributes_offset + 3];
	properties->row_attribute_row[2] = rom[row_attributes_offset + 5];
	properties->row_attribute_row[3] = rom[row_attributes_offset + 7];

	properties->row_attribute_palette[0] = rom[row_attributes_offset + 0];
	properties->row_attribute_palette[1] = rom[row_attributes_offset + 2];
	properties->row_attribute_palette[2] = rom[row_attributes_offset + 4];
	properties->row_attribute_palette[3] = rom[row_attributes_offset + 6];

	properties->palette_total = get_palette_total(properties->row_attribute_palette);

	stage_name = properties->attributes_offset + 0x20;
	clone_string((char *)&rom[stage_name], properties->stage_name);

	return properties;
}

void set_stage_cell1(t_ecco_stage_cell *x, int y)
{
	x->table_id = y >> 8;
	x->hflip = (y >> 9) & 0x1;
	x->vflip = (y >> 8) & 0x1;
	x->block_id = y & 0x00ff;

	if (x->vflip == 1)
		x->vflip = PS_VFLIP;
	if (x->hflip == 1)
		x->hflip = PS_HFLIP;
}

void set_stage_cell2(t_ecco_stage_cell *x, int y)
{
	x->table_index = (y & 0xffff)/2;

	x->relative_x = (y >> 8) & 0xf;
	x->relative_y = (y >> 12) & 0xf;

	x->block_id = y & 0x00ff;
}

void ecco2_get_stage_layer(
	t_ecco2_stage_properties *properties,
	int mode,
	int *_stage_w,
	int *_stage_h,
	int *_stage_size,
	int *_stage_offset)
{
	switch (mode & EC_LAYER_MASK)
	{
		case EC_BACKGROUND:
		{
			*_stage_w = properties->background_w;
			*_stage_h = properties->background_h;

			*_stage_size = properties->objectmap1_offset - 
				properties->background_offset;

			*_stage_offset = properties->background_offset;

			break;
		}

		case EC_FOREGROUND:
		default:
		{
			*_stage_w = properties->foreground_w;
			*_stage_h = properties->foreground_h;

			*_stage_size = properties->background_offset - 
				properties->foreground_offset;

			*_stage_offset = properties->foreground_offset;

			break;
		}
	}
}

void ecco_calculate_map_offsets(
	int *_x,
	int *_y,
	int *_column_offset,
	int *_row_offset)
{
	*_column_offset = *_x / EC_BLOCK_PIXEL_W;
	*_row_offset = *_y / EC_BLOCK_PIXEL_H;

	*_x = *_x - (*_column_offset * EC_BLOCK_PIXEL_W);
	*_y = *_y - (*_row_offset * EC_BLOCK_PIXEL_H);
}

void ecco_calculate_map_dimensions(
	int x,
	int y,
	int map_width,
	int map_height,
	int *_w,
	int *_h,
	int *_column_length,
	int *_row_length
)
{
	if (*_w == 0) *_w = (map_width * EC_BLOCK_PIXEL_W) - x;
	if (*_h == 0) *_h = (map_height * EC_BLOCK_PIXEL_H) - y;

	*_column_length = (((*_w + x) / EC_BLOCK_PIXEL_W) + 1) + 1;
	*_row_length = (((*_h + y) / EC_BLOCK_PIXEL_H) + 1) + 1;
}

// A cleaner solution to creating stage maps is to convert all the
// tables to a format so the loop can focus on just drawing the blocks.
// This is in contrast with determining which table is being used and 
// handling a drawing operation based on said table.
// If this method were to be used then theoretically all the maps
// could easily be converted with very little effort. 

t_bitmap *ecco2_create_stage_map
(
	t_ecco2_stage_properties *properties,
	t_dword *palette,
	t_byte *ram,
	t_bitmap **blockset_1,
	t_bitmap **blockset_2,
	int x, int y,
	int w, int h,
	int mode
)
{
	int index = 0, offset = 0, result = 0;

	int stage_w = 0, stage_h = 0;
	int stage_area = 0, stage_size = 0;
	int stage_offset = 0;
	
	int row = 0, column = 0;
	int row_offset = 0, column_offset = 0;
	int row_length = 0, column_length = 0;

	int table_id = 0;
	int relative_x = 0, relative_y = 0;

	int table1_offset = 0, table2_offset = 0, table3_offset = 0;

	t_dword *table = NULL;
	t_dword row_attributes[EC_ROW_PALETTE_TOTAL * 2];

	t_ecco_stage_cell *cell1 = NULL;
	t_ecco_stage_cell *cell2 = NULL;
	t_ecco_stage_cell *cell3 = NULL;

	t_bitmap *bitmap = NULL;
	t_bitmap *blank = NULL;
	t_bitmap *destination = NULL;

	ecco2_get_stage_layer(
		properties,
		mode,
		&stage_w,
		&stage_h,
		&stage_size,
		&stage_offset);

	if (AND_EQUAL(stage_size, MD_RAM_ADDRESS))
		stage_size &= MD_RAM_MASK;
	if (AND_EQUAL(stage_offset, MD_RAM_ADDRESS))
		stage_offset &= MD_RAM_MASK;

	ecco_calculate_map_offsets(
		&x, &y,
		&column_offset,
		&row_offset);

	ecco_calculate_map_dimensions(
		x, y,
		stage_w,
		stage_h,
		&w, &h,
		&column_length,
		&row_length
	);

	stage_area = stage_w * stage_h;

	table2_offset = stage_area * 1;
	table3_offset = stage_area * 2;

	table = m_array(t_dword, stage_size);
	if (table == NULL) return NULL;

	for (index = 0; index < stage_size; index++)
		table[index] = unpack_word(ram, stage_offset + (index * 2));

	destination = create_bitmap(w, h);
	if (destination == NULL) return NULL;

	blank = create_bitmap(EC_BLOCK_PIXEL_W, EC_BLOCK_PIXEL_H);
	if (blank == NULL) return NULL;

	for (index = 0; index < blank->size; index++)
		blank->container[index] = PS_TRANSPARENTPIXEL | PS_SHADOW;

	for (index = 0; index < destination->size; index++)
		destination->container[index] = PS_TRANSPARENTPIXEL;

	for (index = 0; index < EC_ROW_PALETTE_TOTAL; index++)
	{
		row_attributes[(index * 2)] = (t_dword)properties->row_attribute_row[index];
		row_attributes[(index * 2) + 1] = (t_dword)properties->row_attribute_palette[index];
	}

	cell1 = m_alloc(t_ecco_stage_cell);
    cell2 = m_alloc(t_ecco_stage_cell);
	cell3 = m_alloc(t_ecco_stage_cell);

	for (row = 0; row < row_length; row++)
	{
		if ((row_offset + row) >= stage_h)
			break;

		for (column = 0; column < column_length; column++)
		{
			if ((column_offset + column) >= stage_w)
				break;

			offset = (column_offset + column) + ((row_offset + row) * stage_w);
			set_stage_cell1(cell1, table[offset]);

			relative_x = (column * EC_BLOCK_PIXEL_W) - x;
			relative_y = (row * EC_BLOCK_PIXEL_H) - y;

			table_id = cell1->table_id;

			// PRIORITY TABLE
			if (AND_EQUAL(mode, PS_PRIORITY))
			{
				if ((table_id == 0x00) || (table_id == 0x80))
				{
					bitmap = blank;
					draw_bitmap_direct(
						bitmap,
						destination,
						relative_x,
						relative_y, 0);

					continue;
				}
			}

			table_id = cell1->table_id & 0x04;

			// TABLE 1
			if (table_id == 0x04)
			{
				bitmap = blockset_1[cell1->block_id];
				if (NOT_NULL(bitmap))
				{
					draw_bitmap_direct(
						bitmap,
						destination,
						relative_x,
						relative_y,
						cell1->vflip | cell1->hflip);
				}
			}

			table_id = cell1->table_id & 0xc0;

			// TABLE 2
			if (table_id == 0x80)
			{
				result = table[table2_offset + offset];
				set_stage_cell2(cell2, result);

				bitmap = blockset_2[cell2->block_id];
				if (NOT_NULL(bitmap))
				{
					draw_bitmap_direct(
						bitmap,
						destination,
						(cell2->relative_x * MD_TILE_WIDTH) + 
							relative_x,
						(cell2->relative_y * MD_TILE_HEIGHT) + 
							relative_y,
						0);
				}
			}

			// TABLE 3
			else if (table_id == 0xc0)
			{
				result = table[table2_offset + offset];
				set_stage_cell2(cell2, result);

				for (index = 0;; index++)
				{
					result = table[table3_offset + (index + cell2->table_index)];
					if (result == 0xffff) break;

					set_stage_cell2(cell3, result);

					bitmap = blockset_2[cell3->block_id];
					if (NOT_NULL(bitmap))
					{
						draw_bitmap_direct(
							bitmap,
							destination,
							(cell3->relative_x * MD_TILE_WIDTH) +
							relative_x,
							(cell3->relative_y * MD_TILE_HEIGHT) +
							relative_y,
							0);
					}
				}
			}
		}
	}

	result = draw_bitmap_ecco_indexed(
		row_attributes,
		palette,
		destination,
		destination,
		0, 0, mode);
	if (result == -1) return NULL;

	return destination;
}

t_bitmap *_create_eccojr_stage_map
(
	t_byte *rom,
	t_dword *palette,
	t_dword *palette_row_map,
	t_bitmap **blockset_1,
	t_bitmap **blockset_2,
	int stage_w,
	int stage_h,
	int foreground_offset,
	int stage_container_size,
	int x,
	int y,
	int w,
	int h,
	int mode
)
{
	int index = 0;
	int u = 0, v = 0;
	int row = 0, column = 0;
	int row_offset = 0, column_offset = 0;
	int row_length = 0, column_length = 0;

	int stage_size = 0;
	int table1_offset = 0, table2_offset = 0, table3_offset = 0;

	int palette_row = 0;
	int palette_map_index = 0;
	int palette_index = 0;

	t_plot_state *ps = NULL;

	t_dword *table = NULL;
	t_ecco_stage_cell *cell1 = NULL;
	t_ecco_stage_cell *cell2 = NULL;
	t_ecco_stage_cell *cell3 = NULL;

	t_bitmap *bitmap = NULL;
	t_bitmap *destination = NULL;

	if (AND_EQUAL(stage_container_size, MD_RAM_ADDRESS))
		stage_container_size &= MD_RAM_MASK;

	column_offset = x >> 7;
	row_offset = y >> 7;

	if (w == 0) w = (stage_w * EC_BLOCK_WIDTH) - x;
	if (h == 0) h = (stage_h * EC_BLOCK_HEIGHT) - y;

	x = x - (column_offset << 7);
	y = y - (row_offset << 7);

	column_length = (((w + x) >> 7) + 1) + 1;
	row_length = (((h + y) >> 7) + 1) + 1; // needs to be fixed for level mapping

	stage_size = stage_w * stage_h;

	table2_offset = stage_size * 1;
	table3_offset = stage_size * 2;

	ps = create_plot_state();
	if (ps == NULL) return NULL;

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);
	plot_state_clear_all(ps);

	table = m_array(t_dword, stage_container_size);
	if (table == NULL) return NULL;

	for (index = 0; index < stage_container_size; index++)
		table[index] = unpack_word(rom, foreground_offset + (index * 2));

	destination = create_bitmap(w, h);
	if (destination == NULL) return NULL;

	for (index = 0; index < destination->size; index++)
		destination->container[index] = PS_TRANSPARENTPIXEL;

	cell1 = m_alloc(t_ecco_stage_cell);
	cell2 = m_alloc(t_ecco_stage_cell);
	cell3 = m_alloc(t_ecco_stage_cell);

	for (row = 0; row < row_length; row++)
	{
		if ((row_offset + row) >= stage_h)
			break;

		// palette_index = get_palette_row_index(palette_row_map, row_offset + row);

		for (column = 0; column < column_length; column++)
		{
			if ((column_offset + column) >= stage_w)
				break;

			index = (column_offset + column) + ((row_offset + row) * stage_w);
			set_stage_cell1(cell1, table[index]);

			if (cell1->table_id == 0)
				continue;

			if (NOT_ZERO(cell1->table_id & 0x04))
			{
				bitmap = blockset_1[cell1->block_id];
				if (NOT_NULL(bitmap))
				{
					plot_bitmap_indexed(
						ps,
						palette,
						bitmap,
						destination,
						(column * 128) - x,
						(row * 128) - y,
						palette_index,
						(cell1->vflip | cell1->hflip | mode));
				}
			}

			if (NOT_ZERO(cell1->table_id & 0xf0))
			{
				v = table[table2_offset + index];
				set_stage_cell2(cell2, v);

				if (AND_NEQUAL(cell1->table_id, 0xc0))
				{
					bitmap = blockset_2[cell2->block_id];
					if (NOT_NULL(bitmap))
					{
						plot_bitmap_indexed(
							ps,
							palette,
							bitmap,
							destination,
							((cell2->relative_x * 8) + (column * 128)) - x,
							((cell2->relative_y * 8) + (row * 128)) - y,
							palette_index, mode);
					}
				}

				else if (AND_EQUAL(cell1->table_id, 0xc0))
				{
					for (u = 0;; u++)
					{
						v = table[table3_offset + (u + cell2->table_index)];
						if (v == 0xffff) break;
						set_stage_cell2(cell3, v);

						bitmap = blockset_2[cell3->block_id];
						if (NOT_NULL(bitmap))
						{
							plot_bitmap_indexed(
								ps,
								palette,
								bitmap,
								destination,
								((cell3->relative_x * 8) + (column * 128)) - x,
								((cell3->relative_y * 8) + (row * 128)) - y,
								palette_index, mode);
						}
					}
				}
			}
		}
	}


	plot_state_plot_all(ps);
	release_plot_state(ps);

	return destination;
}

t_bitmap **get_ecco2_stage_tileset(t_byte *file)
{
	t_byte *vram = NULL;
	t_dword *pattern = NULL;
	t_bitmap **tileset = NULL;

	int index = 0;

	vram = m_array(t_byte, MD_VRAM_SIZE);
	if (vram == NULL) return NULL;

	for (index = 0; index < MD_VRAM_SIZE; index++)
		vram[index] = file[MD_STATE_VRAM + index];

	pattern = create_md4_md32_pattern(
		vram, MD_VRAM_SIZE);
	if (pattern == NULL) return NULL;

	free(vram);

	tileset = create_md_tileset(
		pattern, PATTERN_TABLE_SIZE, NULL);
	if (tileset == NULL) return NULL;

	free(pattern);
	return tileset;
}

t_bitmap **ecco2_create_stage_blockset(
	t_byte *ram,
	t_byte *rom,
	t_bitmap **tileset,
	int offset,
	int total,
	int w, int h,
	int mode)
{
	t_dword *nametable_offset = NULL;
	t_md_nametable **md_nametable_map = NULL;
	t_bitmap **blockset = NULL;

	nametable_offset = get_nametable_offsets(rom, offset, total);
	if (nametable_offset == NULL) return NULL;

	md_nametable_map = get_md_nametable_map(ram, rom, nametable_offset, total, w, h);
	if (md_nametable_map == NULL)
	{
		free(nametable_offset);
		return NULL;
	}

	blockset = create_ecco_blockset(
		md_nametable_map, tileset, total, mode);

	if (blockset == NULL)
	{
		release_md_nametable_map(md_nametable_map, total);
		free(nametable_offset);

		return NULL;
	}

	release_md_nametable_map(md_nametable_map, total);
	free(nametable_offset);

	return blockset;
}

int ec2_validate_rom_signature(t_byte *rom)
{
	const char ecco_2_us061994_signature[] = { 0x47,0x4d,0x20,0x4d,0x4b,0x2d,0x31,0x35,0x35,0x33,0x20,0x2d,0x30,0x30,0x4d,0xfe, '\0' };
	const char eccojr_us031995_signature[] = { 0x47,0x4d,0x20,0x4d,0x4b,0x2d,0x31,0x35,0x35,0x34,0x20,0x2d,0x30,0x30,0x33,0x8c, '\0' };

	char rom_signature[17] = { 0 };
	int index = 0;

	for (index = 0; index < 16; index++)
		rom_signature[index] = rom[MD_ROMSIGNATURE_OFFSET + index];

	rom_signature[16] = '\0';

	if (strcmp(rom_signature, ecco_2_us061994_signature) == 0)
		return EC_VERSION_ECCO2;
	else if (strcmp(rom_signature, eccojr_us031995_signature) == 0)
		return EC_VERSION_ECCOJR;

	return -1;
}

t_bitmap *ecco2_extract_stage_map(
	const char *rom_filename,
	int stage_id,
	int x, int y,
	int w, int h,
	int mode)
{
	char *ram_filename = NULL;
	t_byte *ram_file = NULL;

	t_byte *rom = NULL;
	t_byte *ram = NULL;
	t_byte *raw = NULL;

	t_ecco2_stage_properties *properties = NULL;
	t_dword *palette = NULL;
	t_bitmap **tileset = NULL;
	t_bitmap **blockset_1 = NULL;
	t_bitmap **blockset_2 = NULL;

	t_bitmap *stage_map = NULL;
	int size = 0, index = 0;
	int version = 0;

	size = read_file(rom_filename, &rom);
	if (size < 0) return NULL;

	version = ec2_validate_rom_signature(rom);
	if (version == -1) return NULL;

	properties = ecco2_read_stage_properties(rom, stage_id);
	if (properties == NULL) return NULL;

	palette = create_md16_bgra32_palette(
		&rom[properties->palette_offset],
		MD_PALETTE_TOTAL * (properties->palette_total + 1));
	if (palette == NULL) return NULL;

	ram_filename = get_ecco2_stage_save_state(properties->stage_index);
	size = read_file((char *)ram_filename, &ram_file);
	if (size < 0) return NULL;

	// stage decompressor goes here.
	ram = m_array(t_byte, MD_RAM_SIZE);
	if (ram == NULL) return NULL;

	for (index = 0; index < MD_RAM_SIZE; index++)
		ram[index] = ram_file[MD_STATE_68KRAM + index];

	// tile decompressor goes inside get_ecco2_stage_tileset, will take rom as parameter.
	tileset = get_ecco2_stage_tileset(ram_file);
	if (tileset == NULL) return NULL;

	// remove these when decompressors are in place.
	free(ram_file);
	free(ram_filename);

	blockset_1 = ecco2_create_stage_blockset(
		ram, rom,
		tileset,
		properties->blockset1_offset,
		properties->blockset1_total,
		16, 16, 0);

	if (blockset_1 == NULL) return NULL;

	blockset_2 = ecco2_create_stage_blockset(
		ram, rom,
		tileset,
		properties->blockset2_offset,
		properties->blockset2_total,
		0, 0, 0);

	if (blockset_2 == NULL) return NULL;

	if (AND_EQUAL(mode, EC_FOREGROUND))
	{
		if (AND_NEQUAL(mode, PS_TRANSPARENTPIXEL_FILL))
			mode |= PS_TRANSPARENTPIXEL_CLEAR;
	}
	else
	{
		if (AND_EQUAL(mode, PS_PRIORITY))
			mode ^= PS_PRIORITY;
		mode |= PS_TRANSPARENTPIXEL_FILL;
	}

	if (version == EC_VERSION_ECCO2) raw = ram;
		else if (version == EC_VERSION_ECCOJR) raw = rom;
			else return NULL;

	stage_map = ecco2_create_stage_map
	(
		properties,
		palette,
		raw,
		blockset_1,
		blockset_2,
		x,y,
		w,h,
		mode
	);
	
	if (stage_map == NULL) return NULL;

	release_bitmap_array(blockset_1, properties->blockset1_total);
	release_bitmap_array(blockset_2, properties->blockset2_total);
	
	free(palette);
	free(properties);
	free(ram);
	free(rom);

	return stage_map;
}
