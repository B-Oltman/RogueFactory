#include <stdlib.h>
#include "rp_world.h"
#include "rp_strings.h"
#include "rp_render.h"

void CreateWorldWarpMenu(WindowGroup *winGroup, Tile *displayBuffer, int width,
                         int height, int startX, int startY, WorldTile world[],
                         SlotSection *slotSection)
{
    // Create the slots for each warpable tile
    CreateSlotSection(*winGroup->cameraWin, winGroup->cameraWinMin, displayBuffer,
                      width, height, slotSection, startX, startY);
    
    for(int i = 0; i < (width * height); ++i)
    {
        // Assign the slot info of each world tile to it's corresponding slot section position
        AddToolToSlot(slotSection, i, world[i].slotInfo);
    }
    
    // Clear slot section display buffer
    FillTileBuffer(slotSection->parentContainerSize.minWidth,
                   slotSection->parentContainerSize.minHeight,
                   slotSection->displayBuffer, ' ', 0, 0);
}

void LoadWorldWarpMenu(WINDOW *menuWindow, SlotSection *slotSection, WorldTile world[],
                       int activeWarpColor)
{
    char plotChar;
    int fore;
    for(int y = 0, i = 0; y < slotSection->height; ++y)
    {
        for(int x = 0; x < (slotSection->width); ++x)
        {
            // If the world position has saved content, set it's tile display to a green o
            if(world[i].isLoaded)
            {
                plotChar = 'o';
                fore = 112;
                
                // If tile is loaded, set the default display in warp window to loaded tile
                slotSection->slots[i].defaultDisplay.fore = fore;
                slotSection->slots[i].defaultDisplay.back = Black;
                slotSection->slots[i].defaultDisplay.character = plotChar;
            }
            // If the tile does not have saved content, display a red x
            else
            {
                plotChar = 'x';
                fore = 124;
            }
            
            // Plot cells for warp window
            if(slotSection->slots[i].active == 0)
            {
                PlotCell(plotChar, slotSection->windowStartX + x,
                         slotSection->windowStartY + y, fore, Black, slotSection->displayBuffer,
                         slotSection->parentContainerSize);
                
                // Fill in default cells for highlighting
                slotSection->slots[i].defaultDisplay.fore = fore;
                slotSection->slots[i].defaultDisplay.back = Black;
                slotSection->slots[i].defaultDisplay.character = plotChar;
            }
            else
            {
                PlotCell(plotChar, slotSection->windowStartX + x,
                         slotSection->windowStartY + y, fore, activeWarpColor,
                         slotSection->displayBuffer, slotSection->parentContainerSize);
            }
            
            ++i;
        }
    }
}

// Gets list of zone ids with corresponding zone names
void GetZones(ZoneList *zoneList)
{
    // Copy initialized values to zone struct.
    zoneList->zoneCount = ZoneCount;
    
    zoneList->zoneID[0] = 1;
    RpStrCopy(zoneList->zoneName[0], Z_DEEP_SEA_NAME);
    
    zoneList->zoneID[1] = 2;
    RpStrCopy(zoneList->zoneName[1], Z_SHALLOW_SEA_NAME);
    
    zoneList->zoneID[2] = 3;
    RpStrCopy(zoneList->zoneName[2], Z_LAND_NAME);
    
    zoneList->zoneID[3] = 4;
    RpStrCopy(zoneList->zoneName[3], Z_DUNGEON_NAME);
}

// Fill zone info structure with proper zone data.
void SetUpZone(Zone_Type zoneType, ZoneBuilderInfo *zoneBuilderInfo)
{
    // Set zone info
    if(zoneType == ZDeepSea)
    {
        zoneBuilderInfo->zCellTypeCount = Z_DEEP_SEA_CELL_TYPES;
        zoneBuilderInfo->type = ZDeepSea;
    }
    else if(zoneType == ZShallowSea)
    {
        zoneBuilderInfo->zCellTypeCount = Z_SHALLOW_SEA_CELL_TYPES;
        zoneBuilderInfo->type = ZShallowSea;
    }
    else if(zoneType == ZLand)
    {
        zoneBuilderInfo->zCellTypeCount = Z_LAND_CELL_TYPES;
        zoneBuilderInfo->type = ZLand;
    }
    else if(zoneType == ZDungeon)
    {
        zoneBuilderInfo->zCellTypeCount = Z_DUNGEON_CELL_TYPES;
        zoneBuilderInfo->type = ZDungeon;
    }
}

// Allocate memory for zone display palette so that it can be used
// to construct world tiles.
ZoneCell *
BuildZoneCellPalette(ZoneBuilderInfo *zoneBuilderInfo)
{
    
    ZoneCell *zCellBuf = (ZoneCell *)calloc(zoneBuilderInfo->zCellTypeCount, sizeof(ZoneCell));
    
    if(zCellBuf == NULL)
    {
        
    }
    
    // Builds zCells with pre-defined data in header.
    // TODO(baruch): Read from database file instead of #defines in header
    
    if(zoneBuilderInfo->type == ZDeepSea)
    {
        RpStrCopy(zCellBuf[0].slotInfo.label, ZDS0_LBL);
        zCellBuf[0].slotInfo.iD = 0;
        zCellBuf[0].display.character = ZDS0_CHAR;
        zCellBuf[0].display.fore = ZDS0_FORE;
        zCellBuf[0].display.back = ZDS0_BACK;
        
        RpStrCopy(zCellBuf[1].slotInfo.label, ZDS1_LBL);
        zCellBuf[1].slotInfo.iD = 1;
        zCellBuf[1].display.character = ZDS1_CHAR;
        zCellBuf[1].display.fore = ZDS1_FORE;
        zCellBuf[1].display.back = ZDS1_BACK;
        
        RpStrCopy(zCellBuf[2].slotInfo.label, ZDS2_LBL);
        zCellBuf[2].slotInfo.iD = 2;
        zCellBuf[2].display.character = ZDS2_CHAR;
        zCellBuf[2].display.fore = ZDS2_FORE;
        zCellBuf[2].display.back = ZDS2_BACK;
        
        RpStrCopy(zCellBuf[3].slotInfo.label, ZDS3_LBL);
        zCellBuf[3].slotInfo.iD = 3;
        zCellBuf[3].display.character = ZDS3_CHAR;
        zCellBuf[3].display.fore = ZDS3_FORE;
        zCellBuf[3].display.back = ZDS3_BACK;
        
        RpStrCopy(zCellBuf[4].slotInfo.label, ZDS4_LBL);
        zCellBuf[4].slotInfo.iD = 4;
        zCellBuf[4].display.character = ZDS4_CHAR;
        zCellBuf[4].display.fore = ZDS4_FORE;
        zCellBuf[4].display.back = ZDS4_BACK;
        
        RpStrCopy(zCellBuf[5].slotInfo.label, ZDS5_LBL);
        zCellBuf[5].slotInfo.iD = 5;
        zCellBuf[5].display.character = ZDS5_CHAR;
        zCellBuf[5].display.fore = ZDS5_FORE;
        zCellBuf[5].display.back = ZDS5_BACK;
    }
    else if(zoneBuilderInfo->type == ZShallowSea)
    {
        RpStrCopy(zCellBuf[0].slotInfo.label, ZSS0_LBL);
        zCellBuf[0].slotInfo.iD = 0;
        zCellBuf[0].display.character = ZSS0_CHAR;
        zCellBuf[0].display.fore = ZSS0_FORE;
        zCellBuf[0].display.back = ZSS0_BACK;
        
        RpStrCopy(zCellBuf[1].slotInfo.label, ZSS1_LBL);
        zCellBuf[1].slotInfo.iD = 1;
        zCellBuf[1].display.character = ZSS1_CHAR;
        zCellBuf[1].display.fore = ZSS1_FORE;
        zCellBuf[1].display.back = ZSS1_BACK;
        
        RpStrCopy(zCellBuf[2].slotInfo.label, ZSS2_LBL);
        zCellBuf[2].slotInfo.iD = 2;
        zCellBuf[2].display.character = ZSS2_CHAR;
        zCellBuf[2].display.fore = ZSS2_FORE;
        zCellBuf[2].display.back = ZSS2_BACK;
        
        RpStrCopy(zCellBuf[3].slotInfo.label, ZSS3_LBL);
        zCellBuf[3].slotInfo.iD = 3;
        zCellBuf[3].display.character = ZSS3_CHAR;
        zCellBuf[3].display.fore = ZSS3_FORE;
        zCellBuf[3].display.back = ZSS3_BACK;
        
        RpStrCopy(zCellBuf[4].slotInfo.label, ZSS4_LBL);
        zCellBuf[4].slotInfo.iD = 4;
        zCellBuf[4].display.character = ZSS4_CHAR;
        zCellBuf[4].display.fore = ZSS4_FORE;
        zCellBuf[4].display.back = ZSS4_BACK;
        
        RpStrCopy(zCellBuf[5].slotInfo.label, ZSS5_LBL);
        zCellBuf[5].slotInfo.iD = 5;
        zCellBuf[5].display.character = ZSS5_CHAR;
        zCellBuf[5].display.fore = ZSS5_FORE;
        zCellBuf[5].display.back = ZSS5_BACK;
        
        RpStrCopy(zCellBuf[6].slotInfo.label, ZSS6_LBL);
        zCellBuf[6].slotInfo.iD = 6;
        zCellBuf[6].display.character = ZSS6_CHAR;
        zCellBuf[6].display.fore = ZSS6_FORE;
        zCellBuf[6].display.back = ZSS6_BACK;
    }
    else if(zoneBuilderInfo->type == ZLand)
    {
        RpStrCopy(zCellBuf[0].slotInfo.label, ZL0_LBL);
        zCellBuf[0].slotInfo.iD = 0;
        zCellBuf[0].display.character = ZL0_CHAR;
        zCellBuf[0].display.fore = ZL0_FORE;
        zCellBuf[0].display.back = ZL0_BACK;
        
        RpStrCopy(zCellBuf[1].slotInfo.label, ZL1_LBL);
        zCellBuf[1].slotInfo.iD = 1;
        zCellBuf[1].display.character = ZL1_CHAR;
        zCellBuf[1].display.fore = ZL1_FORE;
        zCellBuf[1].display.back = ZL1_BACK;
        
        RpStrCopy(zCellBuf[2].slotInfo.label, ZL2_LBL);
        zCellBuf[2].slotInfo.iD = 2;
        zCellBuf[2].display.character = ZL2_CHAR;
        zCellBuf[2].display.fore = ZL2_FORE;
        zCellBuf[2].display.back = ZL2_BACK;
    }
    else if(zoneBuilderInfo->type == ZDungeon)
    {
        RpStrCopy(zCellBuf[0].slotInfo.label, ZD0_LBL);
        zCellBuf[0].slotInfo.iD = 0;
        zCellBuf[0].display.character = ZD0_CHAR;
        zCellBuf[0].display.fore = ZD0_FORE;
        zCellBuf[0].display.back = ZD0_BACK;
        
        RpStrCopy(zCellBuf[1].slotInfo.label, ZD1_LBL);
        zCellBuf[1].slotInfo.iD = 1;
        zCellBuf[1].display.character = ZD1_CHAR;
        zCellBuf[1].display.fore = ZD1_FORE;
        zCellBuf[1].display.back = ZD1_BACK;
        
        RpStrCopy(zCellBuf[2].slotInfo.label, ZD2_LBL);
        zCellBuf[2].slotInfo.iD = 2;
        zCellBuf[2].display.character = ZD2_CHAR;
        zCellBuf[2].display.fore = ZD2_FORE;
        zCellBuf[2].display.back = ZD2_BACK;
    }
    
    return zCellBuf;
}

// TODO(baruch): Not used right now, need to replace CreateNewWorld
// with this function when loading a world rather than creating eventually
void AllocateWorldTiles(WorldInfo *worldInfo, WorldTile world[])
{
    for(int i = 0; i < worldInfo->tileCount; ++i)
    {
        world[i].entityBuffer = (EnvironmentEntity *)malloc((worldInfo->tileWidth *
                                                             worldInfo->tileHeight) *
                                                            sizeof(EnvironmentEntity));
    }
}

void CreateNewWorld(char worldName[RP_MAX_FILE_NAME_LENGTH], int worldWidth,
                    int worldHeight, int tileWidth, int tileHeight, WorldInfo *worldInfo, 
                    WorldTile world[])
{
    // Fill world header
    RpStrCopy(worldInfo->worldName, worldName);
    worldInfo->tileWidth = tileWidth;
    worldInfo->tileHeight = tileHeight;
    worldInfo->tileCount = worldWidth  * worldHeight;
    worldInfo->worldWidth = worldWidth;
    worldInfo->worldHeight = worldHeight;
    
    for(int y = 0, i = 0; y < worldInfo->worldHeight; ++y)
    {
        for(int x = 0; x < worldInfo->worldWidth; ++x)
        {
            world[i].entityBuffer = (EnvironmentEntity *)malloc((tileWidth * tileHeight) *
                                                                sizeof(EnvironmentEntity));
            // Fill out world tile data
            world[i].worldX = x;
            world[i].worldY = y;
            world[i].slotInfo.iD = i;
            world[i].slotInfo.labelType = ClickToShow;
            world[i].isLoaded = 0;
            char intBuf[10];
            RpStrCopy(world[i].slotInfo.label, "Tile_");
            RpIntToString(i, intBuf);
            RpStrAppend(world[i].slotInfo.label, intBuf);
            
            // Clear each tile
            for(int Y = 0, j = 0; Y < tileHeight; ++Y)
            {
                for(int X = 0; X < tileWidth; ++X)
                {
                    world[i].entityBuffer[j].cell.character = ' ';
                    world[i].entityBuffer[j].cell.fore = Black;
                    world[i].entityBuffer[j].cell.character = Black;
                    ++j;
                }
            }
            
            ++i;
        }
        
    }
}

void DestroyWorld(WorldTile *world, WorldInfo *worldInfo)
{
    int count = worldInfo->tileCount;
    // Free all world tile buffers
    for(int i = 0; i < count; ++i)
    {
        free(world[i].entityBuffer);
    }
}

void DestroyZoneCellBuffer(ZoneCell *zCellBuf)
{
    free(zCellBuf);
}
