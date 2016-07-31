#include "gc.h"
#include "gc_config.h"
#include "gc_obj.h"

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef uint8_t octet;
struct gc {
	size_t nbObjs;
	size_t maxObjs;

	octet *stackBase;
	gc_obj_t *first;
};

// private

static void addToObjList(gc_t *gc, gc_obj_t *obj) {
	assert(obj != NULL && "object must exist");
	assert(gc != NULL && "gc context must exist");
	
	obj->next = gc->first;
	gc->first = obj;
}

static void markFromStack(gc_t *gc) {
	octet *ebpAdrs;
	GC_REG_VAL(ebp, ebpAdrs);

	for (gc_obj_t *obj = gc->first; obj != NULL; obj = obj->next) {
		for (unsigned int i = 0; (ebpAdrs + i) < gc->stackBase; i += GC_PADDING_SIZE)
			if (*(void**)(ebpAdrs + i) == obj->data) {
				obj->marked = true;
				break;
			}
	}
}

static void markAll(gc_t *gc) {
	markFromStack(gc);
	// TODO: mark heap
	// TODO: mark bss and data
}

static void sweep(gc_t *gc) {
	gc_obj_t **objects = &gc->first;
	while (*objects) {
		// object can be collected
		if (!(*objects)->marked) {
			gc_obj_t *toFree = *objects;
			*objects = toFree->next;
			toFree->destr(toFree->data);
			free(toFree);

			--gc->nbObjs;
		}
		else {
			(*objects)->marked = false;
			objects = &(*objects)->next;
		}
	}
}

// interface

gc_t* gc_create(int const * argc, char const * const argv[]) {
	assert(argc != NULL && "Wrong param");
	assert(argv != NULL && "Wrong param");
	assert(argv[*argc] == NULL && "Bad argc and argv arguments");
	assert(argv[*argc - 1] != NULL && "Bad argc and argv arguments");

	gc_t *gc = malloc(sizeof *gc);
	if (gc) {
		*gc = (gc_t) { 0, GC_MAX_OBJ_INIT, NULL, NULL };
		gc->stackBase = (void*)argc;
		return gc;
	}
	return NULL;
}

void gc_release(gc_t * gc) {
	assert(gc != NULL && "Invalid argument: this pointer can't be NULL");
	gc_collect(gc);
	free(gc);
}

void* gc_alloc(gc_t * gc, size_t size, gc_destrutor objDestr) {
	assert(gc != NULL && "gc context must be a valid pointer to object");
	assert(size != 0 && "Object size cannot be equal to 0");

	if (gc->nbObjs >= gc->maxObjs)
		gc_collect(gc);

	gc_obj_t *obj = malloc(sizeof *obj);
	if (obj) {
		obj->data = malloc(size);
		if (!obj->data)
			goto cleanup;
		obj->destr = (objDestr) ? objDestr : free;
		obj->marked = false;
		obj->size = size;

		addToObjList(gc, obj);
		++gc->nbObjs;

		return obj->data;
	}
cleanup:
	free(obj);
	return NULL;
}

int gc_push(gc_t * gc, void * blc, size_t blcSize, gc_destrutor objDestr) {
	assert(gc != NULL && "gc context must be a valid pointer to object");
	assert(blcSize != 0 && "Object size cannot be equal to 0");
	assert(blc != NULL && "The block of memory cannot be NULL");

	if (gc->nbObjs >= gc->maxObjs)
		gc_collect(gc);

	gc_obj_t *obj = malloc(sizeof *obj);
	if (obj) {
		obj->data = blc;
		obj->destr = (objDestr) ? objDestr : free;
		obj->marked = false;
		obj->size = blcSize;

		addToObjList(gc, obj);
		++gc->nbObjs;

		return 0;
	}
	return -1;
}

void gc_collect(gc_t * gc) {
	assert(gc != NULL && "gc context must be a valid pointer");

	markAll(gc);
	sweep(gc);

	gc->maxObjs = gc->nbObjs * 2;
}
