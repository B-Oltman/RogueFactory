#ifndef RP_EDITOR_RH
#define RP_EDITOR_RH

#include "rp_terminal.h"
#include "rp_world.h"
#include "rp_user_input.h"
#include "rp_ui.h"

// Available editor states
typedef enum
{
    EditorInitialization,
    OnCanvas,
    QuitToMain,
    WarpWindow,
    WarpSelected,
    EditorStateCount
}Editor_State;

// Brush used to paint entities
typedef struct
{
    int size;
    int hasPaint;
    EnvironmentEntity paint;
}EnvironmentBrush;

// Contains everything that needs to persist in the main editor loop
typedef struct
{
    InputDefinition editorInputDefs[GameInputFlagCount];
    SlotSection warpWindowSlots;
    WorldInfo worldInfo;
    WorldTile world[STANDARD_WORLD_HEIGHT * STANDARD_WORLD_WIDTH];
    Editor_State editorState;
    int activeTileiD;
    int activeTooliD;
}Editor;

// Contains all of the data required by the canvas tools
typedef struct
{
    SlotSection uISlotSection[ZoneCount];
    ZoneBuilderInfo zInfo[ZoneCount];
    ZoneCell *zCellPalette[ZoneCount];
    Tile paletteBuffer[ZoneCount];
    EnvironmentBrush activeBrush;
    int canvasSavedFlag;
}CanvasToolData;

// Initialize editor, sets the editor up for the update loop
void InitEditor(Editor *editor, WindowGroup winGroup, Tile *uITile, Tile *bigUiTile);

// Update editor, this is where the editor does its work, it runs in the editor loop
void UpdateEditor(Editor *editor, CanvasToolData *canvasToolData, Input *inputContainer,
                  WindowGroup winGroup, Tile *bigUiTile, Tile *uITile);

// Renders entity to window and updates color pairs
void RenderEntityTileUpdatePairs(WorldTile *worldTile, WINDOW *displayDest,
                                 int tileHeight, int tileWidth);

// Renders entity to window and doesn't update color pairs
void RenderEntityTileLeavePairs(WorldTile *worldTile, WINDOW *displayDest,
                                int tileHeight, int tileWidth);

// Plot entity to buffer
int PlotEntity(EnvironmentBrush *activeBrush, Input inputContainer,
               EnvironmentEntity *entityCanvas, Window_Minimum canvasSize, WINDOW *canvasWindow);

/******************CANVAS TOOLS******************/
// Prepares the canvas tools, this includes the uI portion of the editor
void CreateCanvasTools(CanvasToolData *canvasToolData, int uIWinStartY,
                       WindowGroup winGroup, Editor *editor);

// Loads the zone editor section of the editor
ZoneCell *LoadZoneEditor(ZoneBuilderInfo *zInfo, Window_Minimum uIWindowMin,
                         SlotSection *slotSection);

// Updates the colors and character painted by the brush
void UpdateBrushPaint(EnvironmentBrush *brush, ZoneCell zCell);

/*****************CLEANING BUFFERS***************/
void DestroyEntityCanvas(EnvironmentEntity *entityCanvas);
void EndWorldEditor(CanvasToolData canvasToolData);


#endif

