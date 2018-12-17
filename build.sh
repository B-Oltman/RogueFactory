# With optimizations set, compiler seems to optimize away some of the rest loops, so
# this can't be used until we implement a real clock system
# the -flto flag causes the compiler create a large single object file before linking
# so optimizations can be more aggresive.

OPTIMIZATION_ON="-flto -O3"
OPTIMIZATION_OFF="-O0"

gcc -o roguePirate.out $OPTIMIZATION_ON rp_main.c rp_terminal.c rp_grid_tools.c rp_render.c rp_strings.c rp_globals.c rp_colors.c rp_ui.c rp_world.c rp_game.c rp_editor.c rp_user_input.c rp_physics.c rp_io.c -lncurses
