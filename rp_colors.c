#include "rp_colors.h"
#include "rp_terminal.h"

// Used to create new color pallete based on buffered cell fore and background
// colors.
void ReInitColorPairs(void)
{
    // i = 1 is reserved for default terminal pair.
    for(int i = 2; i < 255; ++i)
    {
        // Initialize linked list of array items.
        // Works by manipulating the index instead of a pointer.
        cPairs[i].next = i + 1;
    }
    
    // Set head node to first open index.
    cPairs[0].next = 2;
    
    // Set 'tail' to 0, that way, once cPairs[index].next = 0, we know
    // that the array is full and we have to choose next best color pair.
    cPairs[255].next = 0;
    cPairs[1].next = 0;
}

int GetColorPair(Color fore, Color back)
{
    int cPair = cPairs[1].next;
    
    while(cPair != 0)
    {
        if(cPairs[cPair].fore == fore && cPairs[cPair].back == back)
        {
            // Match found in pair list, return pair index.
            return cPair;
        }
        
        
        cPair = cPairs[cPair].next;
    }
    
    // Match not found, enter as new pair if space in array
    cPair = cPairs[0].next;
    if(cPair != 0)
    {
        // Update head to point to new further node
        cPairs[0].next = cPairs[cPair].next;
        
        // Points cPair.next back to cPairs[0], which is updated to point at
        // furthest node.
        cPairs[cPair].next = cPairs[1].next;
        cPairs[1].next = cPair;
        
        cPairs[cPair].fore = fore;
        cPairs[cPair].back = back;
        
        // Initialize color pair in ncurses.
        init_pair(cPair, fore, back);
        
        return cPair;
    }
    
    // TODO(baruch): Need to handle code to find a close match from list if pairs
    // Can't be added, for now, it simply replaces colors with terminal default 0.
    // It should be rare to find that colors exceed 255 on screen at once.
    
    return 0; // Color pair not found, nor added.
}


