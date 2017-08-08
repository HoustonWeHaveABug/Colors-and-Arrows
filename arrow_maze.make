ARROW_MAZE_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings
ARROW_MAZE_OBJS=erand.o arrow_maze_functions.o arrow_maze.o

arrow_maze: ${ARROW_MAZE_OBJS}
	gcc -o arrow_maze ${ARROW_MAZE_OBJS}

erand.o: erand.c erand.h arrow_maze.make
	gcc ${ARROW_MAZE_C_FLAGS} -o erand.o erand.c

arrow_maze_functions.o: arrow_maze_functions.c erand.h arrow_maze_types.h arrow_maze_functions.h arrow_maze.make
	gcc ${ARROW_MAZE_C_FLAGS} -o arrow_maze_functions.o arrow_maze_functions.c

arrow_maze.o: arrow_maze.c arrow_maze_functions.h arrow_maze.make
	gcc ${ARROW_MAZE_C_FLAGS} -o arrow_maze.o arrow_maze.c

clean:
	rm -f ${ARROW_MAZE_OBJS} arrow_maze
