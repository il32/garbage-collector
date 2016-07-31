#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef void(*gc_destrutor)(void *data);

typedef struct gc_obj gc_obj_t;
struct gc_obj {
	void *data;
	size_t size;
	gc_destrutor destr;

	gc_obj_t *next;
	bool marked;
};

