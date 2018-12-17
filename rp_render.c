#include "rp_render.h"
#include "rp_grid_tools.h"

int PlotCell(int character, int x, int y, Color fore, Color back,
             Tile *tile, Window_Minimum windowMin)
{
    // Simply ignore call to plot if outside bounds of terminal,
    // this prevents buffer overflow crashes.
    if(x < windowMin.minWidth && x >= 0 && y < windowMin.minHeight && y >= 0)
    {
        // Convert cartesian coordinate to array buffer index. Works because first row is 0
        int cartesianToBuf = (y * windowMin.minWidth) + x;
        
        tile->tileBuf[cartesianToBuf].character = character;
        tile->tileBuf[cartesianToBuf].fore = fore;
        tile->tileBuf[cartesianToBuf].back = back;
    }
    else
    {
        // Tried to plot outside bounds of screen.
        return -1;
    }
    
    return 0;
}

void RenderTileRow(Window_Minimum windowMin, Tile *srceTile, int tileRowToRenderFrom,
                   int frameRowToRenderTo, WINDOW *destFrame)
{
    for(int x = 0; x < windowMin.minWidth; ++x)
    {
        int readIndex = (tileRowToRenderFrom == 0) ? x :
        ( windowMin.minWidth * tileRowToRenderFrom) - windowMin.minWidth + x;
        
        Color fore = srceTile->tileBuf[readIndex].fore;
        Color back = srceTile->tileBuf[readIndex].back;
        
        int cPair = GetColorPair(fore, back);
        
        // Activate color pair
        wattron(destFrame, COLOR_PAIR(cPair));
        // Write cell data to frame
        mvwaddch(destFrame, frameRowToRenderTo, x, srceTile->tileBuf[readIndex].character);
        wattroff(destFrame, COLOR_PAIR(cPair));
    }
}

void RenderTileColumn(Window_Minimum windowMin, Tile *srceTile, int tileCol,
                      int frameCol, WINDOW *destFrame)
{
    for(int y = 0; y < windowMin.minHeight; ++y)
    {
        int readIndex = tileCol + (y * windowMin.minWidth);
        Color fore = srceTile->tileBuf[readIndex].fore;
        Color back = srceTile->tileBuf[readIndex].back;
        
        int cPair = GetColorPair(fore, back);
        
        wattron(destFrame, COLOR_PAIR(cPair));
        mvwaddch(destFrame, y, frameCol, srceTile->tileBuf[readIndex].character);
        wattroff(destFrame, COLOR_PAIR(cPair));
    }
}

void RenderFullTileUpdatePairs(Window_Minimum windowMin, Tile *sourceTile, WINDOW *displayDest)
{
    ReInitColorPairs();
    RenderFullTileLeavePairs(windowMin, sourceTile, displayDest);
}

void RenderFullTileLeavePairs(Window_Minimum windowMin, Tile *sourceTile, WINDOW *displayDest)
{
    for(int row = 0, i = 0; row < windowMin.minHeight; ++row)
    {
        wmove(displayDest, row,0);
        for(int col = 0; col < windowMin.minWidth; ++col)
        {
            Color fore = sourceTile->tileBuf[i].fore;
            Color back = sourceTile->tileBuf[i].back;
            
            int cPair = GetColorPair(fore, back);
            wattron(displayDest, COLOR_PAIR(cPair));
            waddch(displayDest, sourceTile->tileBuf[i].character);
            wattroff(displayDest, COLOR_PAIR(cPair));
            ++i;
        }
    }
    // Return cursor to default position
    wmove(displayDest, 0, 0);
}

void FillTileBuffer(int tileWidth, int tileHeight, Tile *tile, char fillChar,
                    Color fore, Color back )
{
    int cellCount = tileWidth * tileHeight;
    for(int i = 0; i < cellCount; ++i)
    {
        tile->tileBuf[i].character = fillChar;
        tile->tileBuf[i].fore = fore;
        tile->tileBuf[i].back = back;
    }
}

// Wrapper to handel frame update
void FlipFrame(WINDOW *frameWindow)
{
    UpdateNCursesWindow(frameWindow);
}