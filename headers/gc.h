#pragma once

#include "gc_obj.h"
#include <stddef.h>

typedef struct gc gc_t;

/// @brief Create a new garbag collector context
/// @param argc, argv The argc adress and the argv
/// @pre argc and argc cannot be NULL
/// @return A new context of garbage collector in success, NULL otherwise
gc_t* gc_create(int *argc, char * argv[]);

/// @brief Destroy a garbage colletcor context
/// @param gc The context of the garbage collector
/// @pre gc cannot be NULL
void gc_release(gc_t *gc);

/// @brief Alloc a block/object that is managed by the garbage collector
/// @param gc The context of the garbage collector
/// @param size The size of block/object
/// @param objDestr The destructor for the object
/// @pre gc cannot be NULL
/// @pre size cannot be equal to 0
/// @return A new block of memory if allocation success, NULL otherwise
void* gc_alloc(gc_t *gc, size_t size, gc_destrutor objDestr);

/// @brief Indicate a block of memory to let the gc manage it for you
/// @param gc The context if the garbage collector
/// @param blc The block of that the gc should manage
/// @param blcSize The size of the memory block
/// @param objDestr The destructor of the object
/// @pre gc cannot be NULL
/// @pre blc cannot be NULL
/// @pre blcSize cannot be equal to 0
/// @return 0 if the operation success, -1 otherwise
int gc_push(gc_t *gc, void *blc, size_t blcSize, gc_destrutor objDestr);

/// @brief Start the garbage collection
/// @note The garbage collection may cause a very big overhead
/// @param gc The garbage collector context
/// @pre gc cannot be NULL
void gc_collect(gc_t *gc);
