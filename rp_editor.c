
#include <stdlib.h>
#include "rp_editor.h"
#include "rp_render.h"
#include "rp_world.h"
#include "rp_io.h"

// Initialize editor, sets the editor up for the update loop
void InitEditor(Editor *editor, WindowGroup winGroup, Tile *uITile, Tile *bigUiTile)
{
    // Load editor key context
    DefineKeyContext(WorldEditor, editor->editorInputDefs);
    // Make sure data directory already exists, it should unless user deleted it.
    MakeRpDirectory("data");
    // Create world directory if doesn't exist
    MakeRpDirectory("data/worlds");
    
    int editorChoice = 0;
    
    // If saved world exists, ask user if they want to load a world, or create a new one
    if(CheckForSavedWorlds() != 0)
    {
        char mainEditorOptions[][RP_MAX_MENU_NAME_LENGTH] = 
        {{"Edit Saved Game World"}, {"Create New Game World"}};
        int mainEditorOptionIDs[] = {1, 2};
        int optionCount = sizeof(mainEditorOptionIDs) / sizeof(int);
        DisplayMenuUI(winGroup.uIWinMin, winGroup.uIWin, uITile, mainEditorOptions,
                      mainEditorOptionIDs, optionCount);
        editorChoice = ChooseNumericMenu(mainEditorOptionIDs, optionCount);
    }
    // If no saved world exists, simply ask the user to create a new world.
    else
    {
        editorChoice = 2;
    }
    
    // Load world from disk if user slects edit saved world
    if(editorChoice == 1)
    {
        // Clear stale uI options
        ClearNCursesWindow(winGroup.uIWin);
        
        // Will only handle explicit number of file names (9) for now  TODO(baruch): make this dynamic and handle multiple digits for selection
        char worldListBuffer[9][RP_MAX_FILE_NAME_LENGTH] = {0};
        int worldListSelectIds[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        int worldNameCount = GetSavedWorldList(worldListBuffer, 9);
        
        ClearNCursesWindow(winGroup.uIWin);
        mvwprintw(winGroup.messageWin, 0, 0, "With your keyboard, enter the number of the world you want to load.");
        
        wrefresh(winGroup.messageWin);
        DisplayMenuUI(winGroup.uIWinMin, winGroup.uIWin, uITile, worldListBuffer,
                      worldListSelectIds, worldNameCount);
        
        int selectedWorldId = ChooseNumericMenu(worldListSelectIds, worldNameCount) - 1; // Subtract 1 to account for starting at 1 instead of 0.
        char worldToLoad[RP_MAX_FILE_NAME_LENGTH];
        RpStrCopy(worldToLoad, worldListBuffer[selectedWorldId]);
        
        ClearNCursesWindow(winGroup.uIWin);
        ClearNCursesWindow(winGroup.messageWin);
        
        //Display loading message
        mvwprintw(winGroup.messageWin, 0, 0, "Loading world, please wait...");
        wrefresh(winGroup.messageWin);
        
        // TODO(baruch): Replace this with function to allocate only memory needed by Load world function.
        CreateNewWorld(worldToLoad, STANDARD_WORLD_WIDTH, STANDARD_WORLD_HEIGHT,
                       winGroup.cameraWinMin.minWidth, winGroup.cameraWinMin.minHeight,
                       &editor->worldInfo, editor->world);
        LoadWorldFile(editor, worldToLoad);
        
        // Set state to warp window so user can pick a world position
        editor->editorState = WarpWindow;
    }
    
    // Create new world
    else if(editorChoice == 2)
    {
        // Clear stale uI options
        ClearNCursesWindow(winGroup.uIWin);
        char newWorldName[RP_MAX_FILE_NAME_LENGTH];
        char *prompt = "Name your new world!!"; 
        
        // Get world name from user and use it as the name for the current world
        PromptForString(prompt, newWorldName, RP_MAX_FILE_NAME_LENGTH, winGroup.messageWin);
        
        ClearNCursesWindow(winGroup.messageWin);
        mvwprintw(winGroup.messageWin, 0, 0, "Loading world, please wait...");
        FlipFrame(winGroup.messageWin);
        // Create the new world and load it into memory
        CreateNewWorld(newWorldName, STANDARD_WORLD_WIDTH, STANDARD_WORLD_HEIGHT,
                       winGroup.cameraWinMin.minWidth, winGroup.cameraWinMin.minHeight,
                       &editor->worldInfo, editor->world);
        ClearNCursesWindow(winGroup.messageWin);
        
        // Set editor state to the warp window so user can pick their first world position
        editor->editorState = WarpWindow;
    }
    
    // Set position for warp window, center in viewing area
    int warpStartX = (editor->worldInfo.tileWidth - (editor->worldInfo.worldWidth)) / 2;
    int warpStartY = (editor->worldInfo.tileHeight - editor->worldInfo.worldHeight) / 2;
    
    // Create the warp window
    CreateWorldWarpMenu(&winGroup, bigUiTile, STANDARD_WORLD_WIDTH, STANDARD_WORLD_HEIGHT,
                        warpStartX, warpStartY, editor->world, &editor->warpWindowSlots);
}

// Update editor, this is where the editor does its work, it runs in the editor loop
void UpdateEditor(Editor *editor, CanvasToolData *canvasToolData, Input *inputContainer,
                  WindowGroup winGroup, Tile *bigUiTile, Tile *uITile)
{
    // Get the user input, mouse and keyboard
    GetInputEvents(inputContainer, editor->editorInputDefs,
                   sizeof(editor->editorInputDefs) / sizeof(InputDefinition));
    
    // Check for warp window button press
    if(FlagIsInMask(inputContainer->triggerFlags, OpenWarpWindow))
    {
        // If warp window button pressed, set state to warp window and display message
        editor->editorState = WarpWindow;
        wclear(winGroup.messageWin);
        // TODO(baruch): Need to make a more robust messaging system that allows user to scroll through past messages etc...
        mvwprintw(winGroup.messageWin, 0, 0, "%s (%c)",
                  editor->editorInputDefs[1].actionLabel, editor->editorInputDefs[1].keyDefinition);
        mvwprintw(winGroup.messageWin, 2, 0,  "Click on a Zone above, then press (%c) to warp to its location (%c) to go back. Zones marked by (x) have no content, those marked by (o) do.", editor->editorInputDefs[0].keyDefinition, editor->editorInputDefs[1].keyDefinition);
        
        wrefresh(winGroup.messageWin);
    }
    
    // Switch on editor state.
    switch(editor->editorState)
    {
        // Main editor state, where the entity painting occurs
        case OnCanvas:
        {
            // TODO(baruch): Need to make a more robust messaging system that allows user to scroll through past messages etc...
            mvwprintw(winGroup.messageWin, 0, 0, "%s (%c) | %s (%c)",
                      editor->editorInputDefs[0].actionLabel, editor->editorInputDefs[0].keyDefinition,
                      editor->editorInputDefs[1].actionLabel, editor->editorInputDefs[1].keyDefinition);
            mvwprintw(winGroup.messageWin, 2, 0,  "Press (%c) to open the warp window and change zones. Press (%c) to go back to main menu", editor->editorInputDefs[0].keyDefinition, editor->editorInputDefs[1].keyDefinition); 
            mvwprintw(winGroup.messageWin, 3, 0,  "With your mouse, choose from the entity palette below and paint!"); 
            
            wrefresh(winGroup.messageWin);
            
            // If go back button is pressed, quit to main menu
            if(FlagIsInMask(inputContainer->triggerFlags, GoBack))
            {
                // TODO(baruch): Placeholder, need to reload canvas tile when user goes back from warp menu.
                ClearNCursesWindow(winGroup.uIWin);
                editor->editorState = QuitToMain;
                return;
            }
            
            // If left mouse clicked, check to see if mouse was over the entity palette, if so pick the
            // entity. If mouse is on canvas, begin painting with entity brush.
            int draw = 0;
            int hasPaint = 0;
            if(inputContainer->mouseEvent.bstate & BUTTON1_PRESSED)
            {
                // Get the zone type of the active world tile
                Zone_Type activeZoneiD = editor->world[editor->activeTileiD].zInfo.zoneID;
                
                // Get the active slot id and highlight selection
                editor->activeTooliD = ToggleSlotsGetOccupant(&canvasToolData->uISlotSection
                                                              [activeZoneiD], inputContainer, 226);
                // Display paint palette
                RenderFullTileLeavePairs(canvasToolData->uISlotSection[activeZoneiD].parentContainerSize,
                                         canvasToolData->uISlotSection[activeZoneiD].displayBuffer,
                                         &canvasToolData->uISlotSection[activeZoneiD].parentContainer);
                
                // If position of cursor is on an active tool, update the paint brush and enable painting.
                if(editor->activeTooliD != -1)
                {
                    // Set display brush to that of clicked zcell
                    UpdateBrushPaint(&canvasToolData->activeBrush,
                                     canvasToolData->zCellPalette
                                     [editor->world[editor->activeTileiD].zInfo.zoneID][editor->activeTooliD]);
                    canvasToolData->activeBrush.hasPaint = 1;
                    
                    
                    // TODO(baruch): This is temporary until UI system is built for labels. Clean up eventually
                    int uImessageX = canvasToolData->uISlotSection[activeZoneiD].windowStartX +
                        canvasToolData->uISlotSection[activeZoneiD].width + 4;
                    
                    // Print label of active tool
                    mvwprintw(winGroup.uIWin, 2, uImessageX, "Active Tool: Entity Brush");
                    mvwprintw(winGroup.uIWin, 2, uImessageX + 28, "Active Entity: %s",
                              canvasToolData->zCellPalette[activeZoneiD][editor->activeTooliD].slotInfo.label);
                    FlipFrame(winGroup.uIWin);
                }
                draw = 1;
            }
            
            RenderEntityTileLeavePairs(&editor->world[editor->activeTileiD], winGroup.cameraWin,
                                       editor->worldInfo.tileHeight, editor->worldInfo.tileWidth);
            FlipFrame(winGroup.cameraWin);
            
            // If brush can paint
            if(canvasToolData->activeBrush.hasPaint)
            {
                // Paint left mouse released
                while(draw == 1)
                {
                    // Continue to check for mouse button events while painting
                    GetInputEvents(inputContainer, editor->editorInputDefs,
                                   sizeof(editor->editorInputDefs) / sizeof(InputDefinition));
                    
                    // Plot the active paint to the tile buffer at the mouse position
                    PlotEntity(&canvasToolData->activeBrush, *inputContainer,
                               editor->world[editor->activeTileiD].entityBuffer,
                               winGroup.cameraWinMin, winGroup.cameraWin);
                    
                    // Canvas was changed set saved flag to 0
                    canvasToolData->canvasSavedFlag = 0;
                    if(inputContainer->mouseEvent.bstate & BUTTON1_RELEASED)
                    {
                        draw = 0;
                    }
                }
            }
        }break;
        // Where the user selects their world position
        case WarpWindow:
        {
            // Load the warp window
            int activeWarpColor = Yellow;
            LoadWorldWarpMenu(winGroup.cameraWin, &editor->warpWindowSlots, editor->world, activeWarpColor);
            
            // Display paint palette
            RenderFullTileLeavePairs(winGroup.cameraWinMin, bigUiTile, winGroup.cameraWin);
            FlipFrame(winGroup.cameraWin);
            
            // Snap current tile so user can go back to it, even after selecting a new warp target.
            int prevTileId = editor->activeTileiD;
            
            while(editor->editorState == WarpWindow || editor->editorState == WarpSelected)
            {
                GetInputEvents(inputContainer, editor->editorInputDefs,
                               sizeof(editor->editorInputDefs) / sizeof(InputDefinition));
                
                int warpId;
                if(inputContainer->mouseEvent.bstate & BUTTON1_PRESSED)
                {
                    warpId = ToggleSlotsGetOccupant(&editor->warpWindowSlots,
                                                    inputContainer, activeWarpColor);
                    if(warpId != -1)
                    {
                        // Display new key options if warpId was valid
                        mvwprintw(winGroup.messageWin, 0, 0, "%s (%c)",
                                  editor->editorInputDefs[0].actionLabel,
                                  editor->editorInputDefs[0].keyDefinition);
                        wrefresh(winGroup.messageWin);
                        
                        // Set selected warp id
                        editor->activeTileiD = warpId;
                        editor->editorState = WarpSelected;
                    }
                }
                
                if(editor->editorState == WarpSelected &&
                   FlagIsInMask(inputContainer->triggerFlags, OpenWarpWindow))
                {
                    // If a warp position is highlighted and the warp position hasn't been touched, mark it as
                    // touched and prompt user to convert it to active zone tile.
                    if(editor->world[warpId].isLoaded == 0)
                    {
                        editor->world[warpId].isLoaded = 1;
                        ZoneList zoneList;
                        GetZones(&zoneList);
                        // Prompt designer to choose zone type of warp target and build that palette
                        editor->world[warpId].zInfo.zoneID =
                            DisplayZoneSelectMenu(winGroup.uIWin, winGroup.uIWinMin,
                                                  uITile, winGroup.messageWin);
                        RpStrCopy(editor->world[warpId].zInfo.zoneName,
                                  zoneList.zoneName[editor->world[warpId].zInfo.zoneID]);
                    }
                    
                    // Display zone palette
                    wclear(winGroup.cameraWin);
                    
                    // Display the zone editor palette
                    
                    RenderFullTileUpdatePairs(winGroup.uIWinMin, canvasToolData->uISlotSection
                                              [editor->world[warpId].zInfo.zoneID].displayBuffer,
                                              winGroup.uIWin);
                    FlipFrame(winGroup.uIWin);
                    
                    wclear(winGroup.messageWin);
                    
                    // Make sure to clear brush and palette selection when going to new tile
                    canvasToolData->activeBrush.hasPaint = 0;
                    editor->editorState = OnCanvas;
                }
                
                RenderFullTileLeavePairs(winGroup.cameraWinMin, bigUiTile, winGroup.cameraWin);
                FlipFrame(winGroup.cameraWin);
                
                // If go back button pressed in warp window, go back to canvas, 
                if(FlagIsInMask(inputContainer->triggerFlags, GoBack))
                {
                    ClearNCursesWindow(winGroup.uIWin);
                    // If designer goes back, make sure they go back to previous tile, not to new warp point.
                    editor->activeTileiD = prevTileId;
                    wclear(winGroup.messageWin);
                    editor->editorState = OnCanvas;
                }
            }
        }break;
    }
}

// Renders entity to window and updates color pairs
void RenderEntityTileUpdatePairs(WorldTile *worldTile,
                                 WINDOW *displayDest, int tileHeight, int tileWidth)
{
    ReInitColorPairs();
    RenderEntityTileLeavePairs(worldTile, displayDest, tileHeight, tileWidth);
}

// Renders entity to window and doesn't update color pairs
void RenderEntityTileLeavePairs(WorldTile *worldTile,
                                WINDOW *displayDest, int tileHeight, int tileWidth)
{
    for(int row = 0, i = 0; row < tileHeight; ++row)
    {
        for(int col = 0; col < tileWidth; ++col)
        {
            Color fore = worldTile->entityBuffer[i].cell.fore;
            Color back = worldTile->entityBuffer[i].cell.back;
            char ch = worldTile->entityBuffer[i].cell.character;
            int cPair = GetColorPair(fore, back);
            
            wattron(displayDest, COLOR_PAIR(cPair));
            mvwaddch(displayDest, row, col, ch);
            wattroff(displayDest, COLOR_PAIR(cPair));
            ++i;
        }
    }
}

// Plot entity to buffer
int PlotEntity(EnvironmentBrush *activeBrush, Input inputContainer,
               EnvironmentEntity *entityCanvas, Window_Minimum canvasSize, WINDOW *canvasWindow)
{
    // Simply ignore call to plot if outside bounds of terminal,
    // this prevents buffer overflow crashes.
    int x = inputContainer.mouseX;
    int y = inputContainer.mouseY;
    
    
    if(x < canvasSize.minWidth && x >= 0 && y < canvasSize.minHeight && y >= 0)
    {
        // Plot to canvas buffer for saving
        int cartesianToBuf = (y * canvasSize.minWidth) + x;
        
        entityCanvas[cartesianToBuf].cell.character = activeBrush->paint.cell.character;
        entityCanvas[cartesianToBuf].cell.fore = activeBrush->paint.cell.fore;
        entityCanvas[cartesianToBuf].cell.back = activeBrush->paint.cell.back;
        
        // Paint to window
        int cPair = GetColorPair(activeBrush->paint.cell.fore, activeBrush->paint.cell.back);
        wattron(canvasWindow, COLOR_PAIR(cPair));
        mvwaddch(canvasWindow, inputContainer.mouseY, inputContainer.mouseX,
                 activeBrush->paint.cell.character);
        wattroff(canvasWindow, cPair);
        
        FlipFrame(canvasWindow);
    }
    else
    {
        // Tried to plot outside bounds of screen.
        return -1;
    }
    
    return 0;
}

// Prepares the canvas tools, this includes the uI portion of the editor
void CreateCanvasTools(CanvasToolData *canvasToolData, int uIWinStartY,
                       WindowGroup winGroup, Editor *editor)
{
    // Initialize brush to blank and black
    canvasToolData->activeBrush.paint.cell.fore = Black;
    canvasToolData->activeBrush.paint.cell.back = Black;
    canvasToolData->activeBrush.paint.cell.character = ' ';
    
    // Prepare color pairs
    ReInitColorPairs();
    
    // Create the zone palettes.
    ZoneList zones;
    for(int i = 0; i < ZoneCount; ++i)
    {
        // Allocate buffer for each zone type
        AllocateTileBuffer(winGroup.uIWinMin.minWidth, winGroup.uIWinMin.minHeight,
                           &canvasToolData->paletteBuffer[i]);
        
        // Create slot section to hold tools in editor
        canvasToolData->uISlotSection[i].yStartPosInTerminal = uIWinStartY;
        
        // Slot are basically sub sections of the window, they can be set
        // to a separate size and moved anywhere in the window, the mouse
        // will still handle clicke events properly.
        CreateSlotSection(*winGroup.uIWin, winGroup.uIWinMin, &canvasToolData->paletteBuffer[i], 9,
                          winGroup.uIWinMin.minHeight, &canvasToolData->uISlotSection[i], 0, 0);
        
        // Allocate memory for the zone display palette and assign each zone in the canvastool struct
        // to its corresponding display buffer.
        SetUpZone(i, &canvasToolData->zInfo[i]);
        canvasToolData->zCellPalette[i] = LoadZoneEditor(&canvasToolData->zInfo[i],
                                                         winGroup.uIWinMin, &canvasToolData->uISlotSection[i]);
    }
    
    
    ClearNCursesWindow(winGroup.messageWin);
    // TODO(baruch): Need to make a more robust messaging system that allows user to scroll through past messages etc...
    mvwprintw(winGroup.messageWin, 2, 0,  "Now that you've created your world, you need to form it.");
    mvwprintw(winGroup.messageWin, 3, 0,  "Click on a Zone above, then press (%c) to warp to its location. Zones marked by (x) have no content, those marked by (o) do.", editor->editorInputDefs[0].keyDefinition);
    wrefresh(winGroup.messageWin);
    
    
    editor->editorState = WarpWindow;
}

// Loads the zone editor section of the editor
ZoneCell *LoadZoneEditor(ZoneBuilderInfo *zInfo, Window_Minimum uIWindowMin, SlotSection *slotSection)
{
    FillTileBuffer(uIWindowMin.minWidth, uIWindowMin.minHeight, slotSection->displayBuffer, ' ', 0, 0);
    
    ZoneCell *zCellPalette = BuildZoneCellPalette(zInfo);
    
    // Renders world editor ui.
    for(int y = 0, i = 0; y < slotSection->height; ++y)
    {
        for(int x = 0; x < slotSection->width && i < zInfo->zCellTypeCount; ++x)
        {
            // Plot zone cells to ui tile, zone tiles are different for each zone.
            PlotCell(zCellPalette[i].display.character,
                     slotSection->windowStartX + x, slotSection->windowStartY + y,
                     zCellPalette[i].display.fore, zCellPalette[i].display.back,
                     slotSection->displayBuffer, slotSection->parentContainerSize);
            
            // Store default slot state for toggle of highlight when active.
            slotSection->slots[i].defaultDisplay.fore = zCellPalette[i].display.fore;
            slotSection->slots[i].defaultDisplay.back = zCellPalette[i].display.back;
            slotSection->slots[i].defaultDisplay.character = zCellPalette[i].display.character;
            ++i;
        }
    }
    
    // Adds zCells to tool slots
    for(int i = 0; i < zInfo->zCellTypeCount; ++i)
    {
        AddToolToSlot(slotSection, i, zCellPalette[i].slotInfo);
    }
    
    return zCellPalette;
}

// Updates the colors and character painted by the brush
void UpdateBrushPaint(EnvironmentBrush *brush, ZoneCell zCell)
{
    brush->paint.cell.fore = zCell.display.fore;
    brush->paint.cell.back = zCell.display.back;
    brush->paint.cell.character = zCell.display.character;
}

// Frees allocated canvas memory
void DestroyEntityCanvas(EnvironmentEntity *entityCanvas)
{
    if(entityCanvas)
    {
        free(entityCanvas);
    }
}

// Frees memory allocated for editor
void EndWorldEditor(CanvasToolData canvasToolData)
{
    for(int i = 0; i < ZoneCount; ++i)
    {
        DestroyZoneCellBuffer(canvasToolData.zCellPalette[i]);
        DestroySlotSection(canvasToolData.uISlotSection[i]);
    }
}