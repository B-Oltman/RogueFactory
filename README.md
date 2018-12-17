## Readme
This document will be updated as the project evolves.

# Final Project - Rogue Pirate

## CSC250 Completion Status
For class, I was able to implement a lot of the core features needed to build the game. Most of my focus went into creating a system to handle rendering and the world editor. Most of your exploration should be done in the World Editor, but there is also a Camera Transition test which will eventually be used in gameplay, to transition between the zones.

There is still a lot to do for this project, I intend to continue it after the class is finished. I have made sure to meet all the project requirements up to this point. Please read below for some important notes on building and executing the engine.

**Rogue Pirate is an ASCII roguelike adventure game including a world editor, island, dungeon, and sea exploration with future support for procedural world generation.**

## Gameplay
The goal of Rogue Pirate is to survive, explore, collect loot, and find the buried treasure. This is my final project for CSC250 and will utilize various concepts learned in the class.

The game will contain a mixture of real-time and turn-based events. Some creatures, for example, will continue to move and attack independently from 'turns.'

## World
The game will be sea-based meaning the pirate player explores the seas by boat while fighting various sea monsters and gathering resources/experience along the way. There will also be islands which can be explored and dungeon portals which will bring the player to a random dungeon.

## Building Rogue Pirate
Simply execute the `build.sh` script in a UNIX terminal `./build.sh`. This shell script includes all of the compilation and linker flags required to build RP on dsunix.

**Build Requirements**
- [ ] Must have Ncurses library installed on the system (already installed on Dsunix). This library is used to handle different terminal escape codes for more control.

## Executing Rogue Pirate

### The following steps are important!

### 1. RP requires the terminal be set to a minimum size to run.

*The engine runs validation code to make this process easy. Simply follow the prompt and restart the game once it closes (prompt only displays if the terminal is too small).*

*For best results, make sure the terminal isn't maximized to full-screen, the game works in full-screen, but some terminals cause strange glitches, depending on how they handle line wrapping etc...*

*If the terminal cannot be set to required dimensions (because of screen limits), reduce the font size and try to resize again. The game needs a minimum terminal width and height to run.*

### 2. To run rogue pirate, execute the program roguePirate like so:
`./roguePirate.out`

### 3. The game requires that client terminal font is set to some constant-width monospace font. I suggest **liberation mono** if available.

### 4. When running RP via SSH from a remote terminal, there are times when using the mouse that the terminal will lose focus if the mouse is moved quickly. If hotkeys aren't working after using the mouse, you need to click in the terminal a couple of times to give it focus again. This problem doesn't exist if executing the game locally.

## Project Requirements
Below is a list of the project requirements and how I have used, or intend to use them in my project.

- [ ] Strings Manipulation - Developed custom string library for easier management of the many string operations that will be required in RP. See `rp_strings.c`.

- [ ] 2D Arrays - Used for worldwide coordinates in rp_game.c. (Makes accessing world position more intuitive than converting a single dimensional array.). Also used for handling related strings, such as in the MenuItems struct in rp_game.h

- [ ] File I/O (More than simply outputting) - Used for saving and loading custom binary file type for the created worlds.

- [ ] Command Line Arguments - Used to pass random seed value so that various randomized aspects of the game can be replicated and shared by remembering your the value. This argument is optional.

- [ ] Pass by Pointer - Used often, for example, in the `AllocateTileBuffer()` function, or whenever I need to access a struct in a function but don't want to pass the entire thing. Etc...

- [ ] Structures - Used often for grouping related data types to build components.

- [ ] Linked Lists (Pointers) - Will be used for tracking living and dead creatures, spawned vs. collected items, and probably in the inventory system. Used an array-linked list for handling color pairs.

- [ ] Header Files (Multiple C Files) - Used extensively to keep the project organized.

- [ ] Sorting - Will be used in situations where heavy searching of an array is required. Only makes sense if the array needs to be searched multiple times for a single sort (not currently implemented).

- [ ] Stack/Queue - May use for custom memory allocator in the future (not currently implemented).


## Development Log (Chronological order)
### The Grid

The grid is a structure that represents camera screen space. The grid isn't bound to a single data type. Each grid can represent one of many different or related operations of the game.

For example, there could be a separate grid to represent each of the following: 
- [ ] Legal spawn points
- [ ] Loot positions
- [ ] Unreachable landmarks and suggested opening placement points
- [ ] Cell color animations etc...

The idea is, I need to track and apply world state so it can be saved and loaded from disk on demand (streamed). Certain aspects of the world will be built directly into the tiles as entities, but some dynamically loaded and randomly generated content will need to be applied at run-time.

These different operations will need to make changes to world tile cells. Having to iterate the tile more than once to find a cell for some operation to be applied is not efficient.

Using a grid structure allows me to process these operations and then to apply them all to the world tile efficiently. They can be processed in such a way that priority is considered, and any rendering order conflicts are resolved.

The grid represents local coordinate space. Grids hold information about the world tile needed by different game operations.

These operations will use the grid information to know which cells can be changed, where items can be spawned, which cells can be docks, entrances, monsters, etc... However, the data from the grid can be shared among related game generators, etc. Therefore, the game world tile won't need to be iterated multiple times in search of valid cells for different operations.

The world tile can be iterated once, and a grid for each operation can be created and stored. The grid will be used to create a buffer of coordinates that can be quickly iterated when points of interest for some game operation need to be picked (such as choosing a random spawn point.)

In short, the grid is a way to allow the world to be changed dynamically at run-time in an efficient way.

### The Camera
In Rogue Pirate, the concept of a camera is like you would expect. It is the window through which the player views the game world. With an ascii open world game, there are a few challenges I need to address.

**Transitioning between tiles**
Most ascii roguelike games I've seen limit the player to a single viewing area until a portal or staircase is discovered. The nature of RP means it is an open world that is freely explorable.

Thus, I needed a solution for moving from one tile to the next as the player moves past screen boundaries. I decided to load two tiles, the current tile, and the destination tile. I then read, from each tile buffer, a row/column of cells into a new frame. I then flip the frame after each row/column is written. The order in which rows/columns are written depends on the transition direction.

**Tracking its own world position**
The camera needs to know where it is in the world so it can know if it's at the game boundary and not attempt to move past it.

Each tile struct will already be storing its world position, so the camera needs to look at the tile it's viewing to know where it is.

**Debugging camera transition**
I ran into a nasty bug. It only presented itself when I tried to transition between two tiles that weren't simply filled with a single character. When a write beyond the bounds of the screen happens, it wraps to the next line. Therefore, when my screen was filled with a single character, the bug went unnoticed.

The bug was caused by behavior in the ncurses library writechar commands. After each write, the cursor advances automatically, but I was also handling this manually.

After fixing this first problem, I realized that my transition code never actually worked. So I spent time figuring out why. I ended up having to think through the process again and finally came up with the proper reads and writes.

I had to coordinate reads out of two tile buffers while simultaneously writing those rows to a single frame in the proper sequence (while also working within the limits of the terminal commands.) This made it more difficult to reason about the problem.

I eventually solved it by testing different inputs to the buffer, eliminating possible sources of the bugs, and carefully thinking through the flow of the code (multiple times).

I'm not going to be using a debugger for this project, so I hope to become better at this process as I work through development.

### Color Support

Our target terminal is limited to 256 colors. The ncurses library limits us to displaying 256 pairs of colors at once. Each cell's fore and background colors are updated together. I implemented a system that handles reloading color pairs for each frame depending on which colors are to be rendered on the screen. This basically removes the 256 color concern since colors can be reloaded on the fly.

This system will make creating custom color combinations very easy. For cross-compatibility, I'm only defining colors that are part of the xterm256-color palette. However, we can combine these colors in any combination.

### Ui and World Editor

The world will consist of tiles; each tile will represent a zone. The zone will determine which cells are available for the tile's construction. For example, when you're on your ship near an island, you will be in a shallow sea zone. This will unlock a certain set of "zone cells" in the world editor. These zone cells will be pre-defined cells with fore and back colors, also the display character. The zone cell will also have a label, which will be visible in the editor when the mouse hovers over it.

Once a zCell is selected and added to the world tile it becomes a world entity; the world entity contains some extra data. Data such as a set of flags that define how the item will behave and exist in the world, the entity will store its parent tile id and its local coordinates within the parent tile. This information can be used to identify the entity's precise location in the game world.

One of my primary goals is to make the interface of this game very simple; it will be a single section of the screen. The ui will be a second ncurses window with its tile for rendering buttons, strings, editor tools, etc...

#### Menu System

The ui will consist of various menus that can be called up from a main menu. There will be options for loading the world editor, playing the game, changing settings, etc...

I'm working on implementing a universal menu system that can be used throughout the engine without needing to worry about how the options are displayed and selected, and it will just work (assuming a few pieces of information are passed to the menu builder function.) For now, this system is in its infancy and gives only the minimum required features.

*Edit:* This menu system is now working, by calling two functions and passing an array of strings with menu item names, and an array of integers, which are the menu option IDs, the user can build and display a menu which returns options selected with the keyboard. Very simple and can be called from within a loop anywhere in the engine. (Input polling required inside whatever loop the menu is called from, as you would expect.).

### Input event system

I implemented a system that allows an array of input definition structures to be created depending on the context (editor vs. game for an example.) The GetInputEvents() function will take in the input definitions and then poll the keyboard and mouse to gather raw input.

After the input loop completes, it will process the raw input and convert it into action bit flags in a bit vector that will eventually be passed to the game update loop. All the game cares about is seeing which bit flags are set, it doesn't need to know anything about the keyboard.

#### Mouse support

At first, I had trouble making the mouse port cross-terminal. I eventually figured out it was caused by storing a pointer to `MEVENT` in my input struct. `MEVENT` is what is used to store the mouse events. For some reason, some terminals crashed when this was dereferenced via a pointer.

The mouse works by sending an escape code to the terminal which represents where it is in the window during some event. The terminal can be configured to get mouse position events all the time, or to get them only while a button is pressed. For this game, we only need to get mouse position when a mouse button is being pressed. This will allow us to paint in the world editor, and also use the mouse for ui things.

The polling loop will get both keyboard and mouse input and store it in the same struct which can be checked within the game's update function. For the type of mouse support we require, the terminal value for XM: needs to be set to 1002, this is accomplished by sending an escape sequence to the terminal through printf: `printf("\033[?1002h\n");`

## Delta Time and Physics

I decided to create a simple physics system for this game. There will need to be some
work done to make movement enjoyable. There are different environment types so that the movements will be more advanced than a typical roguelike game.

## Ui Slots and Tools

The Ui is going to be designed with a modular mindset. Thus I developed the notion of a slot. Ncurses allows us to split the terminal into separate windows that can be updated independently from each other, but this use is limited.

The game and editor will use various 'tools', (each tool can be thought of as any element that has a label describing itself and an ID.) The ID is an integer assigned when the tool is initialized. It will be used for various things in the engine, but in the context of slots, it will link the tool to its slot.

For example, when I load a tool and click its slot position, the program needs to know which tool is in that slot and act accordingly. It can check the slot, look up its occupant's id, and do its thing.

SlotSections are intended for groups of related tools. A slot section will be handled differently depending on the type of label its inhabiting tools use, and some items will always need to display a label, while others will only show when clicked.

The sections can be set to any arbitrary size and moved to any arbitrary position. Eventually, I want to add support to move them around with the mouse and snap them to various positions (the framework is already in place.) A UI will be made up of multiple slot sections, each section handles its clicks independently and can include tools with duplicate ids without collision issues.

The system will be robust which will be necessary for this game because RP will have an extensive inventory system.

## State Machine

I've been working on refactoring the world editor so that it handles state more logically. Currently, the state is contained in an editor struct, and different events will set and read the state. This is used heavily for limiting active tools in menus, etc.

## Moving around the world in the editor

I've implemented a feature allowing the designers to teleport to different zones in the world. The code loads and displays an interface called a warp window. The window is a representation of all the tiles currently in the world. The uI will show which tiles have already been touched and allow the user to choose a zone type when visiting a new tile. As discussed previously, the zones will determine which entities are available to the designer for building that part of the world.

## Saving and loading from disk

I implemented a system that allows the designer to save their game world to disk. The game will ship with a data directory and also a worlds directory. When the user first runs the world editor, they'll be prompted to name their world. Once they name the world and edit tiles, they will be prompted to save when quitting.

If the user decides to save, the current world will be written to disk. First, a file header containing data about the world will be written, and then each tile will be written in a binary format.

Next time the designer loads the world editor (any time world files exist in the directory), they will be asked if they want to create a new world or choose an existing world from a list of those found in the worlds directory.

The RoguePirate world files will end in a .rpw extension. They are large files, for now, containing the data needed to store the full world. However, they will be optimized in the future.