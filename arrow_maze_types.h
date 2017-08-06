#define CELL_TYPES_N 9
#define CELL_TYPE_W 8311
#define CELL_TYPE_NW 28279
#define CELL_TYPE_N 8302
#define CELL_TYPE_NE 28261
#define CELL_TYPE_E 8293
#define CELL_TYPE_SE 29541
#define CELL_TYPE_S 8307
#define CELL_TYPE_SW 29559
#define CELL_TYPE_HOME 8296
#define DIRECTIONS_N 8
#define MASK_FROM_W 1
#define MASK_FROM_NW 2
#define MASK_FROM_N 4
#define MASK_FROM_NE 8
#define MASK_FROM_E 16
#define MASK_FROM_SE 32
#define MASK_FROM_S 64
#define MASK_FROM_SW 128
#define MASK_QUEUED 256

typedef struct {
	int value;
	int direction1;
	int direction2;
}
cell_type_t;

typedef struct cell_s cell_t;

struct cell_s {
	unsigned long column;
	unsigned long row;
	cell_type_t *type;
	int mask;
	cell_t *from;
};
