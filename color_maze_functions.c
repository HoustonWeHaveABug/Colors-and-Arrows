#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "erand.h"
#include "color_maze_types.h"
#include "color_maze_functions.h"

int cm_set_cell(cell_t *, unsigned long, unsigned long);
int solve_maze(unsigned long, int *, unsigned long, unsigned long, node_t **);
int cm_add_neighbours(unsigned long, int *, unsigned long, unsigned long, unsigned long, cell_t *, unsigned long);
int add_neighbour(unsigned long, int *, unsigned long, cell_t *, unsigned long, cell_t *);
int compare_variants(unsigned long, cell_t *, unsigned long, variant_t *);
int add_variant(cell_t *, cell_t *, unsigned long);
int add_node(unsigned long, cell_t *, unsigned long);
void set_node(node_t *, unsigned long, cell_t *, unsigned long);
void mark_path(node_t *);
void print_cells(unsigned long, unsigned long);
void cm_print_cell(cell_t *, int);
int read_color(int *, int);
void set_variant(variant_t *, cell_t *, unsigned long);
variant_t *previous_variant(variant_t *);
void free_cells(cell_t *);

unsigned long cells_n, nodes_n_max, nodes_n;
cell_t *cells;
node_t *nodes;

int *init_sequence(unsigned long sequence_size) {
int *sequence = malloc(sizeof(int)*sequence_size);
unsigned long i;
	if (!sequence) {
		fprintf(stderr, "Could not allocate memory for sequence\n");
		return NULL;
	}
	for (i = 0; i < sequence_size-1; i++) {
		if (!read_color(sequence+i, ' ')) {
			free(sequence);
			return NULL;
		}
	}
	if (!read_color(sequence+i, '\n')) {
		free(sequence);
		return NULL;
	}
	return sequence;
}

int cm_init_data(unsigned long sequence_size, unsigned long columns_n, unsigned long rows_n) {
unsigned long i, j;
cell_t *cell;
	cells_n = columns_n*rows_n;
	if (cells_n < sequence_size) {
		fprintf(stderr, "Maze is too small to contain the sequence\n");
		return 0;
	}
	cells = malloc(sizeof(cell_t)*cells_n);
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		return 0;
	}
	cell = cells;
	for (i = 0; i < rows_n; i++) {
		for (j = 0; j < columns_n; j++) {
			if (!cm_set_cell(cell, j, i)) {
				free_cells(cell);
				return 0;
			}
			cell++;
		}
	}
	nodes_n_max = cells_n;
	nodes = malloc(sizeof(node_t)*nodes_n_max);
	if (!nodes) {
		fprintf(stderr, "Could not allocate memory for nodes\n");
		free_cells(cells+cells_n);
		return 0;
	}
	return 1;
}

int cm_set_cell(cell_t *cell, unsigned long column, unsigned long row) {
	cell->column = column;
	cell->row = row;
	cell->variants_n_max = 1;
	cell->variants = malloc(sizeof(variant_t));
	if (!cell->variants) {
		fprintf(stderr, "Could not allocate memory for cell->variants\n");
		return 0;
	}
	set_variant(cell->variants, NULL, 0UL);
	cell->marked = 0;
	return 1;
}

int cm_solve_or_generate(unsigned long sequence_size, int *sequence, unsigned long columns_n, unsigned long rows_n) {
int *colors, solved;
unsigned long colors_n, i, j;
cell_t *cell;
node_t *node_out;
	if (scanf("%lu", &colors_n) != 1) {
		fprintf(stderr, "Invalid number of colors\n");
		free(sequence);
		return EXIT_FAILURE;
	}
	fgetc(stdin);
	if (colors_n == 0) {
		cell = cells;
		for (i = 0; i < rows_n; i++) {
			for (j = 0; j < columns_n-1; j++) {
				if (!read_color(&cell->color, ' ')) {
					return -1;
				}
				cell++;
			}
			if (!read_color(&cell->color, '\n')) {
				return -1;
			}
			cell++;
		}
		solved = solve_maze(sequence_size, sequence, columns_n, rows_n, &node_out);
		if (solved > 0) {
			mark_path(node_out);
			print_cells(columns_n, rows_n);
		}
	}
	else {
		colors = malloc(sizeof(int)*colors_n);
		if (!colors) {
			fprintf(stderr, "Could not allocate memory for colors\n");
			return -1;
		}
		for (i = 0; i < colors_n-1; i++) {
			if (!read_color(colors+i, ' ')) {
				free(colors);
				return -1;
			}
		}
		if (!read_color(colors+i, '\n')) {
			free(colors);
			return -1;
		}
		solved = cm_generate_maze(sequence_size, sequence, columns_n, rows_n, colors_n, colors);
		free(colors);
	}
	return solved;
}

int cm_generate_maze(unsigned long sequence_size, int *sequence, unsigned long columns_n, unsigned long rows_n, unsigned long colors_n, int *colors) {
int solved;
unsigned long i;
node_t *node_out;
	do {
		for (i = 0; i < cells_n; i++) {
			if (!cells[i].marked) {
				cells[i].color = colors[erand(colors_n)];
			}
		}
		solved = solve_maze(sequence_size, sequence, columns_n, rows_n, &node_out);
		if (!solved) {
			for (i = 0; i < cells_n; i++) {
				cells[i].marked = 0;
			}
			if (node_out) {
				mark_path(node_out);
			}
		}
	}
	while (!solved);
	if (solved > 0) {
		for (i = 0; i < cells_n; i++) {
			cells[i].marked = 1;
		}
		printf("%lu\n", sequence_size);
		for (i = 0; i < sequence_size-1; i++) {
			printf("%c ", sequence[i]);
		}
		printf("%c\n%lu %lu\n0\n", sequence[i], columns_n, rows_n);
		print_cells(columns_n, rows_n);
	}
	return solved;
}

int solve_maze(unsigned long sequence_size, int *sequence, unsigned long columns_n, unsigned long rows_n, node_t **node_out) {
unsigned long i;
	nodes_n = 0;
	for (i = 0; i < cells_n; i++) {
		cells[i].variants_n = 1;
	}
	for (i = cells_n-columns_n; i < cells_n; i++) {
		if (cells[i].color == sequence[0] && !add_node(0UL, cells+i, 0UL)) {
			*node_out = NULL;
			return -1;
		}
	}
	for (i = 0; i < nodes_n && nodes[i].cell->row > 0; i++) {
		if (!cm_add_neighbours(sequence_size, sequence, columns_n, rows_n, nodes[i].sequence_idx+1, nodes[i].cell, nodes[i].variant_idx)) {
			*node_out = NULL;
			return -1;
		}
	}
	if (nodes_n == 0) {
		*node_out = NULL;
		return 0;
	}
	if (i < nodes_n) {
		*node_out = nodes+i;
		return 1;
	}
	else {
		*node_out = nodes+nodes_n-1;
		return 0;
	}
}

int cm_add_neighbours(unsigned long sequence_size, int *sequence, unsigned long columns_n, unsigned long rows_n, unsigned long sequence_idx, cell_t *from, unsigned long variant_idx) {
unsigned long neighbours_n = 0, rand_idx, i;
cell_t *neighbours[4], *cell_tmp;
	if (sequence_idx == sequence_size) {
		sequence_idx = 0;
	}
	if (from->column > 0) {
		neighbours[neighbours_n++] = from-1;
	}
	if (from->row > 0) {
		neighbours[neighbours_n++] = from-columns_n;
	}
	if (from->column < columns_n-1) {
		neighbours[neighbours_n++] = from+1;
	}
	if (from->row < rows_n-1) {
		neighbours[neighbours_n++] = from+columns_n;
	}
	for (i = neighbours_n; i > 1; i--) {
		rand_idx = erand(i);
		cell_tmp = neighbours[i-1];
		neighbours[i-1] = neighbours[rand_idx];
		neighbours[rand_idx] = cell_tmp;
	}
	for (i = 0; i < neighbours_n && add_neighbour(sequence_size, sequence, sequence_idx, from, variant_idx, neighbours[i]); i++);
	return i == neighbours_n;
}

int add_neighbour(unsigned long sequence_size, int *sequence, unsigned long sequence_idx, cell_t *from, unsigned long variant_idx, cell_t *cell) {
unsigned long i;
	if (cell->color == sequence[sequence_idx]) {
		for (i = 0; i < cell->variants_n && compare_variants(sequence_size, from, variant_idx, cell->variants+i); i++);
		if (i < cell->variants_n) {
			return 1;
		}
		return add_variant(cell, from, variant_idx) && add_node(sequence_idx, cell, cell->variants_n-1);
	}
	return 1;
}

int compare_variants(unsigned long sequence_size, cell_t *cell_a, unsigned long variant_idx_a, variant_t *variant_b) {
unsigned long i;
variant_t *variant_a;
	if (variant_b->cell != cell_a) {
		return 1;
	}
	if (sequence_size == 1) {
		return 0;
	}
	variant_a = cell_a->variants+variant_idx_a;
	variant_b = previous_variant(variant_b);
	for (i = 2; i < sequence_size && variant_a && variant_b->cell == variant_a->cell; i++) {
		variant_a = previous_variant(variant_a);
		variant_b = previous_variant(variant_b);
	}
	return i < sequence_size && variant_b->cell != variant_a->cell;
}

int add_variant(cell_t *cell, cell_t *from, unsigned long variant_idx) {
variant_t *variants_tmp;
	if (cell->variants_n == cell->variants_n_max) {
		cell->variants_n_max++;
		variants_tmp = realloc(cell->variants, sizeof(variant_t)*cell->variants_n_max);
		if (!variants_tmp) {
			fprintf(stderr, "Could not reallocate memory for cell->variants\n");
			return 0;
		}
		cell->variants = variants_tmp;
	}
	set_variant(cell->variants+cell->variants_n, from, variant_idx);
	cell->variants_n++;
	return 1;
}

int add_node(unsigned long sequence_idx, cell_t *cell, unsigned long idx) {
node_t *nodes_tmp;
	if (nodes_n == nodes_n_max) {
		nodes_n_max += cells_n;
		nodes_tmp = realloc(nodes, sizeof(node_t)*nodes_n_max);
		if (!nodes_tmp) {
			fprintf(stderr, "Could not reallocate memory for nodes\n");
			return 0;
		}
		nodes = nodes_tmp;
	}
	set_node(nodes+nodes_n, sequence_idx, cell, idx);
	nodes_n++;
	return 1;
}

void set_node(node_t *node, unsigned long sequence_idx, cell_t *cell, unsigned long variant_idx) {
	node->sequence_idx = sequence_idx;
	node->cell = cell;
	node->variant_idx = variant_idx;
}

void mark_path(node_t *node_out) {
variant_t *variant;
	node_out->cell->marked = 1;
	for (variant = node_out->cell->variants+node_out->variant_idx; variant->cell; variant = previous_variant(variant)) {
		variant->cell->marked = 1;
	}
}

void print_cells(unsigned long columns_n, unsigned long rows_n) {
unsigned long i, j;
cell_t *cell;
	cell = cells;
	for (i = 0; i < rows_n; i++) {
		for (j = 0; j < columns_n-1; j++) {
			cm_print_cell(cell++, ' ');
		}
		cm_print_cell(cell++, '\n');
	}
}

void cm_print_cell(cell_t *cell, int separator) {
	if (cell->marked) {
		putchar(cell->color);
	}
	else {
		putchar('/');
	}
	putchar(separator);
}

int read_color(int *color, int separator) {
	*color = fgetc(stdin);
	if (!isalnum(*color)) {
		fprintf(stderr, "Invalid color\n");
		return 0;
	}
	if (fgetc(stdin) != separator) {
		fprintf(stderr, "Invalid separator\n");
		return 0;
	}
	return 1;
}

void set_variant(variant_t *variant, cell_t *cell, unsigned long idx) {
	variant->cell = cell;
	variant->idx = idx;
}

variant_t *previous_variant(variant_t *variant) {
	return variant->cell->variants+variant->idx;
}

void cm_free_data(void) {
	free(nodes);
	free_cells(cells+cells_n);
}

void free_cells(cell_t *last) {
cell_t *cell;
	for (cell = cells; cell < last; cell++) {
		free(cell->variants);
	}
	free(cells);
}
