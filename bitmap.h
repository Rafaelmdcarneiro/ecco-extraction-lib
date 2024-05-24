#include "blue-dream.h"

typedef struct t_bitmap
{
    int w;
    int h;
    int size;

    t_dword *container;
} t_bitmap;

t_bitmap *create_bitmap(int w, int h);
void release_bitmap(t_bitmap *bitmap);

void release_bitmap_array(t_bitmap **bitmap_array, int bitmap_total);
void set_background_color(t_bitmap *bitmap, int color);

void ps_set_destination_bitmap(
    t_plot_state *ps,
    t_bitmap *destination);

void ps_set_source_bitmap(
    t_plot_state *ps,
    t_bitmap *source);

/* begin added from plot.h */

int plot_bitmap_indexed(
    t_plot_state *ps,
    t_dword *palette,
    t_bitmap *source,
    t_bitmap *destination,
    int x, int y,
    int palette_id,
    int mode);

int plot_bitmap_direct(
    t_plot_state *ps,
    t_bitmap *source,
    t_bitmap *destination,
    int x, int y,
    int mode);

int _plot_bitmap_direct(
    t_plot_state *ps,
    t_dword *source,
    t_bitmap *destination,
    int x, int y,
    int mode);

/* end added from plot.h */

t_bitmap *create_tile(
	t_dword *pattern,
	int w,
	int h,
	int size,
	int pattern_id);

t_bitmap **create_tileset(
    t_dword *pattern,
    int w, int h,
    int size,
    int *_total);

int draw_bitmap_indexed(
	t_dword *palette,
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int palette_index,
	int mode);

int draw_bitmap_direct(
	t_bitmap *source,
	t_bitmap *destination,
	int x, int y,
	int mode);