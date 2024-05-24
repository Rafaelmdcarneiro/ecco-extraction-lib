#pragma once

#ifndef _MAIN_HEADER
	#include "blue-dream.h"
#endif

#define EC_WORLD_HOMEBAY              0
#define EC_WORLD_HARDWATER            1
#define EC_WORLD_CITYOFFOREVER        2
#define EC_WORLD_JURASSICBEACH        3
#define EC_WORLD_THEMACHINE           4

#define EC_WORLD_TOTAL                (EC_WORLD_THEMACHINE + 1)
#define EC_PALETTE_OFFSET             (EC_WORLD_TOTAL * 0)

#define EC_STAGE_TOTAL                28

#define EC_WORLD                      (EC_STAGE_TOTAL * 0)
#define EC_NAMETABLEATTRIBUTE_OFFSET  (EC_STAGE_TOTAL * 1)

#define EC_FOREGROUND_W               0
#define EC_FOREGROUND_H               1
#define EC_FG_NAMETABLE_OFFSET        2
#define EC_BACKGROUND_W               3
#define EC_BACKGROUND_H               4
#define EC_PALETTEROWATTRIBUTE_OFFSET 5
#define EC_BG_NAMETABLE_OFFSET        6

#define EC_NAMETABLE_ATTRIBUTE_TOTAL  EC_BG_NAMETABLE_OFFSET + 1

typedef struct t_etd_level_cell
{
	int hflip;
	int vflip;
	int mtile_id;

} t_etd_level_cell;

t_dword get_ecco_world_property(int id, int type);
t_dword get_ecco_stage_property(int id, int type);
char *get_ecco_stage_save_state(int id);

t_dword get_ecco_nametable_attribute(
	t_byte *rom, int offset, int id);

void set_etd_level_cell(t_etd_level_cell *x, int y);

t_bitmap **etd_create_mtile_table(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *ram,
	t_byte *resource,
	t_dword offset,
	int total,
	int flag
);

int etd_generate_level_map
(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *sav_resource,
	int level_w,
	int level_h,
	t_bitmap **mtile_table_1
);