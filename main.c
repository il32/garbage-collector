#include "gc.h"

#include <stdlib.h>
#include <stdio.h>

struct A {
	struct B* b;
};

struct B {
	struct A *a;
};

void destroy(void *data) {
	puts("destroy");
	free(data);
}

void f(gc_t *gc) {
	struct A *a = gc_alloc(gc, sizeof *a, destroy);
	struct B *b = gc_alloc(gc, sizeof *b, destroy);

	a->b = b;
	b->a = a;
}

int main(int argc, char *argv[]) {
	gc_t *gc = gc_create(&argc, argv);

	f(gc);

	gc_release(gc);

	return EXIT_SUCCESS;
}
