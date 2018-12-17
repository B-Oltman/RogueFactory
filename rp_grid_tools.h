/*The grid is a concept that will be used to manage different 'screens' and layers
a grid is represented by a 2D array of min-col x min-rows. A grid is a buffer 
of coordinates that can contain various types of data. Generally it will contain 
characters, but it also may contain boolean values to determine valid spawn 
locations, etc... it is more of a general tool to be used in multiple ways.
*/
#ifndef GRID_TOOLS_RPH
#define GRID_TOOLS_RPH

typedef struct
{
    int x;
    int y;
}Vec2;


#endif