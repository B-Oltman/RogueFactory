#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <dirent.h>
#include "rp_io.h"
#include "rp_strings.h"

int MakeRpDirectory(char *directoryName)
{
    struct stat fileStatus = {0};
    int err;
    
    // Save old permission mask in a temp
    mode_t old_mask = umask(0);
    if(stat(directoryName, &fileStatus) == -1)
    {
        // Modify permission mask to 0777 for operation
        err = mkdir(directoryName, 0777);
        
        // Set permission mask back to old mask
        umask(old_mask);
    }
    
    // Return error status
    return err;
}

int WriteWorldFile(Editor *editor)
{
    char filePath[RP_MAX_FILE_NAME_LENGTH + 12] = {"data/worlds/"};
    RpStrAppend(filePath, editor->worldInfo.worldName);
    RpStrAppend(filePath, ".rpw");
    
    // Write the world info header at front of file
    FILE *ofp = fopen(filePath, "wb");
    fwrite(&editor->worldInfo, sizeof(WorldInfo), 1, ofp);
    fclose(ofp);
    
    // Append world tile data after world header info
    ofp = fopen(filePath, "ab");
    for(int i = 0; i < editor->worldInfo.tileCount; ++i)
    {
        // Write out the entityBuffer of each tile
        for(int j = 0; j < editor->worldInfo.tileWidth * editor->worldInfo.tileHeight; ++j)
        {
            fwrite(&editor->world[i].entityBuffer[j], sizeof(EnvironmentEntity), 1, ofp);
        }
        
        // Write out tile info
        fwrite(&editor->world[i].zInfo, sizeof(editor->world[i].zInfo), 1, ofp);
        fwrite(&editor->world[i].worldX, sizeof(editor->world[i].worldX), 1, ofp);
        fwrite(&editor->world[i].worldY, sizeof(editor->world[i].worldY), 1, ofp);
        fwrite(&editor->world[i].isLoaded, sizeof(editor->world[i].isLoaded), 1, ofp);
        fwrite(&editor->world[i].slotInfo, sizeof(editor->world[i].slotInfo), 1, ofp);
    }
    fclose(ofp);
}


int LoadWorldFile(Editor *editor, char *worldName)
{
    char filePath[RP_MAX_FILE_NAME_LENGTH + 12] = {"data/worlds/"};
    RpStrAppend(filePath, worldName);
    
    FILE *ifp = fopen(filePath, "rb");
    // Read in world header from file
    int err = fread(&editor->worldInfo, sizeof(WorldInfo), 1, ifp);
    
    // Uses world  header info to read in remaining data
    for(int i = 0; i < editor->worldInfo.tileCount; ++i)
    {
        // Write out the entityBuffer of each tile
        for(int j = 0; j < editor->worldInfo.tileWidth * editor->worldInfo.tileHeight; ++j)
        {
            err = fread(&editor->world[i].entityBuffer[j], sizeof(EnvironmentEntity), 1, ifp);
        }
        
        // Write out tile info
        err = fread(&editor->world[i].zInfo, sizeof(editor->world[i].zInfo), 1, ifp);
        err = fread(&editor->world[i].worldX, sizeof(editor->world[i].worldX), 1, ifp);
        err = fread(&editor->world[i].worldY, sizeof(editor->world[i].worldY), 1, ifp);
        err = fread(&editor->world[i].isLoaded, sizeof(editor->world[i].isLoaded), 1, ifp);
        err = fread(&editor->world[i].slotInfo, sizeof(editor->world[i].slotInfo), 1, ifp);
    }
    
    fclose(ifp);
}

int CheckForSavedWorlds()
{
    DIR *dirPointer = NULL;
    struct dirent *directory;
    
    dirPointer = opendir("data/worlds/");
    
    int i = 0;
    while((directory = readdir(dirPointer)) != NULL)
    {
        // Store world file extension
        char compareExtension[5] = ".rpw";
        char compareBuf[5] = {0};
        
        int fileLength = RpStrLength(directory->d_name);
        int k = 0;
        int isWorldFile = 0;
        // Get last 4 characters of current file and store in buffer
        for(int j = fileLength - 4; j < fileLength; ++j)
        {
            compareBuf[k] = directory->d_name[j];
            ++k;
        }
        // Compare end of current file with target file extension
        if(RpStrCompare(compareExtension, compareBuf) == 1)
        {
            isWorldFile = 1;
        }
        
        // Only add files with target extension .rpw
        if(isWorldFile)
        {
            ++i;
        }
    }
    closedir(dirPointer);
    
    // Return file count;
    return i;
}

int GetSavedWorldList(char worldListBuffer[][RP_MAX_FILE_NAME_LENGTH], int maxFileCount)
{
    DIR *dirPointer = NULL;
    struct dirent *directory;
    
    dirPointer = opendir("data/worlds/");
    if(dirPointer)
    {
        int i = 0;
        while((directory = readdir(dirPointer)) != NULL && i < maxFileCount)
        {
            // Store world file extension
            char compareExtension[5] = ".rpw";
            char compareBuf[5] = {0};
            
            int fileLength = RpStrLength(directory->d_name);
            int k = 0;
            int isWorldFile = 0;
            // Get last 4 characters of current file and store in buffer
            for(int j = fileLength - 4; j < fileLength; ++j)
            {
                compareBuf[k] = directory->d_name[j];
                ++k;
            }
            // Compare end of current file with target file extension
            if(RpStrCompare(compareExtension, compareBuf) == 1)
            {
                isWorldFile = 1;
            }
            
            // Only add files with target extension .rpw
            if(isWorldFile)
            {
                RpStrAppend(worldListBuffer[i], directory->d_name);
                ++i;
            }
        }
        closedir(dirPointer);
        
        return i;
    }
}