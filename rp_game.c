#include <stdlib.h>
#include "rp_game.h"
#include "rp_render.h"
#include "rp_ui.h"
#include "rp_world.h"

void RenderFrame(WINDOW *cameraWindow)
{
    FlipFrame(cameraWindow);
}

void InitGame(Game *game)
{
    ReInitColorPairs();
    game->shiftTestIndex = 0;
    game->shiftTestDir = Left;
}

void DestroyGame(Game *game)
{
    // TODO(baruch): To do, implement
}

// Run test code
void UpdateGame(Game *game, double time, double deltaTime, Input input, WindowGroup winGroup)
{
    if(FlagIsInMask(input.triggerFlags, MoveLeft))
    {
        game->shiftTestDir = Left;
        game->shiftTestIndex = 0;
    }
    else if(FlagIsInMask(input.triggerFlags, MoveRight))
    {
        game->shiftTestDir = Right;
        game->shiftTestIndex = 0;
    }
    else if(FlagIsInMask(input.triggerFlags, MoveUp))
    {
        game->shiftTestDir = Up;
        game->shiftTestIndex = 0;
    }
    else if(FlagIsInMask(input.triggerFlags, MoveDown))
    {
        game->shiftTestDir = Down;
        game->shiftTestIndex = 0;
    }
    
    game->shiftTestIndex = 
        TestCameraTransitions(winGroup.cameraWin, winGroup.cameraWinMin, winGroup.messageWin, game->shiftTestIndex, game->shiftTestDir);
    game->shiftTestIndex += 100 * deltaTime;
}

int AllocateGameTileBuffer(int width, int height, GameTile *gameTile)
{
    //if tile buffer exists, free old memory.
    if(gameTile->envEntityBuf)
    {
        free(gameTile->envEntityBuf);
    }
    
    int entityCount = width * height;
    //Allocate memory for tile buffer and initialize to 0.
    gameTile->envEntityBuf = (EnvironmentEntity *)calloc(entityCount, sizeof(EnvironmentEntity));
    
    if(gameTile->envEntityBuf == NULL)
    {
        //error allocation failed
        return -1;
    }
    else
    {
        //returns number of cells in buffer;
        return entityCount;
    }
}

void DestroyGameTileBuffer(GameTile *gameTile)
{
    if(gameTile->envEntityBuf)
    {
        free(gameTile->envEntityBuf);
    }
}

// This will transition the camera by a single unit in any cardinal direction
// frame to the next. Used when player moves beyond or approaches edge of screen.
// shiftIndex is the position from the end of the completed shift, shift while this
// value is either maxWidth or maxHeight depending on if shift is horizontal or 
// vertical.
int MoveCameraOneUnit(Shift_Dir direction, int shiftIndex, Camera camera,
                      Window_Minimum windowMin, Tile *srceTile,
                      Tile *destTile, WINDOW *frameWindow)
{
    // TODO(baruch): Also update camera world position to match new tile here.
    
    int finalRow = windowMin.minHeight;
    int finalCol = windowMin.minWidth;
    
    // NOTE(baruch): Need to RebuildColorPairs() once before transitions.
    
    // Transition directions depend on camera movement direction.
    if(direction == Up)
    {
        // TODO(baruch): Need to handle ease in and ease out, this will be done using
        // the Rest function. Make transition feel snappy, maybe support jitter for added FX.
        
        // Number of times to run tile transition step, (height of terminal times)
        // Fill first section of frame. From first row to shiftIndex.
        int destTileRow = finalRow - shiftIndex;
        for(int activeFrameRow = 0; activeFrameRow <= shiftIndex; ++activeFrameRow)
        {
            // Render each row of tiles to frame.
            RenderTileRow(windowMin, destTile, destTileRow + activeFrameRow, activeFrameRow, frameWindow);
        }
        
        // Fill second section of frame. From row after shiftIndex to last row of frame
        int srceTileRow = 1;
        for(int activeFrameRow = shiftIndex + 1; activeFrameRow < finalRow; ++activeFrameRow)
        {
            RenderTileRow(windowMin, srceTile, srceTileRow++, activeFrameRow, frameWindow);
        }
    }
    else if(direction == Down)
    {
        for(int activeFrameRow = 0; activeFrameRow <= finalRow - shiftIndex; ++activeFrameRow)
        {
            int srceTileRow = shiftIndex + activeFrameRow;
            RenderTileRow(windowMin, srceTile, srceTileRow + 1, activeFrameRow - 1, frameWindow);
        }
        
        int destTileRow = 0;
        for(int activeFrameRow = finalRow - shiftIndex - 1; activeFrameRow < finalRow; ++activeFrameRow)
        {
            RenderTileRow(windowMin, destTile, ++destTileRow, activeFrameRow, frameWindow);
        }
    }
    else if(direction == Right)
    {
        for(int activeFrameCol = 0; activeFrameCol < finalCol - shiftIndex; ++activeFrameCol)
        {
            int srceTileCol = shiftIndex + activeFrameCol;
            RenderTileColumn(windowMin, srceTile, srceTileCol, activeFrameCol - 1, frameWindow);
        }
        
        int destTileCol = 0;
        for(int activeFrameCol = finalCol - shiftIndex - 1; activeFrameCol < finalCol; ++activeFrameCol)
        {
            RenderTileColumn(windowMin, destTile, destTileCol++, activeFrameCol, frameWindow);
        }
    }
    else if(direction == Left)
    {
        int destTileCol = finalCol - shiftIndex - 1;
        for(int activeFrameCol = 0; activeFrameCol <= shiftIndex; ++activeFrameCol)
        {
            RenderTileColumn(windowMin, destTile, destTileCol++, activeFrameCol, frameWindow);
        }
        
        int srceTileCol = 0;
        for(int activeFrameCol = shiftIndex + 1; activeFrameCol < finalCol; ++activeFrameCol)
        {
            RenderTileColumn(windowMin, srceTile, srceTileCol++, activeFrameCol, frameWindow);
        }
    }
    
    // Return shift index if shifting not complete
    if(direction == Up || direction == Down)
    {
        if(shiftIndex < windowMin.minHeight)
        {
            return shiftIndex;
        }
        else
        {
            return 0;
        }
    }
    else if(direction == Left || direction == Right)
    {
        if(shiftIndex < windowMin.minWidth)
        {
            return shiftIndex;
        }
        else
        {
            return 0;
        }
    }
}

int TestCameraTransitions(WINDOW *cameraWindow, Window_Minimum cameraWindowMin, WINDOW *messageWindow, int shiftIndex, Shift_Dir shiftDirection)
{
    /***********************************************************
    CAMERA TRANSITION DEBUG TEST CODE
    ************************************************************/
    // Initialize array of world tiles. Tile buffer to be allocated as needed
    // tile data will be streamed from a file as the player approaches its world position
    // the tile buffer will only be created when a tile loads its data from file.
    
    mvwprintw(messageWindow, 0, 0, "Running Camera Transition Test: Use the arrow keys to change camera direction.");
    mvwprintw(messageWindow, 2, 0, "Game loop not fully integrated, ignore speed changes when changing from left/right to up/down. This will be fixed once game development begins.");
    mvwprintw(messageWindow, 3, 0, "Press (p) to return to main menu");
    wrefresh(messageWindow);
    
    Tile worldTile[2][2] = {0};
    
    int buffer1Size = AllocateTileBuffer(cameraWindowMin.minWidth, cameraWindowMin.minHeight,
                                         &worldTile[0][0]);
    int buffer2Size = AllocateTileBuffer(cameraWindowMin.minWidth, cameraWindowMin.minHeight,
                                         &worldTile[0][1]);
    
    FillTileBuffer(cameraWindowMin.minWidth, cameraWindowMin.minHeight, &worldTile[0][0], '~', SteelBlue, Plum4);
    FillTileBuffer(cameraWindowMin.minWidth, cameraWindowMin.minHeight, &worldTile[0][1], '.', Silver, SpringGreen4);
    
    int foreColor1 = White;
    int foreColor2 = White;
    int backColor1 = Black;
    int backColor2 = Black;
    
    int skip = 0;
    for(int y = 0; y < cameraWindowMin.minHeight; ++y)
    {
        if(skip % 2 == 0)
        {
            for(int x = 0; x < cameraWindowMin.minWidth ; ++x)
            {
                PlotCell('^', cameraWindowMin.minWidth - 1 - x, y + x, foreColor1, backColor1 + (x % 200), &worldTile[0][0], cameraWindowMin);
                PlotCell('#', x, y + x, foreColor2, backColor2 + (x % 200), &worldTile[0][1], cameraWindowMin);
                PlotCell('^', cameraWindowMin.minWidth - 1 - x, y + x, foreColor2, backColor2 + (x % 200), &worldTile[0][1], cameraWindowMin);
            }
        }
        ++skip;
    }
    
    Camera camera = {0};
    int nextShift = MoveCameraOneUnit(shiftDirection, shiftIndex, camera, cameraWindowMin, &worldTile[0][0], &worldTile[0][1], cameraWindow);
    
    // Clean up world tiles
    for(int y = 0; y < 2; ++y)
    {
        for(int x = 0; x < 2; ++x)
        {
            DestroyTile(&worldTile[y][x]);
        }
    }
    
    return nextShift;
}