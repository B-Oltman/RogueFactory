/*
rp_world contains the code that describes the pirate world.
*/
#ifndef RP_WORLD_RPH
#define RP_WORLD_RPH

#include "rp_terminal.h"
#include "rp_strings.h"
#include "rp_physics.h"
#include "rp_ui.h"

typedef enum
{
    ZDeepSea = 0,
    ZShallowSea,
    ZLand,
    ZDungeon,
    ZoneCount
}Zone_Type;

typedef struct
{
    int zoneCount;
    int zoneID[ZoneCount];
    char zoneName[ZoneCount][RP_MAX_MENU_NAME_LENGTH];
}ZoneList;

typedef struct ZoneBuilderInfo
{
    Zone_Type type;
    int zCellTypeCount;
}ZoneBuilderInfo;

typedef struct
{
    Zone_Type zoneID;
    char zoneName[RP_MAX_MENU_NAME_LENGTH];
}ZoneInfo;

// ZoneCells are cells that have slotting info so they can be attached to the UI and
// selected by the editor tools. It's used so that the uI doesn't need to know about
// entities.
typedef struct
{
    SlottingInfo slotInfo;
    Cell display;
}ZoneCell;

typedef enum
{
    // NOTE(baruch): Must be under 32 flags. To set flags, shift 1 << by flag value.
    Animated, // Needs to be added to frame update list.
    Blocking,
    EnvironmentFlagCount
}EnvironmentFlags;

typedef struct
{
    // A world entity is a zone cell that has been placed into a tile
    // it will have coordinates, a tile id and different flags describing how it will
    // exist in the game world.
    Cell cell;
    EnvironmentFlags flags; //Set flags like so: entity.flags = 1<<Animated | 1<<Blocking
    int parentTileId;
    short localX;
    short localY;
}EnvironmentEntity;

typedef struct
{
    char worldName[RP_MAX_FILE_NAME_LENGTH];
    int tileCount;
    int tileWidth;
    int tileHeight;
    int worldWidth;
    int worldHeight;
}WorldInfo;

typedef struct
{
    EnvironmentEntity *entityBuffer;
    ZoneInfo zInfo;
    int worldX;
    int worldY;
    int isLoaded;
    SlottingInfo slotInfo;
}WorldTile;

ZoneCell *BuildZoneCellPalette(ZoneBuilderInfo *zoneInfo);
void DestroyZoneCellBuffer(ZoneCell *zCellBuf);
void SetUpZone(Zone_Type zoneType, ZoneBuilderInfo *zInfo);
void GetZones(ZoneList *zoneList);

void CreateNewWorld(char worldName[RP_MAX_FILE_NAME_LENGTH], int worldWidth, int worldHeight, int tileWidth, int tileHeight, WorldInfo *worldInfo, WorldTile world[]);

void AllocateWorldTiles(WorldInfo *worldInfo, WorldTile world[]);

void DestroyWorld(WorldTile *world, WorldInfo *worldInfo);

void CreateWorldWarpMenu(WindowGroup *winGroup, Tile *displayBuffer, int width, int height, int startX, int startY, WorldTile world[], SlotSection *slotSection);
void
LoadWorldWarpMenu(WINDOW *menuWindow, SlotSection *slotSection, WorldTile world[], int activeWarpColor);

#define STANDARD_WORLD_WIDTH 40
#define STANDARD_WORLD_HEIGHT 32

// Zone Info
#define Z_DEEP_SEA_CELL_TYPES 6
#define Z_SHALLOW_SEA_CELL_TYPES 7
#define Z_LAND_CELL_TYPES 3
#define Z_DUNGEON_CELL_TYPES 3

#define Z_DEEP_SEA_NAME "Deep Sea"
#define Z_SHALLOW_SEA_NAME "Shallow Sea"
#define Z_LAND_NAME "On Land"
#define Z_DUNGEON_NAME "Inside Dungeon"

// Zone Cell Definitions. This is where the zone entity tiles are defined, this needs
// to be replaced by a database or table at some point.

//**********Deep Sea************
#define ZDS_FORE_STANDARD 87 //White standard fore color for deep sea zone
#define ZD_FORE_STANDARD Black

// zCell 0
#define ZDS0_LBL "Calm seas"
#define ZDS0_CHAR '~'
#define ZDS0_BACK 37
#define ZDS0_FORE ZDS_FORE_STANDARD

//zCell 1
#define ZDS1_LBL "Rough seas"
#define ZDS1_CHAR '^'
#define ZDS1_BACK 32
#define ZDS1_FORE ZDS_FORE_STANDARD

//zCell 2
#define ZDS2_LBL "Sea-Monster blood"
#define ZDS2_CHAR 'o'
#define ZDS2_BACK 47
#define ZDS2_FORE 88

//zCell 3
#define ZDS3_LBL "Westward current"
#define ZDS3_CHAR '<'
#define ZDS3_BACK 44
#define ZDS3_FORE 27

//zCell 4
#define ZDS4_LBL "Eastward current"
#define ZDS4_CHAR '>'
#define ZDS4_BACK 44
#define ZDS4_FORE 27

//zCell5
#define ZDS5_LBL "Gale force winds"
#define ZDS5_CHAR '~'
#define ZDS5_BACK 153
#define ZDS5_FORE 110

//*********ZShallowSea,*************
#define ZSS0_LBL "Low tide"
#define ZSS0_CHAR '~'
#define ZSS0_BACK 38
#define ZSS0_FORE ZDS_FORE_STANDARD

#define ZSS1_LBL "High tide"
#define ZSS1_CHAR '~'
#define ZSS1_BACK 32
#define ZSS1_FORE ZDS_FORE_STANDARD

#define ZSS2_LBL "Shoreline"
#define ZSS2_CHAR '~'
#define ZSS2_BACK 179
#define ZSS2_FORE 172

#define ZSS3_LBL "Hidden land"
#define ZSS3_CHAR '.'
#define ZSS3_BACK 137
#define ZSS3_FORE 179

#define ZSS4_LBL "Mysterious stone"
#define ZSS4_CHAR '#'
#define ZSS4_BACK 101
#define ZSS4_FORE 153

#define ZSS5_LBL "Disturbed water"
#define ZSS5_CHAR '^'
#define ZSS5_BACK 32
#define ZSS5_FORE ZDS_FORE_STANDARD

#define ZSS6_LBL "Island wall"
#define ZSS6_CHAR '#'
#define ZSS6_BACK 137
#define ZSS6_FORE 130

//***********ZLand,****************
#define ZL0_LBL "Shore"
#define ZL0_CHAR '.'
#define ZL0_BACK Tan
#define ZL0_FORE ZDS_FORE_STANDARD

#define ZL1_LBL "Pine tree"
#define ZL1_CHAR '^'
#define ZL1_BACK Tan
#define ZL1_FORE 64

#define ZL2_LBL "Palm tree"
#define ZL2_CHAR '%'
#define ZL2_BACK Tan
#define ZL2_FORE 70

//*********ZDungeon,***************
#define ZD0_LBL "Stone wall"
#define ZD0_CHAR '#'
#define ZD0_BACK 246
#define ZD0_FORE ZD_FORE_STANDARD

#define ZD1_LBL "Stone floor"
#define ZD1_CHAR '.'
#define ZD1_BACK 247
#define ZD1_FORE ZD_FORE_STANDARD

#define ZD2_LBL "Glowing rune"
#define ZD2_CHAR '&'
#define ZD2_BACK 246
#define ZD2_FORE 45
#endif

