#ifndef RP_GAME_RPH
#define RP_GAME_RPH

#include "rp_terminal.h"
#include "rp_user_input.h"
#include "rp_grid_tools.h"
#include "rp_world.h"

typedef struct
{
    int worldX;
    int worldY;
}Camera;

typedef struct
{
    int worldPosX;
    int worldPosY;
    int id;
    EnvironmentEntity *envEntityBuf;
}GameTile;

typedef enum Shift_Direction
{ // Camera movement directions
    Up,
    Down,
    Right,
    Left,
    Direction_Count
}Shift_Dir, Move_Dir;

typedef struct
{
    int worldWidth;
    int worldHeight;
    
    // Linked list of pointers to active tiles
    GameTile *activeTileHead;
    // Temporary for caemera test remove when not needed
    int shiftTestIndex;
    Shift_Dir shiftTestDir;
    
    Camera camera;
    
}Game;

void RenderFrame(WINDOW *cameraWindow);
void InitGame(Game *game);
void UpdateGame(Game *game, double time, double deltaTime, Input input, WindowGroup winGroup);

void DestroyGame(Game *game);

// Transitions camera from source tile to destination tile by some shiftIndex. 
int MoveCameraOneUnit(Shift_Dir direction, int shiftIndex, Camera camera,
                      Window_Minimum windowMin,Tile *srceTile, Tile *destTile,
                      WINDOW *frameWindow);

static int TestCameraTransitions(WINDOW *cameraWindow, Window_Minimum cameraWindowMin, WINDOW *messageWindow, int shiftIndex, Shift_Dir shiftDirection);

int AllocateGameTileBuffer(int width, int height, GameTile *gameTile);
void DestroyGameTileBuffer(GameTile *gameTile);

#endif
