#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "color_maze_functions.h"

int main(void) {
int *sequence, s_or_g;
unsigned long sequence_size, columns_n, rows_n;
	if (scanf("%lu", &sequence_size) != 1 || sequence_size < 1) {
		fprintf(stderr, "Invalid sequence size\n");
		return EXIT_FAILURE;
	}
	fgetc(stdin);
	sequence = init_sequence(sequence_size);
	if (!sequence) {
		return EXIT_FAILURE;
	}
	if (scanf("%lu%lu", &columns_n, &rows_n) != 2 || columns_n < 1 || rows_n < 1) {
		fprintf(stderr, "Invalid maze size\n");
		free(sequence);
		return EXIT_FAILURE;
	}
	if (!cm_init_data(sequence_size, columns_n, rows_n)) {
		free(sequence);
		return EXIT_FAILURE;
	}
	srand((unsigned)time(NULL));
	s_or_g = cm_solve_or_generate(sequence_size, sequence, columns_n, rows_n);
	cm_free_data();
	free(sequence);
	if (s_or_g < 0) {
		return EXIT_FAILURE;
	}
	else {
		return EXIT_SUCCESS;
	}
}
