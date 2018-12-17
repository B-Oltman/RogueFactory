#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "rp_game.h"
#include "rp_render.h"
#include "rp_ui.h"
#include "rp_world.h"
#include "rp_editor.h"
#include "rp_globals.h"
#include "rp_io.h"

#define MINIMUM_CONSOLE_HEIGHT 45
#define MINIMUM_CONSOLE_WIDTH 155

#define MESSAGE_FIELD_HEIGHT 4
#define INTERFACE_HEIGHT 6

#define CAMERA_WIDTH MINIMUM_CONSOLE_WIDTH
#define CAMERA_HEIGHT MINIMUM_CONSOLE_HEIGHT - (INTERFACE_HEIGHT + MESSAGE_FIELD_HEIGHT)

#define UIWINDOW_START_POS CAMERA_HEIGHT + MESSAGE_FIELD_HEIGHT
#define MESSAGEWINDOW_START_POS CAMERA_HEIGHT

/*****************************************************
This is where the main loops and persistent memory happens.
this will be cleaned up as development progresses and structures
are fleshed out.
******************************************************/


typedef struct
{
    int choiceCount;
    int choiceId[MenuChoiceCountPlusOne - 1];
    char choiceString[MenuChoiceCountPlusOne - 1][RP_MAX_MENU_NAME_LENGTH];
}MenuItems;

// Prepares main menu for display
State CreateMainMenu(WINDOW *uIWindow, Window_Minimum uIWindowMin, Tile *uITile, WINDOW *messageWindow);

int main(int argc, char *argv[])
{
    // Will be used to seed random num generator.
    int randomSeed;
    if(argc == 2)
    {
        int err = scanf(argv[1], "%d", &randomSeed);
    }
    
    const Term_Minimum termMin = {MINIMUM_CONSOLE_HEIGHT, MINIMUM_CONSOLE_WIDTH};
    
    // Set active child process TERM to xterm-256color so the program knows it can
    // display proper color mode.
    putenv("TERM=xterm-256color");
    
    if(InitializeNCurses() != 0)
    {
        // TODO(baruch): Set flag to run in no-color mode
    }
    
    // Create data directory if not exist
    int err = MakeRpDirectory("data");
    
    // Create windows
    WindowGroup winGroup;
    CreateNcurseWindowGroup(&winGroup, CAMERA_HEIGHT, CAMERA_WIDTH, 0,INTERFACE_HEIGHT, MINIMUM_CONSOLE_WIDTH, UIWINDOW_START_POS, MESSAGE_FIELD_HEIGHT, MINIMUM_CONSOLE_WIDTH, MESSAGEWINDOW_START_POS);
    
    // Ensure terminal is at the correct size to play Rogue Pirate
    Term_Data termData;
    termData = ResizeTerminal(termMin, termData);
    
    // Shared by game and editor so initialized outside state.
    Tile uITile = {0};
    int uIBufferSize = AllocateTileBuffer(winGroup.uIWinMin.minWidth,
                                          winGroup.uIWinMin.minHeight, &uITile);
    
    Tile bigUiTile = {0};
    int bigUiBufferSize = AllocateTileBuffer(winGroup.cameraWinMin.minWidth,
                                             winGroup.cameraWinMin.minHeight, &bigUiTile);
    
    // Create container to hold input data (mouse and keyboard events).
    // Shared across all states.
    Input inputContainer;
    
    // Game struct to store state of current game. Including pointers to game memory
    Game game;
    
    int quit = 0;
    int quitToMainMenu;
    State state;
    
    while(!quit)
    {
        // Clear camera window before creating main menu, just aesthetic thing
        ClearNCursesWindow(winGroup.cameraWin);
        state = CreateMainMenu(winGroup.uIWin, winGroup.uIWinMin, &uITile, winGroup.messageWin);
        
        // Loop control variable exits to main menu if set to 1.
        quitToMainMenu = 0;
        if(state == NewGame || state == ContinueGame)
        {
            if(state == NewGame)
            {
                InitGame(&game);
            }
            
            inputContainer.triggerFlags = 0;
            
            // Create game key contexts
            InputDefinition gameInputDefs[GameInputFlagCount];
            DefineKeyContext(state, gameInputDefs);
            
            // Frame clock variables. Always updating physics state in discrete dt steps.
            // This method learned from Glenn Fiedler https://gafferongames.com/post/fix_your_timestep/
            double t = 0.0;
            double dt = 0.01;
            
            double accumulator = 0.0;
            double currentTime = clock();
            
            // ********************Enter game loop************************
            while(!quitToMainMenu)
            {
                // Bitwise test to see if the Pause event was activated
                if(FlagIsInMask(inputContainer.triggerFlags, Pause))
                {
                    quitToMainMenu = 1;
                }
                
                double newTime = clock();
                double frameTime = (newTime - currentTime) / CLOCKS_PER_SEC;
                if(frameTime > 0.25)
                {
                    frameTime = 0.25;
                }
                currentTime = newTime;
                
                accumulator += frameTime;
                
                while(accumulator >= dt)
                {
                    GetInputEvents(&inputContainer, gameInputDefs, GameInputFlagCount);
                    
                    // TODO(baruch): Previous Physics State = Current Physics State (positions)
                    // TODO(baruch): Integrate new physics state
                    UpdateGame(&game, t, dt, inputContainer, winGroup);
                    
                    t += dt;
                    accumulator -= dt;
                }
                
                const double alpha = accumulator / dt;
                
                // TODO(baruch): Interpolate towards new physics state
                // state = currentState * alpha + previousState * (1.0 - alpha);
                
                // Render Game
                RenderFrame(winGroup.cameraWin);
            }
        }
        else if(state == WorldEditor)
        {
            Editor editor;
            InitEditor(&editor, winGroup, &uITile, &bigUiTile);
            
            editor.editorState = EditorInitialization;
            CanvasToolData canvasToolData;
            
            CreateCanvasTools(&canvasToolData, UIWINDOW_START_POS, winGroup, &editor);
            
            // Is canvas saved?
            canvasToolData.canvasSavedFlag = 1;
            
            // Main editor loop
            while(editor.editorState != QuitToMain)
            {
                UpdateEditor(&editor, &canvasToolData, &inputContainer, winGroup, &bigUiTile, &uITile);
            }
            
            // Prompt user to save world
            if(!canvasToolData.canvasSavedFlag)
            {
                // Clear stale information from message window
                ClearNCursesWindow(winGroup.messageWin);
                
                char saveOptions[][RP_MAX_MENU_NAME_LENGTH] = 
                {{"Save World"}, {"Discard Changes"}};
                int saveOptionIds[] = {1, 2};
                int optionCount = sizeof(saveOptionIds) / sizeof(int);
                DisplayMenuUI(winGroup.uIWinMin, winGroup.uIWin, &uITile, saveOptions,
                              saveOptionIds, optionCount);
                int saveChoice = ChooseNumericMenu(saveOptionIds, optionCount);
                
                if(saveChoice == 1)
                {
                    // If user wants to save, write the current world to disk in a .rpw file
                    mvwprintw(winGroup.messageWin, 0, 0, "Saving world, please wait...");
                    FlipFrame(winGroup.messageWin);
                    WriteWorldFile(&editor);
                    ClearNCursesWindow(winGroup.messageWin);
                    quitToMainMenu = 1;
                }
                else
                {
                    // If user chooses to discard, quit to main without saving
                    quitToMainMenu = 1;
                }
                
                // Clean up world and editor memory
                canvasToolData.canvasSavedFlag = 1;
                DestroyWorld(editor.world, &editor.worldInfo);
                EndWorldEditor(canvasToolData);
            }
            
        }
        else if(state == QuitGame)
        {
            DestroyGame(&game);
            quit = 1;
        }
        
    }
    
    // Clean up windows and ui buffers
    DestroyTile(&uITile);
    DestroyTile(&bigUiTile);
    DestroyNcurseWindow(stdscr);
    DestroyNcurseWindowGroup(&winGroup);
    EndNCurses();
    return 0;
}

State CreateMainMenu(WINDOW *uIWindow, Window_Minimum
                     uIWindowMin, Tile *uITile, WINDOW *messageWindow)
{
    // Flush input buffer after menu called
    ClearNCursesWindow(messageWindow);
    ClearNCursesWindow(uIWindow);
    
    //Waiting for user input
    nodelay(uIWindow, FALSE);
    
    MenuItems menuItems = 
    {
        // Fill struct with menu items
        MenuChoiceCountPlusOne - 1,
        {NewGame, SaveGame, ContinueGame, LoadGame, WorldEditor, QuitGame},
        {{"Run Camera Transition Test"}, {"Save Game (Under Dev)"}, {"Return to Current Game (Under Dev)"},
            {"Load Saved Game (Under Dev)"}, {"Enter World Editor"}, {"Quit Rogue Pirate"}}
        
        /*MenuChoiceCountPlusOne - 1,
        {NewGame, SaveGame, ContinueGame, LoadGame, WorldEditor, QuitGame},
        {{"Start New Game ("}, {"Save Game"}, {"Return to Current Game"},
            {"Load Saved Game"}, {"Enter World Editor"}, {"Quit Rogue Pirate"}}*/
    };
    
    // Display menu items with proper format
    DisplayMenuUI(uIWindowMin, uIWindow, uITile, menuItems.choiceString,
                  menuItems.choiceId, menuItems.choiceCount);
    
    int ch;
    int choiceValid = 0;
    State state;
    while(!choiceValid)
    {
        mvwprintw(messageWindow, 0, 0, "Please make a selection by entering its number on the keyboard. For CSC250 Class Project, only some options have been finished.");
        mvwprintw(messageWindow, 1, 0, "Please select either (1) To test Camera Transitions, (5) To Run World Editor, or (6) To Quit, Other options are under development");
        FlipFrame(messageWindow);
        
        // Get character from user.
        ch = wgetch(uIWindow);
        for(int i = 1; i <= menuItems.choiceCount; ++i)
        {
            // If pressed key is a valid menu option, set valid choice true.
            if(ch == '0' + i)
            {
                // Clear input buffer
                state = i;
                
                if(state == NewGame || state == WorldEditor || state == QuitGame)
                {
                    choiceValid = 1;
                }
                else
                {
                    mvwprintw(messageWindow, 3, 0, "That option is under development and not yet available, please choose from one of the options noted above.");
                    wrefresh(messageWindow);
                }
            }
        }
    }
    
    //Reset terminal for use.
    nodelay(uIWindow, TRUE);
    
    ClearNCursesWindow(uIWindow);
    ClearNCursesWindow(messageWindow);
    
    return state;
}


