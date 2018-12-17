/*
rp_interface will handle the interface state and formatting. There will be at least
two modes. World-Editor and Game mode.
*/
#ifndef RP_INTERFACE_RPH
#define RP_INTERFACE_RPH

#include "rp_user_input.h"
#include "rp_terminal.h"
#include "rp_strings.h"
#include "rp_globals.h"

typedef enum
{
    UIWorldEditor,
    UIMonsterEditor,
    UIItemEditor,
    UIGame,
    UIMainMenu,
    UIModeCount
}Interface_Mode;

/**************SLOT TOOL STRUCTURES**************/
typedef enum
{
    ClickToShow,
    AlwaysShow,
}Label_Type;

// A slotting info struct must be included in any type that is to use the slot system.
typedef struct
{
    int iD;
    Label_Type labelType;
    char label[RP_MAX_LABEL_LENGTH];
}SlottingInfo;

typedef struct
{
    int slotX;
    int slotY;
    int slotWidth;
    SlottingInfo slotInfo;
    int active;
    int occupied;
    Cell defaultDisplay;
}EditorUiSlot;

typedef struct
{
    int width;
    int height;
    WINDOW parentContainer;
    Window_Minimum parentContainerSize;
    int yStartPosInTerminal;
    int windowStartX;
    int windowStartY;
    Tile *displayBuffer;
    EditorUiSlot *slots;
    int slotCount;
}SlotSection;

/******UI ORGANIZATION AND SLOTTING TOOLS******/

// Creates a sub window within the uI window. It holds a group of similar tools
void CreateSlotSection(WINDOW uIWindow, Window_Minimum uIMin,
                       Tile *displayBuffer, int width, int height,
                       SlotSection *slotSection, int startX, int startY);

// Adds a tool to the slot, any tool that will be slotted needs an integer tool id.
// Collisions of tool ids aren't handled within a slot section, so any tools that could
// have matching iDs need to be put in different slot sections for now.
void AddToolToSlot(SlotSection *slotSection, int slotIndex, SlottingInfo slotInfo);

// Activates selected slot, deactivates all other slots in that slot section.
int ToggleSlotsGetOccupant(SlotSection *slotSection, Input *inputContainer, int activeColor);

// Clean up slot by freeing their buffers.
void DestroySlotSection(SlotSection slotSection);

// Display a menu in a given window, uses an array of strings, and an array of corresponding option ids
void DisplayMenuUI(Window_Minimum interFaceSize, WINDOW *uIWindow, Tile *uITile, char menuOptions[][RP_MAX_MENU_NAME_LENGTH],
                   int menuOptionId[], int menuOptionsCount);
// Allows user to select from the list of options which was used to construct the menu in DisplayMenuUi()
int ChooseNumericMenu(int optionIDs[], int optionsCount);

/***************MESSAGES***********/
void PromptForString(char *promptMessage, char *stringBuffer, int maxEntryLength, WINDOW *displayWindow);
void ClearMessageLine(WINDOW *messageWindow, int lineNumber, int startPosForClear);
void ClearMessageLineAndPrint(WINDOW *messageWindow, int lineNumber, int startPosForClear, char *message);


/***********MODULE SPECIFIC UI**************/
// Gets zones from rp_world module and returns selected zoneId.
int DisplayZoneSelectMenu(WINDOW *uIWindow, Window_Minimum uIWindowMin, Tile *uITile, WINDOW *messageWindow);





#endif

