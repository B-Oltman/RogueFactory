/*
Render will handle all display buffers. It will use grids to build buffers for display
 and flip buffers on frame boundary.
*/
#ifndef RENDER_RPH
#define RENDER_RPH
#include "rp_terminal.h"
#include "rp_colors.h"

// Plot colors and character to world tile buffer
int PlotCell(int character, int x, int y, Color fore, Color back, Tile *tile, 
             Window_Minimum windowMin);

// Fill tile with passed colors and character
void FillTileBuffer(int tileWidth, int tileHeight, Tile *tile, char fillChar, Color fore, Color back);

// Render tile row to window
void RenderTileRow(Window_Minimum windowMin, Tile *srceTile, int tileRowToRenderFrom,
                   int frameRowToRenderTo, WINDOW *destFrame);

// Render tile column to window
void RenderTileColumn(Window_Minimum windowMin, Tile *srceTile, int tileCol,
                      int frameCol, WINDOW *destFrame);

// Render full tile to window and update color pairs
void RenderFullTileUpdatePairs(Window_Minimum windowMin, Tile *sourceTile, WINDOW *displayDest);
// When we need to render window but not reinit color pairs.
void RenderFullTileLeavePairs(Window_Minimum windowMin, Tile *sourceTile, WINDOW *displayDest);

// Refresh window and show new frame
void FlipFrame(WINDOW *frameWindow);


#endif
