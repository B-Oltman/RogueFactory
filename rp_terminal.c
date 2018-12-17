/**************************************************************************** 
unix_terminal.c contains the code which will need direct access to terminal
this includes color handling, terminal sizing, character plotting, and buffer
display.
*****************************************************************************/
#include <stdlib.h>
#include "rp_terminal.h"
#include "rp_render.h"

// Function to easily dispatch game message to message window
void DispatchMessage(WINDOW *messageWindow, char *message)
{
    mvwprintw(messageWindow, 0, 0, message);
    wrefresh(messageWindow);
}

int InitializeNCurses()
{
    // Enter curses mode
    initscr();
    if(!has_colors())
    {
        // Back to normal mode;
        endwin();
        // Game will work, but not as fun.
        printf("Your terminal doesn't handle custom colors.\n");
        printf("Game will work, but it won't be as fun.\n");
        initscr();
    }
    else
    {
        // Start Color Mode
        start_color();
    }
    
    clear();
    // Make cursor invisible.
    curs_set(0);
    refresh();
    // Don't echo keys back to console.
    noecho();
    // Cursor not used, no need to process it on standard window
    // Leaves cursor wherever it ends up after last update
    leaveok(stdscr, TRUE);
    // Force 8 bit character codes
    meta(stdscr, TRUE);
    // Hook keypad rather than sending escape sequence.
    keypad(stdscr, TRUE);
    
    // Don't let getch() wait for input. Returns ERR if no input is ready.
    nodelay(stdscr, TRUE);
    
    /* NOTE: (baruch) Set up mouse listening events. getch() will return KEY_MOUSE
   whenever one of these mousemask events happens. Then you pass getmouse()which
returns a struct containing mouse position and the state of the mouse when event
was triggered. Must pass getmouse the address of struct a MEVENT *event 
getmouse(event); See rp_user_input.h for usage*/
    mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED |
              BUTTON3_PRESSED | BUTTON3_RELEASED |
              REPORT_MOUSE_POSITION, NULL);
    
    // No delay between clicks
    mouseinterval(0);
    
    // Enable mouse position tracking in terminal
    printf("\033[?1002h\n");
    
    // Don't use standard buffering
    cbreak();
    return 0;
}

void EndNCurses(void)
{
    clear();
    refresh();
    endwin(); //back to normal mode
}

//Attempts to resize terminal, or asks user to do it manually.
//Returns new terminal dimensions.
Term_Data ResizeTerminal(Term_Minimum termMinSize, Term_Data termData)
{
    getmaxyx(stdscr, termData.height, termData.width); //get updated width and height
    if(termData.height < termMinSize.minHeight || termData.width < termMinSize.minWidth)
    {
        //Waiting for user input
        getmaxyx(stdscr, termData.height, termData.width); //update window size
        // While < termMinWidth + 1 because we need 1 column of padding to prevent weird
        // behavior when cells are ploted along edge of terminal.
        while(termData.height < termMinSize.minHeight || termData.width < termMinSize.minWidth + 1)
        {
            ReInitColorPairs();
            int cPair = GetColorPair(White, 74);
            
            erase(); //clear window
            mvprintw(0, 0, "Rogue Pirate requires a terminal size of at least %d x %d", termMinSize.minWidth + 1,
                     termMinSize.minHeight);
            mvprintw(2, 0, "Please resize your terminal now.\n");
            mvprintw(3, 0, "Width: %d/%d", termData.width, termMinSize.minWidth + 1);
            mvprintw(4, 0, "Height: %d/%d", termData.height, termMinSize.minHeight);
            wattron(stdscr, A_BOLD | A_BLINK | COLOR_PAIR(cPair));
            mvprintw(7, 0, "IMPORTANT");
            wattroff(stdscr, A_BLINK);;
            mvprintw(8, 0, "Rogue Pirate will close once the terminal meets minimum size requirements");
            mvprintw(9, 0, "You will need to execute the game again using the corrected terminal size.");
            mvprintw(10, 0, "DON'T change terminal size while game is running. Game must be restarted if this happens.");
            wattroff(stdscr, COLOR_PAIR(cPair) | A_BOLD);
            
            // wait for user action
            getch();
            getmaxyx(stdscr, termData.height, termData.width);
        }
        
        
        //Reset terminal for use.
        EndNCurses();
        exit(1);
    }
    //return updated terminal size
    return termData;
}


int AllocateTileBuffer(int width, int height, Tile *tile)
{
    //if tile buffer exists, free old memory.
    if(tile->tileBuf)
    {
        free(tile->tileBuf);
    }
    
    int cellCount = width * height;
    //Allocate memory for tile buffer and initialize to 0.
    tile->tileBuf = (Cell *)calloc(cellCount, sizeof(Cell));
    
    if(tile->tileBuf == NULL)
    {
        //error allocation failed
        return -1;
    }
    else
    {
        // Clear buffer to spaces, otherwise 0 prints 2 characters which causes 
        // problems when plotting cells.
        FillTileBuffer(width, height, tile, ' ', -1, -1);
        //returns number of cells in buffer;
        return cellCount;
    }
}

WINDOW *CreateNcurseWindow(int newHeight, int newWidth, int startY, int startX)
{
    WINDOW *newWindow;
    newWindow = newwin(newWidth, newHeight, startY, startX);
    
    return newWindow;
}

void ClearNCursesWindow(WINDOW *nCurseWindow)
{
    werase(nCurseWindow);
    wrefresh(nCurseWindow);
}

void UpdateNCursesWindow(WINDOW *nCurseWindow)
{
    wrefresh(nCurseWindow);
}

void DestroyNcurseWindow(WINDOW *nCurseWindow)
{
    ClearNCursesWindow(nCurseWindow);
    delwin(nCurseWindow);
}

void CreateNcurseWindowGroup(WindowGroup *winGroup,
                             int camWinHeight, int camWinWidth, int camWinStrtY,
                             int uIWinHeight, int uIWinWidth, int uIWinStrtY,
                             int messageWinHeight, int messageWinWidth, int messageWinStrtY)
{
    // Fills out window sizes
    winGroup->cameraWinMin.minHeight = camWinHeight;
    winGroup->cameraWinMin.minWidth = camWinWidth;
    winGroup->uIWinMin.minHeight = uIWinHeight;
    winGroup->uIWinMin.minWidth = uIWinWidth;
    winGroup->messageWinMin.minHeight = messageWinHeight;
    winGroup->messageWinMin.minWidth = messageWinWidth;
    
    // Creates windows
    winGroup->cameraWin = CreateNcurseWindow(camWinWidth, camWinHeight, camWinStrtY, 0);
    winGroup->uIWin = CreateNcurseWindow(uIWinWidth, uIWinHeight, uIWinStrtY, 0);
    winGroup->messageWin = CreateNcurseWindow(messageWinWidth, messageWinHeight, messageWinStrtY, 0);
}

void DestroyNcurseWindowGroup(WindowGroup *winGroup)
{
    DestroyNcurseWindow(winGroup->cameraWin);
    DestroyNcurseWindow(winGroup->uIWin);
    DestroyNcurseWindow(winGroup->messageWin);
}
// A tile is group of cells, along with data about world position and size. Used to construct a
// window. A Tile should always point to the memory of its cell buffer.
void DestroyTile(Tile *tile)
{
    free(tile->tileBuf);
}
