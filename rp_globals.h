/*
For tool functions that could be used everywhere, similar to the string library.
*/

#ifndef RP_GLOBALS_RPH
#define RP_GLOBALS_RPH

typedef enum
{
    NewGame = 1,
    SaveGame,
    ContinueGame,
    LoadGame,
    WorldEditor,
    QuitGame,
    MenuChoiceCountPlusOne
}State;

// Sets flag in mask, returns mask
int SetFlag(int mask, int flag);

// Checks to see if flag is set in a bit mask, returns true if it is
int FlagIsInMask(int mask, int flag);

#endif

