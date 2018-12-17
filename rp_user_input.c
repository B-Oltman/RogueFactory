#include "rp_ui.h"

// Defines keys depending on menu context.
void DefineKeyContext(State stateId, InputDefinition inputDefs[])
{
    if(stateId == NewGame)
    {
        RpStrCopy(inputDefs[0].actionLabel, "Move Up");
        inputDefs[0].flagIndex = MoveUp;
        inputDefs[0].keyDefinition = KEY_UP;
        
        RpStrCopy(inputDefs[1].actionLabel, "Move Down");
        inputDefs[1].flagIndex = MoveDown;
        inputDefs[1].keyDefinition = KEY_DOWN;
        
        RpStrCopy(inputDefs[2].actionLabel, "Move Left");
        inputDefs[2].flagIndex = MoveLeft;
        inputDefs[2].keyDefinition = KEY_LEFT;
        
        RpStrCopy(inputDefs[3].actionLabel, "Move Right");
        inputDefs[3].flagIndex = MoveRight;
        inputDefs[3].keyDefinition = KEY_RIGHT;
        
        RpStrCopy(inputDefs[4].actionLabel, "Try to Interact");
        inputDefs[4].flagIndex = ActionInteract;
        inputDefs[4].keyDefinition = 'e';
        
        RpStrCopy(inputDefs[5].actionLabel, "Attack");
        inputDefs[5].flagIndex = ActionAttack;
        inputDefs[5].keyDefinition = 'q';
        
        RpStrCopy(inputDefs[6].actionLabel, "Pause Game");
        inputDefs[6].flagIndex = Pause;
        inputDefs[6].keyDefinition = 'p';
    }
    if(stateId == WorldEditor)
    {
        RpStrCopy(inputDefs[0].actionLabel, "Teleport");
        inputDefs[0].flagIndex = OpenWarpWindow;
        inputDefs[0].keyDefinition = 'w';
        
        RpStrCopy(inputDefs[1].actionLabel, "Go Back");
        inputDefs[1].flagIndex = GoBack;
        inputDefs[1].keyDefinition = 'b';
        
        RpStrCopy(inputDefs[2].actionLabel, "Accept Choice");
        inputDefs[2].flagIndex = Accept;
        inputDefs[2].keyDefinition = 'g';
    }
}

void GetInputEvents(Input *input, InputDefinition inputDefinitions[], int inputDefCount)
{
    // Reset flags
    input->triggerFlags = 0;
    
    int pollingCycles = 4;
    int cycle = 0;
    int ch[pollingCycles];
    
    // Poll raw input into array
    for(int i = 0; i < pollingCycles; ++i)
    {
        // Get raw keyboard input
        ch[i] = getch();
    }
    
    // Iterate the raw input
    for(int j = 0;  j <  pollingCycles; ++j)
    {
        // Iterate defined keyboard actions
        for(int k = 0; k < inputDefCount; ++k)
        {
            // If key is defined, process it
            if(ch[j] == inputDefinitions[k].keyDefinition)
            {
                input->triggerFlags = SetFlag(input->triggerFlags ,inputDefinitions[k].flagIndex);
            }
        }
        // Check for mouse input and process it
        if(ch[j] == KEY_MOUSE)
        {
            getmouse(&input->mouseEvent);
            input->mouseX = input->mouseEvent.x;
            input->mouseY = input->mouseEvent.y;
        }
    }
    
}


