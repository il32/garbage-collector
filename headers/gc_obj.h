#pragma once

#include <stddef.h>
#include <stdbool.h>

/// @brief The destructor of each oject
/// @note Each destructor should finalise and free the object
/// @note The default destructor is free()
typedef void(*gc_destrutor)(void *data);

typedef struct gc_obj gc_obj_t;
struct gc_obj {
	void *data;
	size_t size;
	gc_destrutor destr;

	gc_obj_t *next;
	bool marked;
};

