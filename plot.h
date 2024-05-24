#ifndef _MAIN_HEADER
    #include "blue-dream.h"
#endif

#define MD_4BIT_PATTERN 2
#define RGBA_32BIT_COLOR 4

#define PLOT_STATE_TOTAL (4096 * 3)
#define PS_STATUS_VACANT 0
#define PS_STATUS_OCCUPIED 1

// lower 8 bits reserved
#define PS_TRANSPARENTPIXEL_IGNORE 0
#define PS_TRANSPARENTPIXEL_CLEAR (0x1 << 8)
#define PS_TRANSPARENTPIXEL_FILL (0x1 << 9)
#define PS_PRIORITY (0x1 << 10)

// all in the dword boundry passed the word boundry
#define PS_VFLIP (0x1 << 16)
#define PS_HFLIP (0x1 << 17)
#define PS_SHADOW (0x1 << 18)
#define PS_HIGHLIGHT (0x1 << 19)
#define PS_TRANSPARENTPIXEL (0x1 << 20)


#define PS_VHFLIP (PS_VFLIP | PS_HFLIP)

#define PS_MASK_COLORINDEX 0x0000ffff
#define PS_MASK_MODE 0xffff0000
#define PS_MASK_FLIP PS_VHFLIP
#define PS_MASK_PRIORITY (PS_SHADOW | PS_HIGHLIGHT)

#define PS_TRANSPARENTPIXEL_SHADOW ((0xff000000 >> 1) & 0xff000000);

typedef struct t_region
{
    int x, y, w, h;
} t_region;

typedef struct t_plot_state
{
    int id;
    int class_id; // Determins type of plot state (preprocessing, post, post with pre).
    int status_id; // Used to determine is a plot state is active or inactive.
    int mode;

    void *parameters;
    t_dword *palette;
    t_dword *source;
    t_dword *destination;

    int parameters_size;
    int palette_id;
    int palette_index;
    int palette_w;
    int palette_total;

    int source_id;
    int source_index;
    int source_w;
    int source_h;
    int source_size;

    int destination_w;
    int destination_h;
    int destination_size;

    t_region *source_clip;
    t_region *destination_clip;

    int (*input)(t_plot_state *, int, int);
    int (*output)(t_plot_state *, int, int);

    int (*plot)(t_plot_state *);

    t_plot_state *next;
    t_plot_state *_next;

} t_plot_state;

typedef struct t_plot_process
{
    t_dword *palette;
    t_dword *source;
    t_dword *destination;

    int palette_id;
    int palette_w;
    int palette_total;

    int source_w;
    int source_h;
    int source_size;

    int destination_w;
    int destination_h;
    int destination_size;

    t_plot_state *ps;
} t_plot_process;

t_region *create_region(int x, int y, int w, int h);
void set_region(t_region *region, int x, int y, int w, int h);
void clone_region(t_region *x, t_region *y);

void set_horizontal_clip(
    int x,
    int source_w,
    int destination_w,
    t_region *u,
    t_region *v);

void set_vertical_clip(
    int y,
    int source_h,
    int destination_h,
    t_region *u,
    t_region *v);

void update_clipping_regions(t_plot_state *ps, int x, int y);

t_plot_process *create_plot_process(void);
void plot_process_set_source(
    t_plot_process *pp,
    t_dword *source,
    int w,
    int h);

void plot_process_set_destination(
    t_plot_process *pp,
    t_dword *destination,
    int w,
    int h);

t_plot_state *create_plot_state(void);
void release_plot_state(t_plot_state *plot_state);

void clear_plot_state(t_plot_state *ps);
void clone_plot_state(t_plot_state *x, t_plot_state *y);

void plot_state_set_palette(
    t_plot_state *ps,
    t_dword *palette,
    int w,
    int total,
    int id);

void plot_state_set_source(t_plot_state *ps, t_dword *source, int w, int h);
void plot_state_set_destination(t_plot_state *ps, t_dword *destination, int w, int h);

void plot_state_set_source_region(
    t_plot_state *ps,
    int x, int y, int w, int h,
    int pattern_id);

t_plot_state *plot_state_add(t_plot_state *plot_state);
t_plot_state *plot_state_get_vacant(t_plot_state *plot_state);
t_plot_state *plot_state_add_vacant(t_plot_state *plot_state);
t_plot_state *plot_state_push_vacant(t_plot_state *plot_state);
void plot_state_pop_vacant(t_plot_state *plot_state);
void plot_state_clear_all(t_plot_state *plot_state);

void set_palette_id(t_plot_state *ps, int id);
int get_palette_color_index(t_plot_state *ps, int color_id);
void set_pattern_id(t_plot_state *ps, int id);
void plot_state_set_flip(t_plot_state *ps, int flag);

int ps_priority_shadow(int color);

int ps_input_default(t_plot_state *ps, int x, int y);
int ps_input_hflip(t_plot_state *ps, int x, int y);
int ps_input_vflip(t_plot_state *ps, int x, int y);
int ps_input_vhflip(t_plot_state *ps, int x, int y);
int ps_output_default(t_plot_state *ps, int x, int y);

int ps_plot_default_pixel(t_plot_state *ps, int x, int y);
int ps_plot_default(t_plot_state *plot_state);
int ps_plot_direct(t_plot_state *plot_state);
int ps_plot_zlm(t_plot_state *ps);
int ps_plot_mask(t_plot_state *ps);
int ps_plot_md_indexed(t_plot_state *ps);


int plot_state_plot_all(t_plot_state *ps);

int ps_plot_update_palette(t_plot_state *ps);