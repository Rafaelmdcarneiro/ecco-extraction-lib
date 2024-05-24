#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/* Ecco 2 Addresses
 *
 * 0x11e4 - Loads Palettes
 * 0010b0de - Selection Scr Palette 
 * 0010b0fe
 * 0010b11e
 * 0010b13e
 * 0x2c000 - Dolphin sprite location
 */

/* common */

#define NOT_EQUAL(x, y) ((x) != (y))
#define NOT_NULL(x) ((x) != NULL)
#define NOT_ZERO(x) ((x) != 0)

#define PROGRAM_NAME L"Project Blue Dream"
#define PROGRAM_VERSION L"00.05162020"

#define VIEWPORT_W 426
#define VIEWPORT_H 240

#define DIAGNOSTICS_LOG "diagnostics.log"
#define MEMORY_DUMP "memory.dmp"

#define PATTERN_TABLE_SIZE 0x10000

#define MD_16BIT_COLOR 2
#define RGBA_32BIT_COLOR 4

#define PS_STATUS_VACANT 0
#define PS_STATUS_OCCUPIED 1

#define ECCO_2_US_ROM "ECCO_2_US_JUNE_1994.MD"
#define MD_SAVE_STATE "ECCO_2_US_JUNE_1994_SG.gsx"

#define HOMEBAY_PALETTE0_OFFSET 0x10afda
#define HOMEBAY_PALETTE1_OFFSET 0x10b0da
#define HOMEBAY_METATILE_OFFSET 0x1186f6 + 0x800

// 13b338
#define TRELLIASBAY_PALETTE1_OFFSET 0x135b96 - 0x100
#define TRELLIASBAY_METATILE_OFFSET 0x13b75c + 0x6800

#define VORTEXFUTURE_PALETTE1_OFFSET 0x181750
#define VORTEXFUTURE_METATILE_OFFSET 0x189680

#define SEAOFGREEN_PALETTE1_OFFSET 0xECDA2 - 0x104
#define SEAOFGREEN_METATILE_OFFSET 0xF507C + 0x2400

// #define HOMEBAY_METATILE_OFFSET 0x12a958 + 0x00

#define MD_STATE_CRAM 0x112
#define MD_STATE_VRAM 0x12478

typedef char t_unicode;

typedef uint8_t t_byte;
typedef uint16_t t_word;
typedef uint32_t t_dword;
typedef uint64_t t_qword;

/* string.c */

t_unicode *create_string(int length);
t_unicode *hex_string(int x);
t_unicode *int_string(int x);
t_unicode *float_string(float x);

t_unicode *copy_string(const t_unicode *x);

t_unicode *copy_append_string(const t_unicode *x, const t_unicode *y);
t_unicode *append_string_x(t_unicode *x, const t_unicode *y);
t_unicode *append_string_y(const t_unicode *x, t_unicode *y);
t_unicode *append_string_xy(t_unicode *x, t_unicode *y);

t_unicode *copy_append_nl(const t_unicode *v, const t_unicode *w);
t_unicode *copy_append_nl_hex(const t_unicode *v, const t_unicode *w, int x);
t_unicode *copy_append_nl_int(const t_unicode *v, const t_unicode *w, int x);
t_unicode *copy_append_nl_float(const t_unicode *v, const t_unicode *w, float x);

t_unicode *append_nl(t_unicode *w, const t_unicode *x);
t_unicode *append_nl_hex(t_unicode *w, const t_unicode *x, int y);
t_unicode *append_nl_int(t_unicode *w, const t_unicode *x, int y);
t_unicode *append_nl_float(t_unicode *w, const t_unicode *x, float y);

int append_log(const t_unicode *filename, const t_unicode *x);
int append_log_nl(const t_unicode *filename, const t_unicode *x);
int append_log_nl_hex(const t_unicode *filename, const t_unicode *x, int y);
int append_log_nl_int(const t_unicode *filename, const t_unicode *x, int y);
int append_log_nl_float(const t_unicode *filename, const t_unicode *x, float y);
int dump_memory(const t_unicode *filename, const t_byte *x, size_t size);

/* language.c */

#define LNG_RECORD_TOTAL 128

#define LNG_UNKNOWN_E       	0
#define LNG_COM_INITIALIZE_F	1
#define LNG_MEM_ALLOC_F			2
#define LNG_WIN_CREATE_F		3

#define LNG_D2D_CREATE_F		4
#define LNG_D2D_RTARGET_F		5
#define LNG_D2D_BITMAP_F		6

#define LNG_FILE_EOF		    7
#define LNG_FILE_UNKNOWN_E      8
#define LNG_FILE_OPEN_F		    9
#define LNG_FILE_READ_F		    10
#define LNG_FILE_WRITE_F		11

const t_unicode *get_language_record(const t_unicode **table, int id);
char **create_language_table(void);
void release_language_table(t_unicode **language);
void get_eng_language(t_unicode **language);

/* plot.c */

typedef struct t_region
{
    int x, y, w, h;
} t_region;

typedef struct t_process_pipeline
{
   int (*process)(void *);
   t_process_pipeline *next;

} t_process_pipeline;

typedef struct t_plot_state
{
    int id;
    int class_id; // Determins type of plot state (preprocessing, post, post with pre).

    int status_id; // Used to determine is a plot state is active or inactive.

    int transparent_id; // Transparent color in a palette
    int priority_id; // Determines which layer the tile should be drawn on.

    // Operations that need to be performed. 
    int vertical_flip;
    int horizontal_flip;

    t_byte *palette; // needs to be dwords!
    t_word *source;
    t_byte *destination;

    int palette_id;
    int palette_index;
    int palette_w;
    int palette_total;

    // Need to add this into the toolset.
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

    void *resource; // remove

    void *v_interrupt;
    void *h_interrupt; // Used for VH Interrupts.

    int (*input)(void *, int, int);
    int (*output)(void *, int, int);

    int (*plot)(void *);

    t_plot_state *next; // "user" defined.
    t_plot_state *_next; // next physical (free) plot_state.

} t_plot_state;

t_byte convert_md_rgba_channel(t_byte x);
t_byte convert2_md_rgba_channel(double x);

void copy_md16bit_md32bit_palette(t_byte *palette, t_byte *md_palette, int total);
void copy2_md16bit_md32bit_palette(t_byte *palette, t_byte *md_palette, int total);

void convert_md32bit_rgba32bit_palette(t_byte *palette, int total);
void convert_rgba32bit_bgra32bit_palette(t_byte *palette, int total);
void copy_md4bpp_16bpp_pattern(t_word *pattern, t_byte *md_pattern, int total);

t_region *create_region(int x, int y, int w, int h);
void set_region(t_region *region, int x, int y, int w, int h);
void clone_region(t_region *x, t_region *y);

t_plot_state *create_plot_state(void);
void release_plot_state(t_plot_state *plot_state);

void clear_plot_state(t_plot_state *ps);
void clone_plot_state(t_plot_state *x, t_plot_state *y);
t_plot_state *plot_state_add(t_plot_state *plot_state);

t_plot_state *plot_state_get_vacant(t_plot_state *plot_state);
t_plot_state *plot_state_add_vacant(t_plot_state *plot_state);

t_plot_state *plot_state_push_vacant(t_plot_state *plot_state);
void plot_state_pop_vacant(t_plot_state *plot_state);

void plot_state_clear_all(t_plot_state *plot_state);

void set_palette_id(t_plot_state *ps, int id);
int get_palette_color_index(t_plot_state *ps, int color_id);
void set_pattern_id(t_plot_state *ps, int id);

int ps_plot_default(void *plot_state);
int ps_plot_vflip(void *plot_state);
int ps_plot_hflip(void *plot_state);
int ps_plot_vhflip(void *plot_state);

/* core */

typedef struct t_core
{
	t_unicode **language;

	t_byte *palette;
    t_word *pattern;
    t_byte *viewport;

    int viewport_w, viewport_h;

    t_plot_state *plot_state;

} t_core;

t_core *create_core(void);
void release_core(t_core *core);

t_byte convert_md_rgba_channel(t_byte x);
t_byte convert2_md_rgba_channel(double x);

void copy_md16bit_md32bit_palette(t_byte *palette, t_byte *md_palette, int total);
void convert_md32bit_rgba32bit_palette(t_byte *palette, int total);
void convert_rgba32bit_bgra32bit_palette(t_byte *palette, int total);

/* testing */

int core_setup(t_core *core);
int core_process(t_core *core);