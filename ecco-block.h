#ifndef _MAIN_HEADER
    #include "blue-dream.h"
#endif

#define EC_BLOCK_WIDTH 16
#define EC_BLOCK_HEIGHT 16
#define EC_BLOCK_PIXEL_W (MD_TILE_WIDTH * EC_BLOCK_WIDTH)
#define EC_BLOCK_PIXEL_H (MD_TILE_HEIGHT * EC_BLOCK_HEIGHT)

typedef struct t_md_pattern_name
{
	int priority;
	int pattern_id;
	int vflip;
	int hflip;
	int palette_id;
} t_md_pattern_name;

typedef struct t_md_nametable
{
	int w;
	int h;
	t_byte *container;
} t_md_nametable;

int plot_md_pattern_name(
	t_plot_state *ps,
	t_bitmap *source,
	t_bitmap *destination,
	t_md_pattern_name *pattern_name,
	int x, int y);

/* md_nametable_pattern_name_table */

void md_pattern_name_set(
	t_md_pattern_name *pn,
	t_byte *container,
	int index);

t_dword *get_nametable_offsets(t_byte *resource, int offset, int total);

void release_md_nametable(t_md_nametable *md_nametable);

t_md_nametable *get_md_nametable(
	t_byte *raw,
	int offset,
	int w,
	int h
);

t_md_nametable **create_md_nametable_map(int total);

void release_md_nametable_map(t_md_nametable **table, int total);

t_md_nametable **get_md_nametable_map(
	t_byte *ram,
	t_byte *rom,
	t_dword *namtable_offsets,
	int total,
	int w,
	int h
);

t_bitmap *create_md_tile(
	t_dword *pattern,
	int size,
	int pattern_id);

t_bitmap **create_md_tileset(t_dword *pattern, int size, int *_total);

t_bitmap *create_ecco_block(
	t_bitmap **tileset,
	t_byte *container,
	int w, int h, int mode);

int get_nametable_total(t_byte *resource, int offset);

t_bitmap **create_ecco_blockset(
	t_md_nametable **md_nametable,
	t_bitmap **tileset,
	int total, int mode
);

// depricated_ecco_1_stage
void plot_etd_tile(
	t_plot_state *ps,
	t_plot_state *pp,
	t_md_pattern_name *pn,
	int x, int y);

void plot_etd_block(
	t_plot_state *ps,
	t_plot_state *pp,
	t_byte *resource,
	int position_x, int position_y,
	int w, int h);