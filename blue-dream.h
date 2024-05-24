#pragma once

#define _MAIN_HEADER
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

#define m_alloc(x) (x *)malloc(sizeof(x));
#define m_array(x, y) (x *)malloc(sizeof(x) * y);
#define m_parray(x, y) (x **)malloc(sizeof(x *) * y);

#define NOT_EQUAL(x, y) ((x) != (y))
#define NOT_NULL(x) ((x) != NULL)
#define NOT_ZERO(x) ((x) != 0)

#define AND_EQUAL(x, y) ((x & y) == y)
#define AND_NEQUAL(x, y) ((x & y) != y)

#define SIGNX(x) ((x) | 0xffffffffffffff00)

#define PROGRAM_NAME L"Project Black Omen"
#define PROGRAM_VERSION L"00.11292020"

#define VIEWPORT_W 640
#define VIEWPORT_H 480

#define DIAGNOSTICS_LOG "diagnostics.log"
#define MEMORY_DUMP "memory.dmp"
#define DUMP_DIR L"DUMP1"
#define DUMP_DIR2 L"DUMP2"

#define WORD_SIZE 2
#define DWORD_SIZE 4

#define MD_PATTERN_TABLE_SIZE 0x10000
#define PATTERN_TABLE_SIZE 0x20000 // number of pixels

#define MD_PALETTE_TOTAL 64

#define MD_TILE_WIDTH 8
#define MD_TILE_HEIGHT 8
#define MD_TILE_AREA (MD_TILE_WIDTH * MD_TILE_HEIGHT)

#define MD_RAM_SIZE 0x10000
#define MD_VRAM_SIZE 0x10000

#define MD_RAM_ADDRESS 0xffff0000
#define MD_RAM_MASK 0x0000ffff

#define SPRITECEL_MAX 64

#define ETD_RAW_BLOCK_SIZE 0x100 * 2
#define ETD_BLOCK_SIZE 0x100

#define ECCO_SPRITE_CEL_SIZE 6
#define PC_SPRITE_CEL_SIZE 20
#define PC_SPRITE_SEQUENCE 12

#define ECCO_ROM_BANK 0
#define ECCO_RAM_BANK 1

#define ECCO_US_ROM "ECCO_US_OCT_1992.MD"
#define ECCO2_US_ROM "ECCO_2_US_JUNE_1994.MD"
#define ECCOJR_US_ROM "ECCO_JR_MAR_1995.MD"
#define ECCO2_PARADOX_ROM "ECCO_2_PROTOTYPE_PARADOX.MD"
#define ECCO2_X11_ROM "ECCO_2_PROTOTYPE_X11_APR_13_1994.MD"

#define ECCO2_SPRITE_DOLPHIN 0x1e0248
#define ECCO2_SPRITE_SHARK 0x1e359c

#define ECCOPC_PALETTE_00 0xc

#define MD_STATE_CRAM 0x112
#define MD_STATE_VRAM 0x12478
#define MD_STATE_68KRAM 0x2478

typedef uint8_t t_byte;
typedef uint16_t t_word;
typedef uint32_t t_dword;
typedef uint64_t t_qword;

t_dword lbyte(t_dword x);
t_dword hbyte(t_dword x);
t_dword lword(t_dword x);
t_dword hword(t_dword x);
t_dword bswap(t_dword x);
t_dword wswap(t_dword x);

t_dword unpack_dword(t_byte *u, t_qword v);
t_word unpack_word(t_byte *u, t_qword v);

/* string.c */

char *create_string(int length);
char *hex_string(int x);
char *int_string(int x);
char *float_string(float x);

void clone_string(const char *x, char *y);
char *copy_string(const char *x);

char *copy_append_string(const char *x, const char *y);
char *append_string_x(char *x, const char *y);
char *append_string_y(const char *x, char *y);
char *append_string_xy(char *x, char *y);

char *copy_append_nl(const char *v, const char *w);
char *copy_append_nl_hex(const char *v, const char *w, int x);
char *copy_append_nl_int(const char *v, const char *w, int x);
char *copy_append_nl_float(const char *v, const char *w, float x);

char *append_nl(char *w, const char *x);
char *append_nl_hex(char *w, const char *x, int y);
char *append_nl_int(char *w, const char *x, int y);
char *append_nl_float(char *w, const char *x, float y);

int append_log(const char *filename, const char *x);
int append_log_nl(const char *filename, const char *x);
int append_log_nl_hex(const char *filename, const char *x, int y);
int append_log_nl_int(const char *filename, const char *x, int y);
int append_log_nl_float(const char *filename, const char *x, float y);

int log_int(const char *x, int y);
int log_hex(const char *x, int y);
int dump_memory(const t_byte *x, size_t size);

/* language.c */

#define LNG_RECORD_TOTAL 128

#define LNG_UNKNOWN_E       	0
#define LNG_COM_INITIALIZE_F	1
#define LNG_MEMm_alloc_F		2
#define LNG_WIN_CREATE_F		3

#define LNG_D2D_CREATE_F		4
#define LNG_D2D_RTARGET_F		5
#define LNG_D2D_BITMAP_F		6

#define LNG_FILE_EOF		    7
#define LNG_FILE_UNKNOWN_E      8
#define LNG_FILE_OPEN_F		    9
#define LNG_FILE_READ_F		    10
#define LNG_FILE_WRITE_F		11

const char *get_language_record(const char **table, int id);
char **create_language_table(void);
void release_language_table(char **language);
void get_eng_language(char **language);

int read_file(const char *filename, t_byte **stream);

#include "megadrive.h"
#include "plot.h"
#include "bitmap.h"

/* zlm_format */

typedef struct t_zlm_sprite_cel
{
    int relative_x;
    int relative_y;
    int width;
    int height;

    // Mask width is adjusted to fit on an even byte boundry.
    int mask_width;
    int mask_column_size; // (mask_width/sizeof(byte))

    t_dword bitmap_offset;
    t_dword mask_offset;
} t_zlm_sprite_cel;

typedef struct t_zlm_header
{
    t_dword id;
    t_dword size;
    t_word sequence_total;
    t_word cel_total;

    t_dword *sequence;
} t_zlm_header;

t_zlm_sprite_cel *create_zlm_sprite_cel(void);
t_zlm_header *create_zlm_header(void);

int zlm_header_set_resource(t_zlm_header *header, t_byte *resource);
void zlm_cel_set_resource(t_zlm_sprite_cel *cel, t_byte *resource);

t_dword *plz_palette_create_resource(t_byte *resource);
t_zlm_sprite_cel *zlm_cel_create_resource(
    t_byte *resource,
    int sequence_index,
    int cel_index,
    int *sequence_total,
    int *cel_total,
    int *_result);

int zlm_plot_sprite_bitmap(
    t_plot_process *pp,
    t_zlm_sprite_cel *cel,
    t_byte *resource);

int zlm_plot_sprite_mask(
    t_plot_process *pp,
    t_zlm_sprite_cel *cel,
    t_byte *resource);

#include "ecco-block.h"
#include "ecco-1-stage.h"
#include "ecco-2-stage.h"
#include "ecco-pc-stage.h"

/* SPRITE BEGIN */

typedef struct t_ecco_sprite_cel
{
    int palette_id;
    int vflip;
    int hflip;

    int terminate;
    int duplicate;
    int pattern_id;

    int row_total;
    int column_total;

    int relative_x;
    int relative_y;
} t_ecco_sprite_cel;

void clear_ecco_sprite_cel(t_ecco_sprite_cel *sp);
t_ecco_sprite_cel *create_ecco_sprite_cel(void);
int ecco_sprite_get_cel_raw(
    t_ecco_sprite_cel *sp,
    t_byte *table,
    int index);

void plot_ecco_sprite_tile(
    t_plot_state *ps,
    t_dword *source,
    t_bitmap *destination,
    t_ecco_sprite_cel *cel,

    int column,
    int row,
    int pattern_index);

t_bitmap *plot_ecco_global_sprite(
    t_plot_state *ps,
    t_dword *source,
    t_byte *resource);

t_bitmap *plot_ecco_local_sprite(
    t_plot_state *ps,
    t_dword *source,
    t_byte *resource);

/* END */

typedef struct t_core
{
    double rotate;

	char **language;

    
    t_byte *resource;
    int resource_size;

    t_dword *surface;
    int surface_w;
    int surface_h;

    t_plot_state *pp;
    t_plot_process *plot_process;

    t_dword palette_offset;
    t_dword pattern_offset;

    t_byte *block;
	t_dword *palette;
    t_dword *pattern;
    t_dword *viewport;

    int viewport_w, viewport_h;

    t_plot_state *plot_state;

} t_core;