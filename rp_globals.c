#include "rp_globals.h"

int FlagIsInMask(int mask, int flag)
{
    // Is flag set in mask?
    return mask & (1 << flag);
}

int SetFlag(int mask, int flag)
{
    // Activate the flag in mask.
    return mask | (1 << flag);
}
