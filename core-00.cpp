#include "blue-dream.h"

t_core *create_core(void)
{
	t_core *core = (t_core *)malloc(sizeof(t_core));
	if (core == NULL)
	{
		append_log
		(
			DIAGNOSTICS_LOG,
			get_language_record((const t_unicode **)core->language, LNG_MEM_ALLOC_F)
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
			get_language_record((const t_unicode **)core->language, LNG_MEM_ALLOC_F)
		);

		return NULL;
	}

	return core;
}

void release_core(t_core *core)
{
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

int read_file(const char *filename, int offset, size_t size, void *stream)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) return -1;

	size_t result = fseek(file, offset, SEEK_SET);

	if (feof(file)) return -2;
	if (ferror(file)) return -3;

	if (NOT_ZERO(result)) return -4;

	result = fread(stream, 1, size, file);

	if (feof(file)) return -5;
	if (ferror(file)) return -6;

	if (result != size) return -7; // problem

	fclose(file);
	return 0;
}

// outputRed = (foregroundRed * foregroundAlpha) + (backgroundRed * (1.0 - foregroundAlpha));

// Needs to be simplified, need to filter errors somewhere else.
t_byte *get_rom_palette_resource(t_core *core, const int offset, size_t color_total)
{
	size_t size = MD_16BIT_COLOR * color_total;

	t_byte *md_palette = (t_byte *)malloc(sizeof(t_byte) * size);
	if (md_palette == NULL)
	{
		append_log
		(
			DIAGNOSTICS_LOG,
			get_language_record((const t_unicode **)core->language, LNG_UNKNOWN_E)
		);

		return NULL;
	}

	int result = read_file(
		ECCO_2_US_ROM,
		offset,
		size,
		md_palette);

	if (result < 0)
	{
		t_unicode *log = NULL;
			
		log = copy_string(get_language_record((const t_unicode **)core->language, LNG_UNKNOWN_E));
		log = append_nl_int(log, "read_file returned: ", result);

		append_log
		(
			DIAGNOSTICS_LOG,
			log
		);

		free(log);
		free(md_palette);

		return NULL;
	}

	return md_palette;
}

t_byte *create_palette(t_byte *md_palette, int color_total)
{
	t_byte *palette = (t_byte *)malloc(sizeof(t_byte) * RGBA_32BIT_COLOR * color_total);
	if (palette == NULL) return NULL;

	copy_md16bit_md32bit_palette(&palette[0], &md_palette[0], color_total);
	convert_md32bit_rgba32bit_palette(&palette[0], color_total);
	convert_rgba32bit_bgra32bit_palette(&palette[0], color_total);

	return palette;
}

int core_generate(t_core *core, t_byte *metatile)
{
	int x = 0, y = 0;
	int index = 0;

	int tile_index = metatile[(index * 2) + 1];
	tile_index |= (metatile[(index * 2) + 0] << 8) & 0x07ff;

	int palette_id = 0;
	int vflip = 0;
	int hflip = 0;

	t_plot_state *next = NULL;

	for (y = 0; y < 16; y++)
	{
		for (x = 0; x < 16; x++)
		{
			tile_index = metatile[(index * 2) + 1];
			tile_index |= (metatile[(index * 2) + 0] << 8) & 0x7ff;

			palette_id = (metatile[(index * 2) + 0] >> 5) & 0x3;

			vflip = (metatile[(index * 2) + 0] & 0x10);
			hflip = (metatile[(index * 2) + 0] & 0x8);

			next = plot_state_add_vacant(core->plot_state);
			clone_plot_state(next, core->plot_state);

			set_region(next->source_clip, 0, 0, 8, 8);
			set_pattern_id(next, tile_index);
			set_palette_id(next, palette_id);

			set_region(next->destination_clip, 32 + (x * 8), 32 + (y * 8), 8, 8);

			if (NOT_ZERO(vflip) && NOT_ZERO(hflip))
				next->plot = ps_plot_vhflip;
			else  if (NOT_ZERO(vflip))
				next->plot = ps_plot_vflip;
			else if (NOT_ZERO(hflip))
				next->plot = ps_plot_hflip;
			else
				next->plot = ps_plot_default;

			index++;
		}
	}

	return 0;
}

int core_setup(t_core *core)
{
	int color_total = 64;

	core->viewport = (t_byte *)malloc(sizeof(t_byte) * RGBA_32BIT_COLOR * VIEWPORT_W * VIEWPORT_H);
	if (core->viewport == NULL) return -1;

	int index; for (index = 0; index < 4 * VIEWPORT_W * VIEWPORT_H; index++)
		core->viewport[index] = 0;

	t_byte *md_palette = get_rom_palette_resource(core, SEAOFGREEN_PALETTE1_OFFSET, color_total);
	if (md_palette == NULL) return -1;

	core->palette = create_palette(md_palette, color_total);

	free(md_palette);
	md_palette = NULL;

	if (core->palette == NULL) return -1;

	core->pattern = (t_word *)malloc(sizeof(t_word) * PATTERN_TABLE_SIZE * 2);
	if (core->pattern == NULL) return -1;

	t_byte *md_pattern = (t_byte *)malloc(sizeof(t_byte) * PATTERN_TABLE_SIZE);
	if (md_pattern == NULL) return -1;

	int result = read_file(MD_SAVE_STATE, MD_STATE_VRAM, PATTERN_TABLE_SIZE, md_pattern);
	
	// int result = read_file(ECCO_2_US_ROM, 0x52000, PATTERN_TABLE_SIZE, md_pattern);
	if (result < 0) return -1;

	copy_md4bpp_16bpp_pattern(&core->pattern[0], &md_pattern[0], PATTERN_TABLE_SIZE);

	t_byte *md_metatile = (t_byte *)malloc(sizeof(t_byte) * 256 * 2);

	result = read_file(ECCO_2_US_ROM, SEAOFGREEN_METATILE_OFFSET, 256 * 2, md_metatile);
	if (result < 0) return -1;

	core->plot_state = create_plot_state();
	t_plot_state *ps = core->plot_state;

	for (index = 0; index < 512; index++)
		plot_state_add(ps);

	ps->palette = core->palette;
	ps->source = core->pattern;
	ps->destination = core->viewport;

	ps->palette_w = 16;
	ps->palette_total = 64;

	ps->source_w = 8;
	ps->source_h = 8;
	ps->source_size = PATTERN_TABLE_SIZE * sizeof(t_word);

	ps->destination_w = VIEWPORT_W;
	ps->destination_h = VIEWPORT_H;
	ps->destination_size = VIEWPORT_W * VIEWPORT_H * RGBA_32BIT_COLOR;

	set_region(ps->source_clip, 0, 0, 8, 8);
	set_region(ps->destination_clip, 0, 0, 8, 8);

	set_palette_id(ps, 1);
	ps->plot = ps_plot_default;

	core_generate(core, md_metatile);

	free(md_palette);
	free(md_pattern);
	free(md_metatile);

	return 0;
}

int core_process_palette(t_core *core)
{
	static int v = 0;

	v++;
	if ((v % 20) > 0)
	    return 1;

	int x, y = 0, z = 2;
	int row = 0, column = 0;

	for (x = 0; x < VIEWPORT_W * VIEWPORT_H; x++)
	{
		column = (x % VIEWPORT_W);
		row = (int)ceil(x / VIEWPORT_W);

		y = (int)ceil(column/8);

		core->viewport[(x * 4) + 0] = core->palette[((y % 64) * 4) + 0];
		core->viewport[(x * 4) + 1] = core->palette[((y % 64) * 4) + 1];
		core->viewport[(x * 4) + 2] = core->palette[((y % 64) * 4) + 2];
		core->viewport[(x * 4) + 3] = core->palette[((y % 64) * 4) + 3];
	}

	return 0;
}

int core_process_pattern(t_core *core)
{
	int x = 0, y = 0;
	int index = 0;

	set_palette_id(core->plot_state, 2);

	for (y = 0; y < 16; y++)
	{
		for (x = 0; x < 16; x++)
		{
			set_pattern_id(core->plot_state, x + (y * 16));
			set_region(core->plot_state->source_clip, 0, 0, 8, 8);
			set_region(core->plot_state->destination_clip, 32+(x * 8), 32+(y * 8), 8, 8);

			core->plot_state->plot(core->plot_state);

			index++;
		}
	}

	return 0;
}

int core_process(t_core *core)
{
	t_plot_state *current = core->plot_state;

	while (NOT_NULL(current))
	{
		current->plot(current);
		current = current->next;
	}

	return 0;
}