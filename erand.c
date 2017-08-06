#include <stdlib.h>
#include "erand.h"

unsigned long erand(unsigned long values) {
	return (unsigned long)(rand()/(RAND_MAX+1.0)*values);
}
