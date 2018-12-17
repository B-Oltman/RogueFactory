#ifndef RP_USER_INPUT_RPH
#define RP_USER_INPUT_RPH

#include <ncurses.h>
#include "rp_strings.h"
#include "rp_globals.h"

// The bit mask options for available events in the game.
typedef enum
{
    // Must be less than 32, using as integer bit mask
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    ActionInteract,
    ActionAttack,
    Inventory,
    Pause,
    GameInputFlagCount
}Game_Input_Flags;

typedef enum
{
    // Must be less than 32, using as integer bit mask
    QuitEditor,
    OpenWarpWindow,
    GoBack,
    Accept,
    EditorInputFlagCount
}Editor_Input_Flags;

// Used to define key actions so that they can be changed depending on context 
// (game vs editor.)
typedef struct
{
    char actionLabel[RP_MAX_LABEL_LENGTH];
    int flagIndex;
    int keyDefinition;
}InputDefinition;

/* NOTE(baruch): To handle input, we will have a function that get's user input in
 rp_main.c each frame. The input will be stored in a struct and passed to game update
 and render.*/
typedef struct
{
    int triggerFlags;
    int mouseX;
    int mouseY;
    MEVENT mouseEvent;
}Input;

void DefineKeyContext(State state, InputDefinition inputDefs[]);
void GetInputEvents(Input *input, InputDefinition inputDefinitions[], int inputDefCount);

#endif