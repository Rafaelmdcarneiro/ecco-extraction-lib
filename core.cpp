#include "w_interface.h"


// outputRed = (foregroundRed * foregroundAlpha) + (backgroundRed * (1.0 - foregroundAlpha));

int create_viewport(t_core *core)
{
	core->viewport = (t_dword *)malloc(
		sizeof(t_dword) * VIEWPORT_W * VIEWPORT_H);
	if (core->viewport == NULL) return -1;

	int index = 0;
	for (index = 0; index < VIEWPORT_W * VIEWPORT_H; index++)
		core->viewport[index] = 0;

	return 0;
}

// for sprites

t_dword get_cel_offset(
	t_byte *table,
	t_word sequence_index,
	t_word cel_index)
{
	int sequence_total = 0;
	int sequence_offset = 0;

	int cel_total = 0;
	int cel_offset = 0;

	sequence_total = unpack_word(table, 0);

	if (sequence_index >= sequence_total)
		sequence_index = 0;

	sequence_offset = unpack_word(table, 
		(2 + (sequence_index * 2)));

	cel_total = unpack_word(table,
		(sequence_offset + 0));

	if (cel_index >= cel_total)
		cel_index = 0;

	cel_offset = unpack_word(table, 
		(sequence_offset + 2) + (cel_index * 2));

	return cel_offset;
}

t_dword get_sequence_total(
	t_byte *table)
{
	return unpack_word(table, 0);
}

t_dword get_cel_total(
	t_byte *table,
	t_word sequence_index)
{
	int sequence_total = 0;
	int sequence_offset = 0;

	int cel_total = 0;
	int cel_offset = 0;

	sequence_total = unpack_word(table, 0);

	if (sequence_index >= sequence_total)
		sequence_index = 0;

	sequence_offset = unpack_word(table,
		(2 + (sequence_index * 2)));

	cel_total = unpack_word(table,
		(sequence_offset + 0));

	return cel_total;
}

int core_plot_ecco_sprite(t_core *core)
{
	const char *filename = ECCO_US_ROM;

	t_byte *resource = NULL;
	int resource_size = 0;

	t_dword *palette = NULL;
	t_dword *pattern = NULL;
	t_byte *sequence = NULL;
	t_byte *cel = NULL;

	int palette_offset = get_ecco_world_property(4, 0) + (0x80 * 0x0) + 0x00;
	int pattern_offset = 0;
	int sprite_offset = 0x6c728;
	int cel_offset = 0;

	int sequence_index = 0;
	int cel_index = 0;

	int index = 0;
	int result = 0;

	t_bitmap source;
	wchar_t _filename[256];

	int sequence_total = 0;
	int cel_total = 0;

	t_plot_process *pp = NULL;
	t_plot_state *ps = NULL;

	t_bitmap *destination = NULL;

	resource_size = read_file(filename, &resource);
	if (resource_size < 0) return -1;

	sequence = &resource[sprite_offset];

	palette = create_md16_bgra32_palette(
		&resource[palette_offset], 64);

	if (palette == NULL) { result = -2; goto _ERROR; }

	if (pattern_offset == 0)
		pattern_offset = unpack_dword(resource, sprite_offset - 4);

	pattern = create_md4_md32_pattern(
		(t_byte *)&resource[pattern_offset], MD_PATTERN_TABLE_SIZE * 4);

	if (pattern == NULL) { result = -3; goto _ERROR; }

	if (create_viewport(core) < 0) { result = -4; goto _ERROR; }

	ps = create_plot_state();
	if (ps == NULL) { result = -5; goto _ERROR; }

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);

	plot_state_clear_all(ps);

	pp = create_plot_process();
	if (pp == NULL) { result = -6; goto _ERROR; }

	sequence_total = get_sequence_total(sequence);
	core->pp = create_plot_state();

	for (sequence_index = 0; sequence_index < sequence_total; sequence_index++)
	{
		cel_total = get_cel_total(sequence, sequence_index);
		for (cel_index = 0; cel_index < cel_total; cel_index++)
		{
			// if (cel_offset == 0)
			cel_offset = get_cel_offset(sequence, sequence_index, cel_index);

			cel = &sequence[cel_offset];

			pp->palette = palette;
			pp->source = pattern;
			pp->destination = NULL;
			pp->ps = ps;

			core->plot_state = ps;

			destination = plot_ecco_global_sprite(ps, pattern, cel);

			plot_bitmap_indexed(
				ps,
				palette,
				destination,
				destination,
				0, 0, 0, PS_TRANSPARENTPIXEL_IGNORE);

			plot_state_plot_all(core->plot_state);

			swprintf_s(_filename, L"DUMP\/%d-%d.png", sequence_index, cel_index);

			SaveD2DBitmap((BYTE *)destination->container,
				destination->w,
				destination->h,
				(LPCWSTR)_filename);
		}
	}
	// needs to be cleaned up to get rid of destination
	core->pp->palette = pp->palette;
	core->pp->palette_w = pp->palette_w;
	core->pp->destination = destination->container;
	core->pp->destination_w = destination->w;
	core->pp->destination_h = destination->h;

	return 0;

_ERROR:

	if (NOT_NULL(ps)) release_plot_state(ps);
	if (NOT_NULL(resource)) free(resource);
	if (NOT_NULL(palette)) free(palette);
	if (NOT_NULL(pattern)) free(pattern);

	resource = NULL;
	palette = NULL;
	pattern = NULL;

	return result;
}

int core_plot_etd_stagesprite(t_core *core)
{
	const char *filename = ECCO_US_ROM;

	t_byte *resource = NULL;
	t_byte *ram = NULL;
	int resource_size = 0;

	const char *sav_filename = "data/ECCO_US_OCT_1992/HOMEBAY.gsx";
	wchar_t _filename[256];

	t_dword *palette = NULL;
	t_dword *pattern = NULL;
	t_byte *sequence = NULL;
	t_byte *cel = NULL;

	int palette_offset = get_ecco_world_property(4, EC_PALETTE_OFFSET) + (0x80 * 0x2);

	int pattern_offset = 0;
	// int sprite_offset = 0x1b407a + (4 * 6); // queen
	// int sprite_offset = 0x155eaa; // medusa
	int sprite_offset = 0x738e8;
	int cel_offset = 0;

	int sequence_index = 0;
	int cel_index = 0;

	int index = 0;
	int total = 0;
	int result = 0;

	t_bitmap *destination = NULL;
	t_plot_state *pp = NULL;
	t_plot_state *ps = NULL;
	
	resource_size = read_file(filename, &resource);
	if (resource_size < 0) return -1;

	resource_size = read_file(sav_filename, &ram);
	if (resource_size < 0) return -1;

	palette = create_md16_bgra32_palette(
		&resource[palette_offset], 64);

	if (palette == NULL) { result = -2; goto _ERROR; }

	pattern = create_md4_md32_pattern(
		(t_byte *)&ram[MD_STATE_VRAM], MD_PATTERN_TABLE_SIZE);

	if (pattern == NULL) { result = -3; goto _ERROR; }

	if (create_viewport(core) < 0) { result = -4; goto _ERROR; }

	ps = create_plot_state();
	if (ps == NULL) { result = -5; goto _ERROR; }

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);

	plot_state_clear_all(ps);

	core->plot_state = ps;
	
	total = unpack_word(resource, sprite_offset);

	for (index = 0; index < total; index++)
	{
		cel = &resource[(sprite_offset + 2) + (index * ECCO_SPRITE_CEL_SIZE)];

		destination = plot_ecco_local_sprite(ps, pattern, cel);
		if (destination == NULL) return -6;

		plot_bitmap_indexed(
			core->plot_state,
			palette,
			destination,
			destination,
			0, 0, 0, PS_TRANSPARENTPIXEL_IGNORE);

		plot_state_plot_all(core->plot_state);

		swprintf_s(_filename, L"DUMP\/%d.png", index);

		SaveD2DBitmap((BYTE *)destination->container,
			destination->w,
			destination->h,
			(LPCWSTR)_filename);
	}

	core->pp = create_plot_state();
	core->pp->palette = palette;
	core->pp->palette_w = 64;
	core->pp->destination = destination->container;
	core->pp->destination_w = destination->w;
	core->pp->destination_h = destination->h;

	return 0;

_ERROR:

	if (NOT_NULL(ps)) release_plot_state(ps);
	if (NOT_NULL(resource)) free(resource);
	if (NOT_NULL(palette)) free(palette); 
	if (NOT_NULL(pattern)) free(pattern);

	resource = NULL;
	palette = NULL;
	pattern = NULL;

	return result;
}

int _core_plot_pc_sprite(t_core *core)
{
	const char *zlm_filename = "LEOSHAR2.ZLM";
	const char *plz_filename = "ICE01.PLZ";

	int index = 0, x = 0, y = 0;
	int size = 0, result = 0;
	int width = 0, height = 0;
	int sequence_total = 0;

	t_dword palette_offset = ECCOPC_PALETTE_00;

	t_byte *u = NULL;
	t_byte *zlm_resource = NULL;
	t_byte *plz_resource = NULL;

	t_zlm_sprite_cel *cel = NULL;

	t_dword *palette = NULL;
	t_dword *destination = NULL;

	t_plot_process *pp = NULL;
	t_plot_state *ps = NULL;

	int sequence_index = 0;
	int cel_index = 1;

	size = read_file(zlm_filename, &zlm_resource);
	if (size < 0) return -1;

	size = read_file(plz_filename, &plz_resource);
	if (size < 0) return -2;

	u = &plz_resource[palette_offset];
	palette = plz_palette_create_resource(u);

	if (palette == NULL) { result = -3; goto _ERROR; }

	cel = zlm_cel_create_resource(
		zlm_resource, sequence_index,
		cel_index, &sequence_total, NULL, NULL);

	if (cel == NULL) { result = -4; goto _ERROR; }

	size = cel->width * cel->height;
	destination = (t_dword *)malloc(sizeof(t_dword) * size);
	if (destination == NULL) { result = -7; goto _ERROR; }

	for (index = 0; index < size; index++)
		destination[index] = 0;

	if (create_viewport(core) < 0) { result = -7; goto _ERROR; }

	pp = create_plot_process();
	if (pp == NULL) { result = -6; goto _ERROR; }

	ps = create_plot_state();

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);

	plot_state_clear_all(ps);

	width = cel->width;
	height = cel->height;

	pp->palette = palette;
	pp->destination = destination;
	pp->destination_w = width;
	pp->destination_h = height;
	pp->destination_size = width * height;
	pp->ps = ps;

	zlm_plot_sprite_bitmap(pp, cel, zlm_resource);
	zlm_plot_sprite_mask(pp, cel, zlm_resource);

	core->plot_process = pp;
	core->plot_state = ps;

	plot_state_plot_all(ps);

	core->pp = create_plot_state();
	core->pp->palette = pp->palette;
	core->pp->palette_w = pp->palette_w;
	core->pp->destination = pp->destination;
	core->pp->destination_w = pp->destination_w;
	core->pp->destination_h = pp->destination_h;

	return 0;

_ERROR:

	if (NOT_NULL(zlm_resource)) free(zlm_resource);

	zlm_resource = NULL;
	return -1;
}
// laci tunnel 
int core_plot_pc_sprite(t_core *core)
{
	const char *zlm_filename = "SONARX.ZLM";
	const char *plz_filename = "PRE.PLZ";

	int index = 0, x = 0, y = 0;
	int size = 0, result = 0;
	int width = 0, height = 0;
	int sequence_total = 0;
	int cel_total = 0;

	int id = 2;
	t_dword palette_offset = ECCOPC_PALETTE_00 + (id * 0);

	t_byte *u = NULL;
	t_byte *zlm_resource = NULL;
	t_byte *plz_resource = NULL;

	t_zlm_sprite_cel *cel = NULL;

	t_dword *palette = NULL;
	t_dword *destination = NULL;
	wchar_t _filename[256];

	t_plot_process *pp = NULL;
	t_plot_state *ps = NULL;

	int sequence_index = 0;
	int cel_index = 0;

	size = read_file(zlm_filename, &zlm_resource);
	if (size < 0) return -1;

	size = read_file(plz_filename, &plz_resource);
	if (size < 0) return -2;

	u = &plz_resource[palette_offset];
	palette = plz_palette_create_resource(u);

	if (palette == NULL) { result = -3; goto _ERROR; }

	cel = zlm_cel_create_resource(
		zlm_resource, sequence_index,
		cel_index, &sequence_total, &cel_total, NULL);

	if (cel == NULL) { result = -4; goto _ERROR; }
	free(cel);

	if (create_viewport(core) < 0) { result = -7; goto _ERROR; }

	pp = create_plot_process();
	if (pp == NULL) { result = -6; goto _ERROR; }

	ps = create_plot_state();

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);

	plot_state_clear_all(ps);

	for (sequence_index = 0; sequence_index < sequence_total; sequence_index++)
	{
		cel = zlm_cel_create_resource(
			zlm_resource, sequence_index,
			0, NULL, &cel_total, NULL);
		free(cel);

		for (cel_index = 0; cel_index < cel_total; cel_index++)
		{
			cel = zlm_cel_create_resource(
				zlm_resource, sequence_index,
				cel_index, NULL, NULL, NULL);
			
			if (cel->height == 0)
				cel = zlm_cel_create_resource(
					zlm_resource, cel->mask_column_size,
					cel->width, NULL, NULL, NULL);
					
			size = cel->width * cel->height;
			destination = (t_dword *)malloc(sizeof(t_dword) * size);
			if (destination == NULL) { result = -7; goto _ERROR; }

			for (index = 0; index < size; index++)
				destination[index] = 0;

			width = cel->width;
			height = cel->height;

			pp->palette = palette;
			pp->destination = destination;
			pp->destination_w = width;
			pp->destination_h = height;
			pp->destination_size = width * height;
			pp->ps = ps;

			zlm_plot_sprite_bitmap(pp, cel, zlm_resource);
			zlm_plot_sprite_mask(pp, cel, zlm_resource);

			core->plot_process = pp;
			core->plot_state = ps;

			plot_state_plot_all(ps);

			swprintf_s(_filename, L"DUMP\/%d-%d.png", sequence_index, cel_index);

			SaveD2DBitmap((BYTE *)destination,
				width,
				height,
				(LPCWSTR)_filename);
		}
	}

	core->pp = create_plot_state();
	core->pp->palette = pp->palette;
	core->pp->palette_w = pp->palette_w;
	core->pp->destination = pp->destination;
	core->pp->destination_w = pp->destination_w;
	core->pp->destination_h = pp->destination_h;

	return 0;

_ERROR:

	if (NOT_NULL(zlm_resource)) free(zlm_resource);

	zlm_resource = NULL;
	return -1;
}

void dump_bitmap_table(
	t_plot_state *ps,
	t_dword *palette,
	t_bitmap **table,
	int total,
	int mode)
{
	int index = 0;
	wchar_t filename[256];

	for (index = 0; index < total; index++)
	{
		if (NOT_NULL(table[index]))
		{
			plot_bitmap_indexed(
				ps,
				palette,
				table[index],
				table[index],
				0, 0, 0, mode);

			plot_state_plot_all(ps);

			swprintf_s(filename, L"%s\/%d.png", DUMP_DIR, index);
			SaveD2DBitmap((BYTE *)table[index]->container,
				table[index]->w,
				table[index]->h,
				(LPCWSTR)filename);
		}
	}
}

void _dump_bitmap_table(
	t_plot_state *ps,
	t_dword *palette,
	t_bitmap **table,
	int total,
	int mode)
{
	int index = 0;
	wchar_t filename[256];

	for (index = 0; index < total; index++)
	{
		if (NOT_NULL(table[index]))
		{
			plot_bitmap_indexed(
				ps,
				palette,
				table[index],
				table[index],
				0, 0, 0, mode);

			plot_state_plot_all(ps);

			swprintf_s(filename, L"%s\/%d.png", DUMP_DIR2, index);
			SaveD2DBitmap((BYTE *)table[index]->container,
				table[index]->w,
				table[index]->h,
				(LPCWSTR)filename);
		}
	}
}

int core_ecco1level(t_core *core)
{
	int index = 0;
	int size = 0;

	t_byte *u = NULL;

	t_byte *resource = NULL;
	t_byte *ram = NULL;

	t_dword *palette = NULL;
	t_dword *pattern = NULL;

	t_plot_state *pp = NULL;
	t_plot_state *ps = NULL;

	const char *rom_filename = ECCO_US_ROM;
	
	t_bitmap source;
	wchar_t _filename[256];

	int mode = PS_TRANSPARENTPIXEL_IGNORE;
	int stage_id = 9;

	int palette_id = (64 * 0x0);
	int foreground = 1;

	int nametable_w = 0;
	int nametable_h = 0;
	int nametable_offset = 0;
	int block_offset = 0;

	t_dword *palette_data = m_array(t_dword, 64);

	int nametable_attribute_offset = get_ecco_stage_property(
		stage_id, EC_NAMETABLEATTRIBUTE_OFFSET);

	int world_id = get_ecco_stage_property(stage_id, EC_WORLD);

	// Palette
	int palette_offset = get_ecco_world_property(
		world_id, EC_PALETTE_OFFSET) + (0x80 * 0x0);

	char *sav_filename = get_ecco_stage_save_state(stage_id);

	size = read_file(rom_filename, &resource);
	if (size < 0) return -1;
	
	size = read_file(sav_filename, &ram);
	if (size < 0) return -1;

	int foreground_w = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_FOREGROUND_W
	);

	int foreground_h = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_FOREGROUND_H
	);

	int background_w = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_BACKGROUND_W
	);

	int background_h = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_BACKGROUND_H
	);

	int fg_nametable_offset = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_FG_NAMETABLE_OFFSET
	);

	int bg_nametable_offset = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_BG_NAMETABLE_OFFSET
	);

	int palette_row_atribute_offset = get_ecco_nametable_attribute(
		resource,
		nametable_attribute_offset,
		EC_PALETTEROWATTRIBUTE_OFFSET
	);

	if (foreground == 1)
	{
		block_offset = 0xdb88;
		nametable_offset = fg_nametable_offset;

		nametable_w = foreground_w;
		nametable_h = foreground_h;
	}
	else
	{
		block_offset = 0xdcac;
		nametable_offset = bg_nametable_offset;

		nametable_w = background_w;
		nametable_h = background_h;
	}

	u = &resource[palette_offset];
	palette = create_md16_bgra32_palette(u, 0x800); // need a large sample
	if (palette == NULL) { return -1; }

	u = &ram[MD_STATE_VRAM];
	pattern = create_md4_md32_pattern(u, MD_PATTERN_TABLE_SIZE);
	if (pattern == NULL) { return -1; }

	if (create_viewport(core) < 0) { return -1; }

	palette_data[0] = resource[palette_row_atribute_offset + 0];
	palette_data[1] = 1;

	palette_data[2] = resource[palette_row_atribute_offset + 1];
	palette_data[3] = 2;

	palette_data[4] = resource[palette_row_atribute_offset + 2];
	palette_data[5] = 3;

	palette_data[6] = palette_data[4];
	palette_data[7] = 0;

	ps = create_plot_state();
	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);
	plot_state_clear_all(ps);

	pp = create_plot_state();

	pp->mode = PS_TRANSPARENTPIXEL_IGNORE;

	plot_state_set_source(pp, pattern, 8, 8);
	plot_state_set_palette(pp, palette, 16, 64, 0);
	pp->source_size = MD_PATTERN_TABLE_SIZE * 2;

	t_bitmap **mtile_table_1 = etd_create_mtile_table(
		ps,
		pp,
		&ram[MD_STATE_68KRAM],
		resource,
		block_offset,
		64,
		0);
	/*
	dump_bitmap_table(
		ps,
		&palette[palette_id], // &palette[0 * resource[palette_row_atribute_offset]],
		mtile_table_1,
		64, mode);
		*/
	core->plot_state = ps;
	core->pp = pp;

	// return 0;

	etd_generate_level_map
	(
		ps,
		pp,
		&resource[nametable_offset],
		nametable_w,
		nametable_h,
		mtile_table_1
	);

	source.container = core->pp->destination;
	source.w = core->pp->destination_w;
	source.h = core->pp->destination_h;
	source.size = source.w * source.h;

	_plot_bitmap_direct(
		core->plot_state,
		palette_data,
		&source,
		0, 0, 0);

	plot_state_plot_all(core->plot_state);

	plot_bitmap_indexed(
		core->plot_state,
		core->pp->palette,
		&source,
		&source,
		0, 0, 0, mode);

	plot_state_plot_all(core->plot_state);

	SaveD2DBitmap((BYTE *)core->pp->destination,
		core->pp->destination_w,
		core->pp->destination_h,
		L"output.png");

	core->plot_state = ps;
	core->pp = pp;

	return 0;
}

/*

// used for dumping blocks in ecco pc
void dump_direct_bitmap_table(
	t_plot_state *ps,
	t_bitmap **table,
	int total,
	int mode)
{
	int index = 0;
	wchar_t filename[256];

	for (index = 0; index < total; index++)
	{
		if (NOT_NULL(table[index]))
		{
			plot_bitmap_direct(
				ps,
				table[index],
				table[index],
				0, 0, mode);

			plot_state_plot_all(ps);

			swprintf_s(filename, L"%s\/%d.png", DUMP_DIR, index);
			SaveD2DBitmap((BYTE *)table[index]->container,
				table[index]->w,
				table[index]->h,
				(LPCWSTR)filename);
		}
	}
}
*/

int core_setup(t_core *core)
{
	// core_plot_pc_sprite(core); // cant use index plot
	// core_plot_ecco_sprite(core);
	// core_plot_etd_stagesprite(core);
	// core_ecco2level(core);
	// core_ecco1level(core);

	return 0;
}
