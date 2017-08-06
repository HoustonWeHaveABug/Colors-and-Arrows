typedef struct variant_s variant_t;

typedef struct {
	unsigned long column;
	unsigned long row;
	unsigned long variants_n_max;
	variant_t *variants;
	int marked;
	int color;
	unsigned long variants_n;
}
cell_t;

struct variant_s {
	cell_t *cell;
	unsigned long idx;
};

typedef struct {
	unsigned long sequence_idx;
	cell_t *cell;
	unsigned long variant_idx;
}
node_t;
