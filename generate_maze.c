#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "erand.h"
#include "color_maze_functions.h"
#include "arrow_maze_functions.h"

#define FORMAT_S_(x) #x
#define FORMAT_S(x) FORMAT_S_(x)
#define MAZE_TYPE_LEN 5
#define MAZE_TYPE_COLOR "color"
#define MAZE_TYPE_ARROW "arrow"
#define COLORS_N 6UL
#define PATH_LEN_MIN 8UL

int main(void) {
int colors[COLORS_N] = { 'B', 'G', 'O', 'P', 'R', 'Y' }, sequence[COLORS_N], color_tmp, generated;
char maze_type[MAZE_TYPE_LEN+1];
unsigned long columns_n, rows_n, rand_idx, i;
	if (scanf("%" FORMAT_S(MAZE_TYPE_LEN) "s %lu %lu", maze_type, &columns_n, &rows_n) != 3 || columns_n < 1 || rows_n < 1) {
		fprintf(stderr, "Invalid parameters\n");
		return EXIT_FAILURE;
	}
	if (!strcmp(maze_type, MAZE_TYPE_COLOR)) {
		if (!cm_init_data(COLORS_N, columns_n, rows_n)) {
			return EXIT_FAILURE;
		}
		srand((unsigned)time(NULL));
		for (i = 0; i < COLORS_N; i++) {
			sequence[i] = colors[i];
		}
		for (i = COLORS_N; i > 1; i--) {
			rand_idx = erand(i);
			color_tmp = sequence[i-1];
			sequence[i-1] = sequence[rand_idx];
			sequence[rand_idx] = color_tmp;
		}
		generated = cm_generate_maze(COLORS_N, sequence, columns_n, rows_n, COLORS_N, colors);
		cm_free_data();
	}
	else if (!strcmp(maze_type, MAZE_TYPE_ARROW)) {
		if (!am_init_data(columns_n, rows_n)) {
			return EXIT_FAILURE;
		}
		am_generate_maze(columns_n, rows_n, PATH_LEN_MIN);
		generated = 1;
		am_free_data();
	}
	else {
		fprintf(stderr, "Invalid maze type\n");
		return EXIT_FAILURE;
	}
	if (generated < 0) {
		return EXIT_FAILURE;
	}
	else {
		return EXIT_SUCCESS;
	}
}
