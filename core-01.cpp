#include "w_interface.h"

t_dword unpack_dword(t_byte *u, t_qword v)
{
	t_dword w, x, y, z;

	w = u[v + 0] << 24;
	x = u[v + 1] << 16;
	y = u[v + 2] << 8;
	z = u[v + 3];

	return w | x | y | z;
}

t_word unpack_word(t_byte *u, t_qword v)
{
	t_word x, y;

	x = u[v + 0] << 8;
	y = u[v + 1];

	return x | y;
}

t_core *create_core(void)
{
	t_core *core = (t_core *)malloc(sizeof(t_core));
	if (core == NULL)
	{
		append_log
		(
			DIAGNOSTICS_LOG,
			get_language_record((const t_unicode **)core->language, LNG_MEMm_alloc_F)
		);

		return NULL;
	}


	// this makes no sense!!!
	core->language = create_language_table();
	if (core->language == NULL)
	{
		free(core);
		core = NULL;

		append_log
		(
			DIAGNOSTICS_LOG,
			get_language_record((const t_unicode **)core->language, LNG_MEMm_alloc_F)
		);

		return NULL;
	}

	core->plot_state = NULL;

	return core;
}
// FIX
void release_core(t_core *core)
{
	return;
	if (core == NULL) return;
	
	if (NOT_NULL(core->language))
	{
		release_language_table(core->language);
		core->language = NULL;
	}

	if (NOT_NULL(core->palette))
	{
		free(core->palette);
		core->palette = NULL;
	}

	if (NOT_NULL(core->pattern))
	{
		free(core->pattern);
		core->pattern = NULL;
	}

	if (NOT_NULL(core->viewport))
	{
		free(core->viewport);
		core->viewport = NULL;
	}

	if (NOT_NULL(core->plot_state))
	{
		release_plot_state(core->plot_state);
		core->plot_state = NULL;
	}

	free(core);
	core = NULL;
}

int read_file(const char *filename, t_byte **stream)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) return -1;

	int result = fseek(file, 0L, SEEK_END);
	if (NOT_ZERO(result))
	{
		fclose(file);
		return -2;
	}

	size_t file_size = ftell(file);
	if (file_size < 0)
	{
		fclose(file);
		return -3;
	}

	*stream = (t_byte *)malloc(sizeof(t_byte) * file_size);
	if (*stream == NULL)
	{
		fclose(file);
		return -4;
	}

	rewind(file);
	size_t total_read = fread(*stream, 1, file_size, file);

	if (ferror(file))
	{
		fclose(file);
		return -5;
	}

	if (NOT_EQUAL(total_read, file_size))
	{
		fclose(file);
		return -6;
	}

	fclose(file);
	return file_size;
}

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

t_dword *create_md16_bgra32_palette(t_byte *md16_palette, int color_total)
{
	t_dword *md32_palette = (t_dword *)malloc(sizeof(t_dword) * color_total);
	if (md32_palette == NULL) return NULL;

	int index = 0, z = 0;
	for (index = 0; index < color_total; index++)
	{
		z = unpack_word(md16_palette, index * 2);
		md32_palette[index] = bswap(z);
	}

	t_dword *palette = (t_dword *)malloc(sizeof(t_dword) * color_total);
	if (palette == NULL) return NULL;

	copy_md16_md32_palette(&md32_palette[0], &palette[0], color_total);
	convert32_md_rgba_palette(&palette[0], color_total);
	convert32_rgba_bgra_palette(&palette[0], color_total);

	free(md32_palette);
	md32_palette = NULL;

	return palette;
}

t_dword *create_md4_md32_pattern(t_byte *md4_pattern, int size)
{
	t_dword *pattern = (t_dword *)malloc(
		sizeof(t_dword) * (size * MD_4BIT_PATTERN));
	if (pattern == NULL) return NULL;

	unpack_md4_md32_pattern((t_byte *)&md4_pattern[0], &pattern[0], size);

	return pattern;
}



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

int core_plot_etd_sprite(t_core *core)
{
	const char *filename = ECCO_US_ROM;

	t_byte *resource = NULL;
	int resource_size = 0;

	t_dword *palette = NULL;
	t_dword *pattern = NULL;
	t_byte *sequence = NULL;
	t_byte *cel = NULL;

	int palette_offset = 0x728e8 + (0x80 * 0x17);
	// int palette_offset = 0x9246a + (0x80 * 4);
	int pattern_offset = 0;
	int sprite_offset = 0x71248;
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

	resource_size = read_file(filename, &resource);
	if (resource_size < 0) return -1;

	sequence = &resource[sprite_offset];

	palette = create_md16_bgra32_palette(
		&resource[palette_offset], 64);

	if (palette == NULL) { result = -2; goto _ERROR; }

	if (pattern_offset == 0)
		pattern_offset = unpack_dword(resource, sprite_offset - 4);

	pattern = create_md4_md32_pattern(
		(t_byte *)&resource[pattern_offset], PATTERN_TABLE_SIZE * 4);

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
			pp->ps = ps;

			core->plot_state = ps;

			plot_etd_sprite(pp, cel);
			plot_state_plot_all(ps);

			core->pp = create_plot_state();
			core->pp->palette = pp->palette;
			core->pp->palette_w = pp->palette_w;
			core->pp->destination = pp->destination;
			core->pp->destination_w = pp->destination_w;
			core->pp->destination_h = pp->destination_h;

			source.resource = pp->destination;
			source.w = pp->destination_w;
			source.h = pp->destination_h;
			source.size = source.w * source.h;

			plot_bitmap_indexed(
				core->plot_state,
				pp->palette,
				&source,
				&source,
				0, 0, PS_TRANSPARENTPIXEL);

			plot_state_plot_all(core->plot_state);

			swprintf_s(_filename, L"DUMP\/%d-%d.png", sequence_index, cel_index);

			SaveD2DBitmap((BYTE *)pp->destination,
				pp->destination_w,
				pp->destination_h,
				(LPCWSTR)_filename);
		}
	}

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

/*
int core_plot_etd_sprite(t_core *core)
{
	const char *filename = ECCO2_US_ROM;

	t_byte *resource = NULL;
	int resource_size = 0;

	t_dword *palette = NULL;
	t_dword *pattern = NULL;
	t_byte *sequence = NULL;
	t_byte *cel = NULL;

	int palette_offset = ECCO2_PALETTE_00;
	int pattern_offset = 0;
	int sprite_offset = ECCO2_SPRITE_SHARK;
	int cel_offset = 0;

	int sequence_index = 0;
	int cel_index = 0;

	int index = 0;
	int result = 0;

	t_plot_process *pp = NULL;
	t_plot_state *ps = NULL;
	
	resource_size = read_file(filename, &resource);
	if (resource_size < 0) return -1;

	sequence = &resource[sprite_offset];

	if (pattern_offset == 0)
		pattern_offset = unpack_dword(resource, sprite_offset - 4);

	if (cel_offset == 0)
		cel_offset = get_cel_offset(sequence, sequence_index, cel_index);

	cel = &sequence[cel_offset];

	palette = create_md16_bgra32_palette(
		&resource[palette_offset], 64);

	if (palette == NULL) { result = -2; goto _ERROR; }

	pattern = create_md4_md32_pattern(
		(t_byte *)&resource[pattern_offset], PATTERN_TABLE_SIZE);

	if (pattern == NULL) { result = -3; goto _ERROR; }

	if (create_viewport(core) < 0) { result = -4; goto _ERROR; }

	ps = create_plot_state();
	if (ps == NULL) { result = -5; goto _ERROR; }

	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);

	plot_state_clear_all(ps);

	pp = create_plot_process();
	if (pp == NULL) { result = -6; goto _ERROR; }

	pp->palette = palette;
	pp->source = pattern;
	pp->ps = ps;

	core->plot_state = ps;

	plot_etd_sprite(pp, cel);
	plot_state_plot_all(ps);

	core->pp = create_plot_state();
	core->pp->palette = pp->palette;
	core->pp->palette_w = pp->palette_w;
	core->pp->destination = pp->destination;
	core->pp->destination_w = pp->destination_w;
	core->pp->destination_h = pp->destination_h;

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
*/
int core_plot_pc_sprite(t_core *core)
{
	const char *zlm_filename = ECCOPC_ZLM_DOLPHIN;
	const char *plz_filename = ECCOPC_PLZ_HOME;

	int index = 0, x = 0, y = 0;
	int size = 0, result = 0;
	int width = 0, height = 0;

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
		cel_index, NULL);

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
	pp->destination_w  = width;
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
	const char *sav_filename = "data/ECCO_US_OCT_1992/JURASSIC_BEACH.gsx";

	int palette_offset = 0x9246a;

	t_bitmap *source;
	wchar_t _filename[256];

	size = read_file(rom_filename, &resource);
	if (size < 0) return -1;

	size = read_file((char *)sav_filename, &ram);
	if (size < 0) return -1;

	u = &resource[palette_offset];
	palette = create_md16_bgra32_palette(u, 64);
	if (palette == NULL) { return -1; }

	u = &ram[MD_STATE_VRAM];
	pattern = create_md4_md32_pattern(u, PATTERN_TABLE_SIZE);
	if (pattern == NULL) { return -1; }

	if (create_viewport(core) < 0) { return -1; }

	ps = create_plot_state();
	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);
	plot_state_clear_all(ps);

	pp = create_plot_state();

	plot_state_set_source(pp, pattern, 8, 8);
	plot_state_set_palette(pp, palette, 16, 64, 0);
	pp->source_size = PATTERN_TABLE_SIZE * 2;

	t_bitmap **mtile_table_1 = etd_create_mtile_table(
		ps,
		pp,
		&ram[MD_STATE_68KRAM],
		resource,
		0xdb88,
		64,
		0);
	/*
	for (index = 0; index < 64; index++)
	{
		if (mtile_table_1[index] == NULL)
			continue;

		source = mtile_table_1[index];
		plot_bitmap_indexed(
			ps,
			palette,
			source,
			source,
			0, 0, PS_TRANSPARENTPIXEL);

		plot_state_plot_all(ps);

		swprintf_s(_filename, L"DUMP\/%d.png", index);

		SaveD2DBitmap((BYTE *)source->resource,
			source->w,
			source->h,
			(LPCWSTR)_filename);

		plot_state_set_destination(pp, source->resource, source->w, source->h);
	}

	core->plot_state = ps;
	core->pp = pp;

	return 0; 
	*/
	etd_generate_level_map
	(
		ps,
		pp,
		&resource[0x93668],
		0x20,
		0x18,
		mtile_table_1
	);

	core->plot_state = ps;
	core->pp = pp;

	return 0;
}

// keep in mind plot_state limits!
int core_ecco2level(t_core *core)
{
	int index = 0;
	wchar_t _filename[256];
	const char *rom_filename = ECCO2_US_ROM;
	
	int size = 0;
	t_byte *u = NULL;

	t_byte *resource = NULL;
	t_byte *ram = NULL;

	t_dword *palette = NULL;
	t_dword *pattern = NULL;

	t_plot_state *pp = NULL;
	t_plot_state *ps = NULL;

	t_ecco2_level_table **ecco2_level = get_ecco2_level_table();
	t_ecco2_level_table *level;

	int level_id = ECCO2_LEVEL_TRELLIASBAY;

	t_unicode *sav_filename = ecco2_level[level_id]->save_state_filename;
	int palette_offset = ecco2_level[level_id]->palette_offset;

	int mtile_total = ecco2_level[level_id]->mtile_total;
	int mtile_custom_total = ecco2_level[level_id]->mtile_custom_total;
	int mtile_offset = ecco2_level[level_id]->mtile_offset;
	int mtile_custom_offset = ecco2_level[level_id]->mtile_custom_offset;

	int level_width = ecco2_level[level_id]->foreground_w;
	int level_height = ecco2_level[level_id]->foreground_h;

	level = ecco2_level[level_id];

	size = read_file(rom_filename, &resource);
	if (size < 0) return -1;

	size = read_file((char *)sav_filename, &ram);
	if (size < 0) return -1;

	u = &resource[palette_offset];
	palette = create_md16_bgra32_palette(u, 64);
	if (palette == NULL) { return -1; }

	u = &ram[MD_STATE_VRAM];
	pattern = create_md4_md32_pattern(u, PATTERN_TABLE_SIZE);
	if (pattern == NULL) { return -1; }

	if (create_viewport(core) < 0) { return -1; }

	ps = create_plot_state();
	for (index = 0; index < PLOT_STATE_TOTAL; index++)
		plot_state_add(ps);
	plot_state_clear_all(ps);

	/*  needs cleaning up */

	t_dword *ptable = get_metatile_ptable(resource, level->mtile_offset, level->mtile_total);
	if (ptable == NULL) return NULL;

	t_md_metatile **md_metatile_1 = create_md_metatile_table(level->mtile_total);
	get_rom_md_metatile_table(md_metatile_1, resource, ptable, level->mtile_total, 16, 16);
	get_ram_md_metatile_table(md_metatile_1, &ram[MD_STATE_68KRAM], ptable, level->mtile_total, 16, 16);

	free(ptable);

	ptable = get_metatile_ptable(resource, level->mtile_custom_offset, level->mtile_custom_total);
	if (ptable == NULL) return NULL;

	dump_memory((t_byte *)ptable, level->mtile_total * 4);

	t_md_metatile **md_metatile_2 = create_md_metatile_table(level->mtile_custom_total);
	get_rom_md_metatile_table(md_metatile_2, resource, ptable, level->mtile_custom_total, 0, 0);
	get_ram_md_metatile_table(md_metatile_2, &ram[MD_STATE_68KRAM], ptable, level->mtile_custom_total, 0, 0);

	/*  needs cleaning up, extract the roms resources first! */

	pp = create_plot_state();

	plot_state_set_source(pp, pattern, 8, 8);
	plot_state_set_palette(pp, palette, 16, 64, 0);
	pp->source_size = PATTERN_TABLE_SIZE * 2;

	t_bitmap **mtile_table_1 = create_mtile_table(
		ps,
		pp,
		md_metatile_1,
		level->mtile_total);

	t_bitmap **mtile_table_2 = create_mtile_table(
		ps,
		pp,
		md_metatile_2,
		level->mtile_custom_total);
	/*
	for (index = 0; index < level->mtile_total; index++)
	{
		if (mtile_table_1[index] == NULL)
			continue;

		plot_bitmap_indexed(
			ps,
			palette,
			mtile_table_1[index],
			mtile_table_1[index],
			0, 0, PS_TRANSPARENTPIXEL);

		plot_state_plot_all(ps);

		swprintf_s(_filename, L"DUMP\/%d.png", index);

		SaveD2DBitmap((BYTE *)mtile_table_1[index]->resource,
			mtile_table_1[index]->w,
			mtile_table_1[index]->h,
			(LPCWSTR)_filename);

		plot_state_set_destination(pp, mtile_table_1[index]->resource, mtile_table_1[index]->w, mtile_table_1[index]->h);
	}

	core->plot_state = ps;
	core->pp = pp;

	return 0;
	*/
	generate_level_map
	(
		ps,
		pp,
		&ram[MD_STATE_68KRAM],
		level_width,
		level_height,
		mtile_table_1,
		mtile_table_2
	);

	core->plot_state = ps;
	core->pp = pp;

	return 0;
}

int core_setup(t_core *core)
{
	// core_plot_pc_sprite(core); // cant use index plot
	// core_plot_etd_sprite(core);
	core_ecco1level(core);
	// core_ecco1level(core);

	return 0;
}

int core_generate(t_core *core)
{
	int index;

	for (index = 0; index < VIEWPORT_W * VIEWPORT_H; index++)
		core->viewport[index] = core->pp->palette[0];

	t_bitmap source, destination;

	source.resource = core->pp->destination;
	source.w = core->pp->destination_w;
	source.h = core->pp->destination_h;
	source.size = source.w * source.h;

	destination.resource = core->viewport;
	destination.w = VIEWPORT_W;
	destination.h = VIEWPORT_H;

	plot_bitmap_indexed(
		core->plot_state,
		core->pp->palette,
		&source,
		&destination,
		0, 0, 0);

	return 0;
}

int core_process(t_core *core)
{
	static int frame = 0;

	frame++;
	if (frame < 1) return 0;
		frame = 0;

	t_plot_state *current = core->plot_state;

	core_generate(core);

	while (NOT_NULL(current))
	{
		if (NOT_NULL(current->plot))
			current->plot(current);

		current = current->next;
	}

	plot_state_clear_all(core->plot_state);

	return 0;
}