#pragma once

#ifndef _MAIN_HEADER
    #include "blue-dream.h"
#endif

#define EC_FOREGROUND 0x1
#define EC_BACKGROUND 0x1 << 1
#define EC_LAYER_MASK (EC_FOREGROUND | EC_BACKGROUND)

#define EC_ROW_PALETTE_TOTAL 4

#define EC_VERSION_ECCO2 1
#define EC_VERSION_ECCOJR 2

#define E2_STAGE_HEADER_SIZE 0x28

#define E2_STAGE_INDEX_MASK 0xff0000
#define E2_STAGE_DOMAIN_MASK 0x00ff00
#define E2_DOMAIN_STAGE_MASK 0x0000ff

// need to work on this and ecco jr palette switching.

#define E2_DOMAIN_PROLOGUE         0
#define E2_DOMAIN_HOMEBAY          1
#define E2_DOMAIN_TRELLIASBAY      2 
#define E2_DOMAIN_AQUATUBEWAY      3
#define E2_DOMAIN_SEAOFGREEN       4
#define E2_DOMAIN_ASTERITESHOME    5
#define E2_DOMAIN_LUNARBAY         6
#define E2_DOMAIN_VORTEXFUTURE     7
#define E2_DOMAIN_GLOBEHOLDER      8
#define E2_DOMAIN_BIGWATER         9
#define E2_DOMAIN_VORTEXQUEEN      10
#define E2_DOMAIN_ATLANTIS         11
#define E2_DOMAIN_TMACHINE         12

#define E2_DOMAIN_TOTAL		      (E2_DOMAIN_TMACHINE + 1)

#define E2_PROLOGUE       0x000000
#define E2_SELECTIONSCR   0x010100
#define E2_TWOTIDES       0x020101
#define E2_THELOSTORCAS   0x030102
#define E2_VENTSOFMEDUSA  0x040103
#define E2_FOURISLANDS    0x050104
#define E2_MAZEOFSTONE    0x060105
#define E2_HOMEBAY        0x070106
#define E2_SEAOFDARKNESS  0x080107
#define E2_CRYSTALSPRINGS 0x090108
#define E2_FAULTZONE      0x0a0109
#define E2_GATEWAY        0x0b010a
#define E2_TRELLIASBAY    0x0c0200
#define E2_SKYWAY         0x0d0201
#define E2_ASTERITESCAVE  0x0e0202
#define E2_EAGLESBAY      0x0f0203
#define E2_FINETOFEATHER  0x100204
#define E2_SKYLANDS       0x110205
#define E2_TUBEOFMEDUSA   0x120300
#define E2_VORTEXARRIVED  0x130301
#define E2_AQUATUBEWAY    0x140302  
#define E2_SKYTIDES       0x150303
#define E2_SEAOFGREEN     0x160400
#define E2_DEEPRIDGE      0x170401
#define E2_THEEYE         0x180402
#define E2_SEAOFBIRDS     0x190403
#define E2_SECRETCAVE     0x1a0404
#define E2_THEHUNGRYONES  0x1b0405
#define E2_CONVERGENCE    0x1c0406
#define E2_MORAYABYSS     0x1d0500
#define E2_ASTERITESHOME  0x1e0508
#define E2_EPILOGUE       0x1f0509
#define E2_LUNARBAY       0x200600
#define E2_DARKSEA        0x210601
#define E2_NEWMACHINE     0x220602
#define E2_INSIDE         0x230603
#define E2_INTER          0x240604
#define E2_INNUENDO       0x250605
#define E2_TRANS          0x260606
#define E2_BLACKCLOUDS    0x270700
#define E2_VORTEXFUTURE   0x280701
#define E2_GRAVITORBOX    0x290702
#define E2_GLOBEHOLDER    0x2a0800
#define E2_VORTEXQUEEN    0x2b0900
#define E2_BIGWATER       0x2c0a00
#define E2_THEPOD         0x2d0a01
#define E2_ATLANTIS       0x2e0b00
#define E2_FISHCITY       0x2f0b01
#define E2_CITYOFFOREVER  0x300b02
#define E2_TMACHINE       0x310c00
#define JR_HOMESEA        0x320d00
#define JR_BAYOFSONGS     0x330d01
#define JR_AQUAMAZE       0x340e02
#define JR_VENTSOFPEARL   0x350f00
#define JR_ENDLESSSEA     0x361000

#define E2_STAGE_TOTAL (((JR_ENDLESSSEA & E2_STAGE_INDEX_MASK) >> 24) + 1)


typedef struct t_ecco2_stage_properties
{
	// domain header
	int palette_offset;
	int blockset2_offset;
	int local_sprite_offset;
	int stage_list_offset;

	// stage header list
	int stage_total;
	int blockset1_offset;

	// stage header
	int objectmap2_offset;
	int objectmap1_offset;
	int foreground_w;
	int foreground_h;
	int origin_x;
	int origin_y;
	int background_w;
	int background_h;
	int attributes_offset;
	int foreground_offset;
	int background_offset;
	int objectmap_w;
	int objectmap_h;

	int blockset1_total;
	int blockset2_total;

	int stage_index;
	int domain_stage_id;
	char stage_name[0x11];

	int row_attribute_row[4];
	int row_attribute_palette[4];
	int palette_total;
} t_ecco2_stage_properties;

typedef struct t_ecco_stage_cell
{
    int table_id;

    int hflip;
    int vflip;
    int block_id;

	int table_index;
	int relative_x;
	int relative_y;

} t_level_cell1;

t_bitmap *ecco2_extract_stage_map(
	const char *rom_filename,
	int stage_id,
	int x, int y,
	int w, int h,
	int mode);