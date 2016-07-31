#include "gc.h"

#include <stdlib.h>
#include <stdio.h>

void destroy(void *data) {
	puts("Destroy");
	free(data);
}

int main(int argc, char *argv[]) {
	gc_t *gc = gc_create(&argc, argv);

	gc_alloc(gc, 4, destroy);

	gc_release(gc);

	return EXIT_SUCCESS;
}
