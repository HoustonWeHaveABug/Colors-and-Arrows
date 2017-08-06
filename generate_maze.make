GENERATE_MAZE_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings
GENERATE_MAZE_OBJS=erand.o color_maze_functions.o arrow_maze_functions.o generate_maze.o

generate_maze.exe: ${GENERATE_MAZE_OBJS}
	gcc -o generate_maze.exe ${GENERATE_MAZE_OBJS}

erand.o: erand.c erand.h generate_maze.make
	gcc ${GENERATE_MAZE_C_FLAGS} -o erand.o erand.c

color_maze_functions.o: color_maze_functions.c erand.h color_maze_types.h color_maze_functions.h generate_maze.make
	gcc ${GENERATE_MAZE_C_FLAGS} -o color_maze_functions.o color_maze_functions.c

arrow_maze_functions.o: arrow_maze_functions.c erand.h arrow_maze_types.h arrow_maze_functions.h generate_maze.make
	gcc ${GENERATE_MAZE_C_FLAGS} -o arrow_maze_functions.o arrow_maze_functions.c

generate_maze.o: generate_maze.c erand.h color_maze_functions.h arrow_maze_functions.h generate_maze.make
	gcc ${GENERATE_MAZE_C_FLAGS} -o generate_maze.o generate_maze.c

clean:
	rm -f ${GENERATE_MAZE_OBJS} generate_maze.exe
