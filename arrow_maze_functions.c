#include <stdio.h>
#include <stdlib.h>
#include "erand.h"
#include "arrow_maze_types.h"
#include "arrow_maze_functions.h"

void am_set_cell(cell_t *, unsigned long, unsigned long);
int read_cell_type(cell_t *, int);
int rand_cell_type(unsigned long, unsigned long, cell_t *);
int get_cell_type(cell_t *);
void add_excluded_type(int);
void reset_maze(void);
void am_add_neighbours(unsigned long, unsigned long, cell_t *);
void add_to_queue(cell_t *, cell_t *);
void am_print_cell(cell_t *, int);
void print_cell_type(cell_type_t *, int);

int excluded_types[DIRECTIONS_N];
unsigned long cells_n, excluded_types_n, nodes_n;
cell_type_t cell_types[CELL_TYPES_N] = {
	{ CELL_TYPE_W, ' ', 'w' },
	{ CELL_TYPE_NW, 'n', 'w' },
	{ CELL_TYPE_N, ' ', 'n' },
	{ CELL_TYPE_NE, 'n', 'e' },
	{ CELL_TYPE_E, ' ', 'e' },
	{ CELL_TYPE_SE, 's', 'e' },
	{ CELL_TYPE_S, ' ', 's' },
	{ CELL_TYPE_SW, 's', 'w' },
	{ CELL_TYPE_HOME, ' ', 'h' }
};
cell_t *cells, *home, **nodes;

int am_init_data(unsigned long columns_n, unsigned long rows_n) {
unsigned long i, j;
cell_t *cell;
	cells_n = columns_n*rows_n;
	cells = malloc(sizeof(cell_t)*cells_n);
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		return 0;
	}
	cell = cells;
	for (i = 0; i < rows_n; i++) {
		for (j = 0; j < columns_n; j++) {
			am_set_cell(cell++, j, i);
		}
	}
	home = cells+columns_n/2+columns_n*(rows_n/2);
	nodes = malloc(sizeof(cell_t *)*cells_n);
	if (!nodes) {
		fprintf(stderr, "Could not allocate memory for nodes\n");
		free(cells);
		return 0;
	}
	return 1;
}

void am_set_cell(cell_t *cell, unsigned long column, unsigned long row) {
	cell->column = column;
	cell->row = row;
}

int am_solve_or_generate(unsigned long columns_n, unsigned long rows_n) {
unsigned long path_len_min, start_column, start_row, i, j;
cell_t *start, *cell;
	if (scanf("%lu", &path_len_min) != 1 || path_len_min > cells_n) {
		fprintf(stderr, "Invalid minimal path length\n");
		return -1;
	}
	fgetc(stdin);
	if (path_len_min == 0) {
		if (scanf("(%lu,%lu)", &start_column, &start_row) != 2 || start_column >= columns_n || start_row >= rows_n) {
			fprintf(stderr, "Invalid start\n");
			return -1;
		}
		fgetc(stdin);
		cell = cells;
		for (i = 0; i < rows_n; i++) {
			for (j = 0; j < columns_n-1; j++) {
				if (!read_cell_type(cell, ' ')) {
					return -1;
				}
				cell++;
			}
			if (!read_cell_type(cell, '\n')) {
				return -1;
			}
			cell++;
		}
		start = cells+start_column+columns_n*start_row;
		reset_maze();
		for (i = 0; i < nodes_n && nodes[i] != start; i++) {
			am_add_neighbours(columns_n, rows_n, nodes[i]);
		}
		if (i < nodes_n) {
			for (cell = nodes[i]; cell->from; cell = cell->from) {
				am_print_cell(cell, ' ');
			}
			am_print_cell(cell, '\n');
		}
	}
	else {
		am_generate_maze(columns_n, rows_n, path_len_min);
	}
	return 1;
}

int read_cell_type(cell_t *cell, int separator) {
int value;
unsigned long i;
	value = fgetc(stdin) << 8;
	value += fgetc(stdin);
	for (i = 0; i < CELL_TYPES_N && cell_types[i].value != value; i++);
	if (i == CELL_TYPES_N) {
		fprintf(stderr, "Invalid value\n");
		return 0;
	}
	cell->type = cell_types+i;
	if (fgetc(stdin) != separator) {
		fprintf(stderr, "Invalid separator\n");
		return 0;
	}
	return 1;
}

void am_generate_maze(unsigned long columns_n, unsigned long rows_n, unsigned long path_len_min) {
unsigned long path_len, i, j;
cell_t *cell;
	do {
		do {
			for (i = 0; i < cells_n && rand_cell_type(columns_n, rows_n, cells+i); i++);
		}
		while (i < cells_n);
		home->type = cell_types+DIRECTIONS_N;
		reset_maze();
		for (i = 0; i < nodes_n; i++) {
			am_add_neighbours(columns_n, rows_n, nodes[i]);
		}
		path_len = 1;
		for (cell = nodes[i-1]; cell->from; cell = cell->from) {
			path_len++;
		}
	}
	while (path_len < path_len_min);
	printf("%lu %lu\n0\n(%lu,%lu)\n", columns_n, rows_n, nodes[i-1]->column, nodes[i-1]->row);
	cell = cells;
	for (i = 0; i < rows_n; i++) {
		for (j = 0; j < columns_n-1; j++) {
			print_cell_type(cell->type, ' ');
			cell++;
		}
		print_cell_type(cell->type, '\n');
		cell++;
	}
}

int rand_cell_type(unsigned long columns_n, unsigned long rows_n, cell_t *cell) {
unsigned long i;
	excluded_types_n = 0;
	if (cell->column == 0) {
		add_excluded_type(CELL_TYPE_SW);
		add_excluded_type(CELL_TYPE_W);
		add_excluded_type(CELL_TYPE_NW);
		if (cell->row == 0) {
			add_excluded_type(CELL_TYPE_N);
			add_excluded_type(CELL_TYPE_NE);
		}
		else if (cell->row == rows_n-1) {
			add_excluded_type(CELL_TYPE_SE);
			add_excluded_type(CELL_TYPE_S);
		}
	}
	else if (cell->column < columns_n-1) {
		if (cell->row == 0) {
			add_excluded_type(CELL_TYPE_NW);
			add_excluded_type(CELL_TYPE_N);
			add_excluded_type(CELL_TYPE_NE);
		}
		else if (cell->row == rows_n-1) {
			add_excluded_type(CELL_TYPE_SE);
			add_excluded_type(CELL_TYPE_S);
			add_excluded_type(CELL_TYPE_SW);
		}
	}
	else {
		add_excluded_type(CELL_TYPE_NE);
		add_excluded_type(CELL_TYPE_E);
		add_excluded_type(CELL_TYPE_SE);
		if (cell->row == 0) {
			add_excluded_type(CELL_TYPE_NW);
			add_excluded_type(CELL_TYPE_N);
		}
		else if (cell->row == rows_n-1) {
			add_excluded_type(CELL_TYPE_S);
			add_excluded_type(CELL_TYPE_SW);
		}
	}
	do {
		cell->type = cell_types+erand(DIRECTIONS_N);
		for (i = 0; i < excluded_types_n && excluded_types[i] != cell->type->value; i++);
	}
	while (i < excluded_types_n);
	return 1;
}

int get_cell_type(cell_t *cell) {
	return cell->type->value;
}

void add_excluded_type(int type) {
	excluded_types[excluded_types_n++] = type;
}

void reset_maze(void) {
unsigned long i;
	for (i = 0; i < cells_n; i++) {
		cells[i].mask = 0;
	}
	nodes_n = 0;
	add_to_queue(home, NULL);
}

void am_add_neighbours(unsigned long columns_n, unsigned long rows_n, cell_t *from) {
unsigned long i, j;
cell_t *cell = from;
	for (i = from->column; i > 0 && !(cell->mask & MASK_FROM_W); i--) {
		cell->mask |= MASK_FROM_W;
		cell--;
		if (cell->type->value == CELL_TYPE_E && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->column, j = from->row; i > 0 && j > 0 && !(cell->mask & MASK_FROM_NW); i--, j--) {
		cell->mask |= MASK_FROM_NW;
		cell -= columns_n+1;
		if (cell->type->value == CELL_TYPE_SE && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->row; i > 0 && !(cell->mask & MASK_FROM_N); i--) {
		cell->mask |= MASK_FROM_N;
		cell -= columns_n;
		if (cell->type->value == CELL_TYPE_S && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->column, j = from->row; i < columns_n-1 && j > 0 && !(cell->mask & MASK_FROM_NE); i++, j--) {
		cell->mask |= MASK_FROM_NE;
		cell -= columns_n-1;
		if (cell->type->value == CELL_TYPE_SW && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->column; i < columns_n-1 && !(cell->mask & MASK_FROM_E); i++) {
		cell->mask |= MASK_FROM_E;
		cell++;
		if (cell->type->value == CELL_TYPE_W && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->column, j = from->row; i < columns_n-1 && j < rows_n-1 && !(cell->mask & MASK_FROM_SE); i++, j++) {
		cell->mask |= MASK_FROM_SE;
		cell += columns_n+1;
		if (cell->type->value == CELL_TYPE_NW && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->row; i < rows_n-1 && !(cell->mask & MASK_FROM_S); i++) {
		cell->mask |= MASK_FROM_S;
		cell += columns_n;
		if (cell->type->value == CELL_TYPE_N && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
	cell = from;
	for (i = from->column, j = from->row; i > 0 && j < rows_n-1 && !(cell->mask & MASK_FROM_SW); i--, j++) {
		cell->mask |= MASK_FROM_SW;
		cell += columns_n-1;
		if (cell->type->value == CELL_TYPE_NE && !(cell->mask & MASK_QUEUED)) {
			add_to_queue(cell, from);
		}
	}
}

void add_to_queue(cell_t *cell, cell_t *from) {
	cell->mask |= MASK_QUEUED;
	cell->from = from;
	nodes[nodes_n++] = cell;
}

void am_print_cell(cell_t *cell, int separator) {
	printf("(%lu,%lu)%c", cell->column, cell->row, separator);
}

void print_cell_type(cell_type_t *cell_type, int separator) {
	putchar(cell_type->direction1);
	putchar(cell_type->direction2);
	putchar(separator);
}

void am_free_data(void) {
	free(nodes);
	free(cells);
}
