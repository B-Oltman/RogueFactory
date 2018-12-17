#include <stdlib.h>
#include "rp_ui.h"
#include "rp_editor.h"
#include "rp_render.h"
#include "rp_strings.h"

void DisplayMenuUI(Window_Minimum interFaceSize, WINDOW *uIWindow,
                   Tile *uITile, char menuOptions[][RP_MAX_MENU_NAME_LENGTH],
                   int menuOptionId[], int menuOptionsCount)
{
    // TODO(baruch): Replace print options with buttons and rendering
    // TODO(baruch): Need to come up with a way to pad so that items line up,
    // likely won't be working on this because the plan is to add buttons anyways.
    
    int spaceBtwnItems = 7;
    int rowWidth = 0;
    int rowNumber = 0;
    int lastItemLength = 0;
    int colNumber = 0;
    for(int i = 0; i < menuOptionsCount; ++i)
    {
        // Store length of menu item.
        lastItemLength = RpStrLength(menuOptions[i]);
        
        // Keep track of cursor position on row
        rowWidth += lastItemLength + spaceBtwnItems;
        if(rowWidth > interFaceSize.minWidth - (2 * spaceBtwnItems))
        {
            // Move to next row if width of printed items would write beyond
            // edge of window.
            rowNumber++;
            rowWidth = 0;
            colNumber = 0;
            if(rowNumber > interFaceSize.minHeight)
            {
                // Menu items exceeded space available, handle second page
                // likely will never get here. Simply break for now
                break;
            }
        }
        
        // Print at position of cursor
        mvwprintw(uIWindow, rowNumber, colNumber, "%d. %s",menuOptionId[i], menuOptions[i]);
        
        // Move cursor to end of last item string + space between items.
        colNumber += lastItemLength + spaceBtwnItems;
    }
    FlipFrame(uIWindow);
}

static void PrepareSlotsForUse(SlotSection *slotSection)
{
    for(int y = 0, i = 0; y < slotSection->height; ++y)
    {
        for(int x = 0; x < slotSection->width; ++x)
        {
            slotSection->slots[i].slotY = slotSection->windowStartY + y +
                slotSection->yStartPosInTerminal;
            slotSection->slots[i].slotX = slotSection->windowStartX + x;
            slotSection->slots[i].active = 0;
            slotSection->slots[i].occupied = 0;
            
            ++i;
        }
    }
}

void CreateSlotSection(WINDOW uIWindow, Window_Minimum uIMin,
                       Tile *displayBuffer, int width, int height,
                       SlotSection *slotSection, int startX, int startY)
{
    slotSection->width = width;
    slotSection->height = height;
    slotSection->slotCount = width * height;
    slotSection->parentContainer = uIWindow;
    slotSection->parentContainerSize = uIMin;
    slotSection->displayBuffer = displayBuffer;
    slotSection->windowStartX = startX;
    slotSection->windowStartY = startY;
    
    slotSection->slots = (EditorUiSlot *)calloc((width * height), sizeof(EditorUiSlot));
    
    PrepareSlotsForUse(slotSection);
}

void PromptForString(char *promptMessage, char *stringBuffer, int maxEntryLength, WINDOW *displayWindow)
{
    mvwprintw(displayWindow, 0, 0, "%s", promptMessage);
    mvwprintw(displayWindow, 1, 0, "Press Enter when finished and use Backspace if you make a mistake.");
    mvwprintw(displayWindow, 2, 0, "Special characters will be ignored");
    FlipFrame(displayWindow);
    
    // Don't continue execution until character is entered.
    nodelay(stdscr, FALSE);
    
    int ch;
    int i = 0;
    while(1)
    {
        // Get character and check that it's writable
        ch = getch();
        if((ch >= 'A' && ch <= 'Z')||
           ch == ' ' ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= '0' && ch <= '9'))
        {
            // Convert spaces to '_'
            if(ch == ' ')
            {
                ch = '_';
            }
            
            // Write character to screen and to buffer
            stringBuffer[i] = ch;
            mvwaddch(displayWindow, 3, i, ch);
            FlipFrame(displayWindow);
            
            ++i;
        }
        // if return is pressed
        if(ch == '\n')
        {
            // End string and break from loop.
            if(i != 0)
            {
                stringBuffer[i] = '\0';
                break;
            }
            // If user didn't enter a character, prompt them and continue loop
            else
            {
                mvwprintw(displayWindow, 2, 0, "You must enter at least one character");
                FlipFrame(displayWindow);
            }
        }
        // Clear characters when backspace pressed.
        if(ch == KEY_BACKSPACE && i > 0)
        {
            
            --i;
            // Clear character
            ClearMessageLine(displayWindow, 3, i);
            FlipFrame(displayWindow);
        }
    }
    
    
    stringBuffer[maxEntryLength] = '\0';
    ClearNCursesWindow(displayWindow);
    mvwprintw(displayWindow, 1, 0, "Welcome to: \"%s\" You must remember this name to load your world in the future.", stringBuffer);
    mvwprintw(displayWindow, 3, 0, "Press any key to continue.");
    FlipFrame(displayWindow);
    
    getch();
    
    nodelay(stdscr, TRUE);
}

void ClearMessageLineAndPrint(WINDOW *messageWindow, int lineNumber, int startPosForClear, char *message)
{
    ClearMessageLine(messageWindow, lineNumber, startPosForClear);
    mvwprintw(messageWindow, lineNumber, startPosForClear, "%s", message);
    FlipFrame(messageWindow);
}
void ClearMessageLine(WINDOW *messageWindow, int lineNumber, int startPosForClear)
{
    wmove(messageWindow, lineNumber, startPosForClear);
    wclrtoeol(messageWindow);
}

// Iterates slot section looking for open space. Adds tool if space found
void AddToolToSlot(SlotSection *slotSection, int slotIndex, SlottingInfo slotInfo)
{
    if(slotSection->slots[slotIndex].occupied  == 0)
    {
        slotSection->slots[slotIndex].occupied  = 1;
        slotSection->slots[slotIndex].slotInfo.iD = slotInfo.iD;
        slotSection->slots[slotIndex].slotInfo.labelType = slotInfo.labelType;
        
        if(slotInfo.labelType == AlwaysShow)
        {
            slotSection->slots[slotIndex].slotWidth = RP_MAX_LABEL_LENGTH;
        }
        else
        {
            slotSection->slots[slotIndex].slotWidth = 1;
        }
    }
}

// Helper function for toggle slots
static void HighlightActiveSlotsToggle(SlotSection *slotSection, int activeColor, int selectionChar)
{
    // Iterate entire slotSection
    for(int y = 0, i = 0; y < slotSection->height; ++y)
    {
        for(int x = 0; x < slotSection->width; ++x)
        {
            if(slotSection->slots[i].occupied)
            {
                if(slotSection->slots[i].active)
                {
                    // If slot is active and occupied, change background color and character to those
                    // set as for active display
                    wclear(&slotSection->parentContainer);
                    PlotCell(selectionChar, x + slotSection->windowStartX, y + slotSection->windowStartY,
                             Red1, activeColor, slotSection->displayBuffer,slotSection->parentContainerSize);
                }
                else
                {
                    // Color all other slots to their defaultDisplay
                    wclear(&slotSection->parentContainer);
                    PlotCell(slotSection->slots[i].defaultDisplay.character, x + slotSection->windowStartX, y + slotSection->windowStartY, slotSection->slots[i].defaultDisplay.fore,
                             slotSection->slots[i].defaultDisplay.back,slotSection->displayBuffer, slotSection->parentContainerSize);
                }
            }
            ++i;
        }
    }
}

// Helper function for toggle slots
static void ClearOtherActiveSlots(SlotSection *slotSection, int skipIndex)
{
    for(int i = 0; i < slotSection->slotCount; ++i)
    {
        if(i != skipIndex)
        {
            slotSection->slots[i].active = 0;
        }
    }
}

// If click event happened, return id of source, otherwise return -1
int ToggleSlotsGetOccupant(SlotSection *slotSection, Input *inputContainer, int activeColor)
{
    for(int i = 0; i <= slotSection->slotCount; ++i)
    {
        if(inputContainer->mouseX == slotSection->slots[i].slotX
           && inputContainer->mouseY == slotSection->slots[i].slotY)
        {
            slotSection->slots[i].active = 1;
            ClearOtherActiveSlots(slotSection, i);
            HighlightActiveSlotsToggle(slotSection, activeColor, slotSection->slots[i].defaultDisplay.character);
            return slotSection->slots[i].slotInfo.iD;
        }
    }
    
    return -1;
}

void DestroySlotSection(SlotSection slotSection)
{
    free(slotSection.slots);
}

int DisplayZoneSelectMenu(WINDOW *uIWindow, Window_Minimum uIWindowMin, Tile *uITile, WINDOW *messageWindow)
{
    ZoneList zoneList;
    int choice;
    GetZones(&zoneList);
    
    ClearNCursesWindow(messageWindow);
    ClearNCursesWindow(uIWindow);
    
    //Waiting for user input
    nodelay(uIWindow, FALSE);
    
    // Display menu items with proper format
    DisplayMenuUI(uIWindowMin, uIWindow, uITile, zoneList.zoneName,
                  zoneList.zoneID, zoneList.zoneCount);
    
    int ch;
    int choiceValid = 0;
    while(!choiceValid)
    {
        mvwprintw(messageWindow, 0, 0, "Select which environment type you wish to create in your chosen Zone");
        FlipFrame(messageWindow);
        
        // Get character from user.
        ch = wgetch(uIWindow);
        for(int i = 0; i <= zoneList.zoneCount; ++i)
        {
            // If pressed key is a valid menu option, set valid choice true.
            if(ch == '0' + zoneList.zoneID[i])
            {
                // Clear input buffer
                flushinp();
                choice = zoneList.zoneID[i];
                choiceValid = 1;
            }
        }
    }
    
    //Reset terminal for use.
    nodelay(uIWindow, TRUE);
    
    ClearNCursesWindow(uIWindow);
    ClearNCursesWindow(messageWindow);
    
    // Convert back to 0 index
    return choice - 1;
}

int ChooseNumericMenu(int optionIDs[], int optionsCount)
{
    int ch;
    int choiceValid = 0;
    int choice;
    while(!choiceValid)
    {
        // Get character from user.
        ch = getch();
        for(int i = 0; i < optionsCount; ++i)
        {
            // If pressed key is a valid menu option, set valid choice true.
            if(ch == '0' + optionIDs[i])
            {
                // Clear input buffer
                flushinp();
                choice = optionIDs[i];
                choiceValid = 1;
            }
        }
    }
    
    return choice;
}

