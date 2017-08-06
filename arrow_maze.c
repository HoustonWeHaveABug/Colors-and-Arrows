#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arrow_maze_functions.h"

int main(void) {
int s_or_g;
unsigned long columns_n, rows_n;
	if (scanf("%lu%lu", &columns_n, &rows_n) != 2 || columns_n < 1|| rows_n < 1) {
		fprintf(stderr, "Invalid maze size\n");
		return EXIT_FAILURE;
	}
	if (!am_init_data(columns_n, rows_n)) {
		return EXIT_FAILURE;
	}
	srand((unsigned)time(NULL));
	s_or_g = am_solve_or_generate(columns_n, rows_n);
	am_free_data();
	if (s_or_g < 0) {
		return EXIT_FAILURE;
	}
	else {
		return EXIT_SUCCESS;
	}
}
