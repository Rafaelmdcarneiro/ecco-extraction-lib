#define PLZ_PALETTE_OFFSET 0xa
#define PLZ_PALETTE_SIZE 256
#define PLZ_PALETTE_BGCOLOR 0x40
#define PLZ_ROW_PALETTE_TOTAL (4 - 1) // 4th entry unused.

#define PLZ_TILE_WIDTH 16
#define PLZ_TILE_HEIGHT 16
#define PLZ_TILE_AREA 256
#define PLZ_TILE_SIZE 0x100

#define PLZ_BLOCK_HEADER_SIZE 0x12
#define PLZ_BLOCK_WIDTH 16
#define PLZ_BLOCK_HEIGHT 16
#define PLZ_BLOCK_PIXEL_W (PLZ_BLOCK_WIDTH * PLZ_TILE_WIDTH)
#define PLZ_BLOCK_PIXEL_H (PLZ_BLOCK_HEIGHT * PLZ_TILE_HEIGHT)

#define PLZ_BLOCK_SIZE 0x200

#define PLZ_FOREGROUND 0x1
#define PLZ_BACKGROUND (0x1 << 1)
#define PLZ_BACKGROUND2 (0x1 << 2)
#define PLZ_LAYER_MASK (PLZ_FOREGROUND | PLZ_BACKGROUND | PLZ_BACKGROUND2)

typedef struct t_plz_header
{
	char plz_signature[5];

	int palette_total;
	int pattern_total;
	int block_total;

	int pattern_offset;
	int block_offset;
	int stage_header_offset;

	int foreground_w;
	int foreground_h;

	int background_1_w;
	int background_1_h;

	int background_2_w;
	int background_2_h;

	int foreground_offset;
	int background_1_offset;
	int background_2_offset;

	int row_attribute_palette[4];

} t_eccopc_plz_header;

t_bitmap *plz_extract_stage_map(
	const char *filename,
	int x, int y,
	int w, int h,
	int mode);