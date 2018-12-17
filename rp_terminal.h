/*
Unix_terminal is where all of the terminal specific code will live. 
*/ 

#ifndef UNIX_TERMINAL_RPH
#define UNIX_TERMINAL_RPH
#include <ncurses.h>
#include "rp_colors.h"

typedef struct def_window_data
{
    int height;
    int width;
}Term_Data, Window_Data;

//A cell is the basic display unit of RP can be treated like a pixel.
typedef struct def_cell
{
    // TODO(baruch): change to char instead of int?
    int character;
    Color fore;
    Color back;
}Cell;

typedef struct def_tile
{
    Cell *tileBuf;
}Tile;

//Terminal/window minimum size requirements.
typedef struct def_terminal_minimum_size
{
    int minHeight;
    int minWidth;
}Term_Minimum, Window_Minimum;

// Can be used as more convenient way to pass all windows to functions
typedef struct
{
    WINDOW *cameraWin;
    WINDOW *uIWin;
    WINDOW *messageWin;
    Window_Minimum cameraWinMin;
    Window_Minimum uIWinMin;
    Window_Minimum messageWinMin;
    
}WindowGroup;

void DispatchMessage(WINDOW *messageWindow, char *message);

int InitializeNCurses(void);
void EndNCurses(void);

//Ensure terminal is proper size for game to display
Term_Data ResizeTerminal(Term_Minimum  minRequired, Term_Data terminalData);

//Functions for managing ncurses windows and state
WINDOW *CreateNcurseWindow(int newWidth, int newHeight, int startY, int startX);
void ClearNCursesWindow(WINDOW* nCurseWindow);
void UpdateNCursesWindow(WINDOW *nCurseWindow);
void DestroyNcurseWindow(WINDOW *nCurseWindow);

void CreateNcurseWindowGroup(WindowGroup *winGroup,
                             int camWinHeight, int camWinWidth, int camWinStrtY,
                             int uIWinHeight, int uIWinWidth, int uIWinStrtY,
                             int messageWinHeight, int messageWinWidth, int messageWinStrtY);
void DestroyNcurseWindowGroup(WindowGroup *winGroup);

//Functions for managing tile memory
int AllocateTileBuffer(int width, int height, Tile *tile);
void DestroyTile(Tile *tile);


#endif