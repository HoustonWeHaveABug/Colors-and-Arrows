COLOR_MAZE_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings
COLOR_MAZE_OBJS=erand.o color_maze_functions.o color_maze.o

color_maze.exe: ${COLOR_MAZE_OBJS}
	gcc -o color_maze.exe ${COLOR_MAZE_OBJS}

erand.o: erand.c erand.h color_maze.make
	gcc ${COLOR_MAZE_C_FLAGS} -o erand.o erand.c

color_maze_functions.o: color_maze_functions.c erand.h color_maze_types.h color_maze_functions.h color_maze.make
	gcc ${COLOR_MAZE_C_FLAGS} -o color_maze_functions.o color_maze_functions.c

color_maze.o: color_maze.c color_maze_functions.h color_maze.make
	gcc ${COLOR_MAZE_C_FLAGS} -o color_maze.o color_maze.c

clean:
	rm -f ${COLOR_MAZE_OBJS} color_maze.exe
