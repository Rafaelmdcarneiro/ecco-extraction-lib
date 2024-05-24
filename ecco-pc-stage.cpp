#include "blue-dream.h"

t_plz_header *plz_read_header(t_byte *resource)
{
	int x = 0, y = 0, z = 0;
	int offset = 0;

	char plz_signature[] = { 0x3F, 0xD6, 0x2B, 0x01, 0x00 };

	t_plz_header *header = NULL;
	
	header = m_alloc(t_plz_header);
	if (header == NULL) return NULL;

	header->plz_signature[0] = resource[0];
	header->plz_signature[1] = resource[1];
	header->plz_signature[2] = resource[2];
	header->plz_signature[3] = resource[3];
	header->plz_signature[4] = '\0';

	if (NOT_ZERO(strcmp(header->plz_signature, plz_signature)))
		return NULL;

	x = unpack_word(resource, 0x4);
	y = unpack_word(resource, 0x6);
	z = unpack_word(resource, 0x8);

	x = bswap(x);
	y = bswap(y);
	z = bswap(z);

	header->palette_total = x;
	header->pattern_total = y;
	header->block_total = z;

	header->pattern_offset = PLZ_PALETTE_OFFSET;
	header->pattern_offset+= (header->palette_total * PLZ_PALETTE_SIZE) * DWORD_SIZE;
	header->pattern_offset+= header->palette_total * WORD_SIZE;

	header->block_offset = header->pattern_offset;
	header->block_offset+= header->pattern_total * PLZ_TILE_SIZE;

	header->stage_header_offset = header->block_offset;
	header->stage_header_offset += header->block_total * (PLZ_BLOCK_SIZE + PLZ_BLOCK_HEADER_SIZE);

	offset = header->stage_header_offset;

	header->row_attribute_palette[0] = resource[offset + 0x2e];
	header->row_attribute_palette[1] = resource[offset + 0x2f];
	header->row_attribute_palette[2] = resource[offset + 0x30];
	header->row_attribute_palette[3] = resource[offset + 0x31];

	x = unpack_word(resource, offset + 0x36);
	y = unpack_word(resource, offset + 0x38);

	x = bswap(x);
	y = bswap(y);

	header->foreground_w = x;
	header->foreground_h = y;

	x = unpack_word(resource, offset + 0x3e);
	y = unpack_word(resource, offset + 0x40);

	x = bswap(x);
	y = bswap(y);

	header->background_2_w = x;
	header->background_2_h = y;

	x = unpack_word(resource, offset + 0x46);
	y = unpack_word(resource, offset + 0x48);

	x = bswap(x);
	y = bswap(y);

	header->background_1_w = x;
	header->background_1_h = y;

	header->foreground_offset = offset + 0x4e;

	offset = header->foreground_offset;
	header->background_2_offset = offset;
	header->background_2_offset+= (header->foreground_w * 
		header->foreground_h) * WORD_SIZE;

	offset = header->background_2_offset;
	header->background_1_offset = offset;
	header->background_1_offset += (header->background_2_w * 
		header->background_2_h) * WORD_SIZE;

	return header;
}

void plz_convert_palette_id(
	int *palette_id,
    int *_row_attribute_palette)
{
	int x = 0, y = 0;
	int palette_index[4] = { 0 };

	for (x = 0; x < PLZ_ROW_PALETTE_TOTAL; x++)
	{
		for (y = 0; y < PLZ_ROW_PALETTE_TOTAL; y++)
		{
			if (_row_attribute_palette[x] == palette_id[y])
			{
				palette_index[x] = y;
				break;
			}
		}
	}

	_row_attribute_palette[0] = palette_index[0];
	_row_attribute_palette[1] = palette_index[1];
	_row_attribute_palette[2] = palette_index[2];
	_row_attribute_palette[3] = palette_index[3];
}

t_dword *plz_read_palette(
	t_byte *raw_palette, 
	int palette_total, 
	int *_row_attribute_palette)
{
	int x = 0, y = 0, z = 0;
	int index = 0, offset = 0, size = 0;

	t_dword *palette = NULL;
	int palette_id[4] = { 0 };
	
	size = PLZ_PALETTE_SIZE * palette_total;

	palette = m_array(t_dword, size);
	if (palette == NULL) { return NULL; }

	for (x = 0; x < palette_total; x++)
	{
		index = x * PLZ_PALETTE_SIZE;

		offset = index * DWORD_SIZE;
		offset += x * 2;

		z = unpack_word(raw_palette, offset);
		z = bswap(z);

		if (x < PLZ_ROW_PALETTE_TOTAL) palette_id[x] = z;

		offset = index * DWORD_SIZE;
		offset += (x + 1) * 2;

		for (y = 0; y < PLZ_PALETTE_SIZE; y++)
		{
			z = unpack_dword(raw_palette, offset + (y * 4));
			z = bswap(wswap(bswap(z)));

			palette[index + y] = z | 0xff000000;
		}
	}

	if (NOT_NULL(_row_attribute_palette))
		plz_convert_palette_id(
			palette_id, _row_attribute_palette);

	return palette;
}

t_dword *plz_read_pattern(t_byte *raw_pattern, int pattern_total)
{
	int x = 0;
	t_dword *pattern = NULL;
	int size = 0;

	size = pattern_total * 0x100;

	pattern = m_array(t_dword, size);
	if (pattern == NULL) return NULL;

	for (x = 0; x < size; x++)
		pattern[x] = raw_pattern[x];

	return pattern;
}

t_bitmap *plz_create_block(
	t_byte *raw_block,
	t_bitmap **tileset,
	int w, int h,
	int mode)
{
	t_bitmap *destination = NULL;

	int x = 0, y = 0;
	int tile_index = 0;

	destination = create_bitmap(
		w * PLZ_TILE_WIDTH,
		h * PLZ_TILE_HEIGHT);
	if (destination == NULL) return NULL;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			tile_index = unpack_word(raw_block, ((y * PLZ_TILE_WIDTH) + x) * 2);
			tile_index = bswap(tile_index);
			tile_index = tile_index & 0x3fff;

			draw_bitmap_direct(
				tileset[tile_index],
				destination,
				x * PLZ_TILE_WIDTH,
				y * PLZ_TILE_HEIGHT,
				mode);
		}
	}

	return destination;
}

t_bitmap **plz_create_blockset(
	t_byte *raw_blockset,
	t_bitmap **tileset,
	int block_total)
{
	t_bitmap **blockset = NULL;
	int index = 0, offset = 0;

	blockset = m_parray(t_bitmap, block_total);
	if (blockset == NULL) return NULL;
	
	for (index = 0; index < block_total; index++)
	{
		offset = index * 0x200;
		offset += (index + 1) * 0x12;

		blockset[index] = plz_create_block(
			&raw_blockset[offset], 
			tileset, 
			PLZ_BLOCK_WIDTH,
			PLZ_BLOCK_HEIGHT, 0);

		if (blockset[index] == NULL)
			return NULL;
	}

	return blockset;
}

t_bitmap **plz_extract_tileset(
	t_byte *file,
	t_plz_header *header
)
{
	t_dword *pattern = NULL;
	t_bitmap **tileset = NULL;

	int size = 0;

	pattern = plz_read_pattern(
		&file[header->pattern_offset],
		header->pattern_total);
	if (pattern == NULL) return NULL;

	size = header->pattern_total * PLZ_TILE_AREA;
	
	tileset = create_tileset(
		pattern,
		PLZ_TILE_WIDTH, PLZ_TILE_HEIGHT,
		size,
		NULL);

	free(pattern);
	return tileset;
}

t_bitmap **plz_extract_blockset(
	t_byte *file,
	t_plz_header *header)
{
	t_bitmap **blockset = NULL;
	t_bitmap **tileset = NULL;

	tileset = plz_extract_tileset(file, header);
	if (tileset == NULL) return NULL;

	blockset = plz_create_blockset(
		&file[header->block_offset],
		tileset,
		header->block_total);

	release_bitmap_array(tileset, header->pattern_total);
	return blockset;
}

void plz_enable_palette_transparent_pixel(
	t_dword *palette, int palette_total)
{
	int index = 0;
	for (index = 0; index < palette_total; index++)
	{
		palette[(index * 256) + 0] = 0;
		palette[(index * 256) + 0x10] = 0;
		palette[(index * 256) + 0x20] = 0;
		palette[(index * 256) + 0x30] = 0;
		palette[(index * 256) + 0x40] = 0;
	}
}

void plz_get_stage_layer(
	t_plz_header *header,
	int mode,
	int *_stage_w,
	int *_stage_h,
	int *_stage_offset)
{
	switch (mode & PLZ_LAYER_MASK)
	{
		case PLZ_BACKGROUND:
		{
			*_stage_w = header->background_1_w;
			*_stage_h = header->background_1_h;
			*_stage_offset = header->background_1_offset;

			break;
		}

		case PLZ_BACKGROUND2:
		{
			*_stage_w = header->background_2_w;
			*_stage_h = header->background_2_h;
			*_stage_offset = header->background_2_offset;

			break;
		}

		case PLZ_FOREGROUND:
		default:
		{
			*_stage_w = header->foreground_w;
			*_stage_h = header->foreground_h;
			*_stage_offset = header->foreground_offset;

			break;
		}
	}
}

int plz_get_palette_row_index(int row, int *row_attribute_palette)
{
	switch (row)
	{
		case 0: return row_attribute_palette[0];
		case 1: return row_attribute_palette[0];
		case 2: return row_attribute_palette[1];
	}

	return row_attribute_palette[2];
}

void plz_calculate_map_offsets(
	int *_x,
	int *_y,
	int *_column_offset,
	int *_row_offset)
{
	*_column_offset = *_x / PLZ_BLOCK_PIXEL_W;
	*_row_offset = *_y / PLZ_BLOCK_PIXEL_H;

	*_x = *_x - (*_column_offset * PLZ_BLOCK_PIXEL_W);
	*_y = *_y - (*_row_offset * PLZ_BLOCK_PIXEL_H);
}

void plz_calculate_map_dimensions(
	int x,
	int y,
	int map_width,
	int map_height,
	int *_w,
	int *_h,
	int *_column_length,
	int *_row_length
)
{
	if (*_w == 0) *_w = (map_width * PLZ_BLOCK_PIXEL_W) - x;
	if (*_h == 0) *_h = (map_height * PLZ_BLOCK_PIXEL_H) - y;

	*_column_length = (((*_w + x) / PLZ_BLOCK_PIXEL_W) + 1) + 1;
	*_row_length = (((*_h + y) / PLZ_BLOCK_PIXEL_H) + 1) + 1;
}

t_bitmap *plz_create_stage_map(
	t_plz_header *header,
	t_dword *palette,
	t_bitmap **blockset,
	t_byte *file,
	int x, int y, 
	int w, int h,
	int mode)
{
	int stage_w = 0, stage_h = 0;
	int stage_offset = 0;
	t_bitmap *stage_map = NULL;

	int row = 0, column = 0;
	int row_offset = 0, column_offset = 0;
	int row_length = 0, column_length = 0;
	
	int offset = 0, z = 0;
	int row_attribute_palette_index = 0;
	int palette_index = 0;

	plz_get_stage_layer(
		header,
		mode,
		&stage_w,
		&stage_h,
		&stage_offset);

	plz_calculate_map_offsets(
		&x, &y,
		&column_offset,
		&row_offset);

	plz_calculate_map_dimensions(
		x, y,
		stage_w,
		stage_h,
		&w, &h,
		&column_length,
		&row_length
	);

	stage_map = create_bitmap(w, h);
	if (stage_map == NULL) return NULL;

	if (AND_EQUAL(mode, PS_TRANSPARENTPIXEL_CLEAR))
		plz_enable_palette_transparent_pixel(
			palette, header->palette_total);

	if (AND_EQUAL(mode, PS_TRANSPARENTPIXEL_FILL))
		set_background_color(stage_map, palette[PLZ_PALETTE_BGCOLOR]);

	for (row = 0; row < row_length; row++)
	{
		if ((row_offset + row) >= stage_h)
			break;

		palette_index = plz_get_palette_row_index(
			(row_offset + row), header->row_attribute_palette);

		for (column = 0; column < column_length; column++)
		{
			if ((column_offset + column) >= stage_w)
				break;

			offset = stage_offset;
			offset += (((row_offset + row) * stage_w) + 
				(column_offset + column)) * 2;

			z = unpack_word(file, offset);
			z = lbyte(bswap(z));

			if (NOT_EQUAL(z, 0xff))
			{
				draw_bitmap_indexed(
					palette,
					blockset[z],
					stage_map,
					(column * PLZ_BLOCK_PIXEL_W) - x,
					(row * PLZ_BLOCK_PIXEL_H) - y,
					PLZ_PALETTE_SIZE * palette_index, 0);
			}
		}
	}

	return stage_map;
}

t_bitmap *plz_extract_stage_map(
	const char *filename,
	int x, int y,
	int w, int h,
	int mode)
{
	int size = 0;
	t_byte *file = NULL;

	t_plz_header *header = NULL;

	t_dword *palette = NULL;
	t_bitmap **blockset = NULL;
	t_bitmap *stage_map = NULL;

	size = read_file((char *)filename, &file);
	if (size < 0) return NULL;

	header = plz_read_header(file);
	if (header == NULL) return NULL;

	palette = plz_read_palette(
		&file[PLZ_PALETTE_OFFSET],
		header->palette_total,
		header->row_attribute_palette);
	if (palette == NULL) return NULL;

	blockset = plz_extract_blockset(file, header);
	if (blockset == NULL) return NULL;

	if ((mode & EC_FOREGROUND))
		mode |= PS_TRANSPARENTPIXEL_CLEAR;
	else
		mode |= PS_TRANSPARENTPIXEL_FILL;

	stage_map = plz_create_stage_map(
		header,
		palette,
		blockset,
		file,
		x, y,
		w, h,
		mode);

	if (stage_map == NULL) return NULL;

	release_bitmap_array(
		blockset, header->block_total);

	free(palette);
	free(file);
	free(header);
	
	return stage_map;
}