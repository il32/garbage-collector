#include "gc_dyn_array.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef uint8_t octet;

#define GC_DYN_ARRAY_DEFAULT_CAPACITY 5
#define GC_DYN_ARRAY_SWAP(a, b) {\
									octet buff[sizeof(a) == sizeof(b) ? sizeof(a) : -1];\
									memcpy(buff, &(a), sizeof(a));\
									memcpy(&(a), &(b), sizeof(a));\
									memcpy(&(b), buff, sizeof(a));\
								}

struct gc_dyn_array {
	octet *data;
	size_t size;

	size_t capacity;
	size_t typeSize;

	gc_dyn_array_finaliser finaliser;
};

gc_dyn_array_t * gc_dyn_array_create(size_t typeSize, size_t nbElems, gc_dyn_array_finaliser objFinaliser) {
	assert(typeSize != 0 && "The size of the type can't be equal to 0");

	gc_dyn_array_t *darray = malloc(sizeof *darray);
	void *buff = malloc(typeSize * (nbElems + GC_DYN_ARRAY_DEFAULT_CAPACITY));
	if (darray) {
		if (!buff)
			goto cleanup;
		darray->data = buff;
		darray->size = nbElems;
		darray->capacity = nbElems + GC_DYN_ARRAY_DEFAULT_CAPACITY;
		darray->typeSize = typeSize;
		darray->finaliser = objFinaliser;

		return darray;
	}
cleanup:
	free(darray);
	return NULL;
}

gc_dyn_array_t * gc_dyn_array_createFrom(void const * data, size_t typeSize, size_t nbElems, gc_dyn_array_finaliser objFinaliser) {
	assert(data != NULL && "You must give a list of object to initialise the array");
	assert(typeSize != 0 && "The size of the type can't be equal to 0");

	gc_dyn_array_t *darray = gc_dyn_array_create(typeSize, nbElems, objFinaliser);
	if (darray) {
		memcpy(darray->data, data, typeSize * nbElems);
		return darray;
	}
	return NULL;
}

void gc_dyn_array_release(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	for (size_t i = 0; i < darray->size; ++i) {
		if (darray->finaliser)
			darray->finaliser(gc_dyn_array_at(darray, i));
	}
	free(darray->data);
	free(darray);
}

void * gc_dyn_array_push(gc_dyn_array_t * darray, void const * dataAddress) {
	assert(darray != NULL && "The dynamic array must exist");

	if (darray->size == darray->capacity)
		if (gc_dyn_array_reserve(darray, darray->capacity + GC_DYN_ARRAY_DEFAULT_CAPACITY) == -1)
			return NULL;
	++darray->size;
	if(dataAddress)
		memcpy(gc_dyn_array_back(darray), dataAddress, darray->typeSize);
	return gc_dyn_array_back(darray);
}

void gc_dyn_array_pop(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(!gc_dyn_array_empty(darray) && "Cannot pop an empty array");

	if(darray->finaliser)
		darray->finaliser(gc_dyn_array_back(darray));
	--darray->size;
}

void * gc_dyn_array_insert(gc_dyn_array_t * darray, void const * dataAddress, size_t pos) {
	assert(pos < darray->size && "Out of range");
	assert(darray != NULL && "The dynamic array must exist");

	if (!gc_dyn_array_push(darray, NULL))
		return NULL;
	memmove(gc_dyn_array_at(darray, pos + 1), gc_dyn_array_at(darray, pos), (darray->size - pos) * darray->typeSize);
	
	void *buf = gc_dyn_array_at(darray, pos);
	if(dataAddress)
		memcpy(buf, dataAddress, darray->typeSize);

	return buf;
}

void gc_dyn_array_erase(gc_dyn_array_t * darray, size_t pos) {
	assert(pos < darray->size && "Out of range");
	assert(darray != NULL && "The dynamic array must exist");
	
	if (pos == darray->size - 1) {
		gc_dyn_array_pop(darray);
		return;
	}

	void *toDestroy = gc_dyn_array_at(darray, pos);
	if (darray->finaliser)
		darray->finaliser(toDestroy);
	memmove(gc_dyn_array_at(darray, pos), gc_dyn_array_at(darray, pos + 1), (darray->size - pos) * darray->typeSize);
	--darray->size;
}

void gc_dyn_array_eraseBtw(gc_dyn_array_t * darray, size_t pos1, size_t posEnd) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(pos1 < posEnd && "The first position sould be lower than the second");
	assert(pos1 < darray->size && "pos1 out of range");
	assert(posEnd < darray->size && "posEnd out of range");

	while (pos1 < posEnd--) 
		gc_dyn_array_erase(darray, pos1);
}

void gc_dyn_array_swap(gc_dyn_array_t * darray, gc_dyn_array_t * other) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(other != NULL && "The second dynamic array to intervchange must exist");

	GC_DYN_ARRAY_SWAP(*darray, *other);
}

void gc_dyn_array_clear(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	while (!gc_dyn_array_empty(darray))
		gc_dyn_array_pop(darray);
}

void * gc_dyn_array_at(gc_dyn_array_t * darray, size_t pos) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(pos < darray->size && "Out of range");

	return &darray->data[pos * darray->typeSize];
}

void * gc_dyn_array_front(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(!gc_dyn_array_empty(darray) && "You must have at least one element to access the front of the array");

	return gc_dyn_array_at(darray, 0);
}

void * gc_dyn_array_back(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");
	assert(!gc_dyn_array_empty(darray) && "You must have at least one element to access the back of the array");

	return gc_dyn_array_at(darray, darray->size - 1);
}

void * gc_dyn_array_data(gc_dyn_array_t * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	return darray->data;
}

size_t gc_dyn_array_size(gc_dyn_array_t const * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	return darray->size;
}

int gc_dyn_array_resize(gc_dyn_array_t * darray, size_t newSize) {
	assert(darray != NULL && "The dynamic array must exist");

	if (darray->capacity < newSize)
		if (gc_dyn_array_reserve(darray, newSize) == -1)
			return -1;

	if (darray->size > newSize)
		while (darray->size > newSize)
			gc_dyn_array_pop(darray);
	else
		darray->size = newSize;
	return 0;
}

size_t gc_dyn_array_capacity(gc_dyn_array_t const * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	return darray->capacity;
}

bool gc_dyn_array_empty(gc_dyn_array_t const * darray) {
	assert(darray != NULL && "The dynamic array must exist");

	return darray->size == 0;
}

int gc_dyn_array_reserve(gc_dyn_array_t * darray, size_t newCapacity) {
	assert(darray != NULL && "The dynamic array must exist");

	if (newCapacity <= darray->size)
		return 0;
	void *tmp = realloc(darray->data, newCapacity * darray->typeSize);
	if (!tmp)
		return -1;
	darray->data = tmp;
	darray->capacity = newCapacity;

	return 0;
}
