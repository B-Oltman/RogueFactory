#ifndef RP_IO_RH
#define RP_IO_RH

#include "rp_editor.h"

// Create a new sub-directory within the RP directory
int MakeRpDirectory(char *directoryName);
// Save loaded world to disk, currently overwrites worlds with same name
// TODO(baruch): Handle naming collisions
int WriteWorldFile(Editor *editor);

// Load world file from disk into the editor
int LoadWorldFile(Editor *editor, char *worldName);

// Get list of world names in data/worlds directory
int GetSavedWorldList(char worldListBuffer[][RP_MAX_FILE_NAME_LENGTH], int maxFileCount);

// Checks to see if any saved worlds are in the worlds directory
int CheckForSavedWorlds();




#endif
