#pragma once 

#include <stddef.h>
#include <stdbool.h>

typedef struct gc_dyn_array gc_dyn_array_t;

/// @brief The destructor of the objects in the dynamic array
/// @note The destructor should not free the object(s)
typedef void(*gc_dyn_array_finaliser)(void *data);

/// @brief Create a dynamic array
/// @param typeSize The size of the type of the objects you add in the array
/// @param nbElems The size of the array
/// @param objFinalizer The destructor of each object you add in the array
/// @pre The size of the type cannot be equal to 0
/// @return A new dynamic array if the allocation success, NULL otherwise
gc_dyn_array_t* gc_dyn_array_create(size_t typeSize, size_t nbElems, gc_dyn_array_finaliser objFinaliser);

/// @brief Create a dynamic array and initialise it with an array of value 
/// @param data An array to a list of value
/// @param typeSize The size of the type of the objects you want to add in the array
/// @param nbElems The number of elements in the array
/// @param objFinaliser The destructor of each objects you add in the array
/// @pre data cannot be equal to NULL
/// @pre The size of the type cannot be equal to 0
/// @return A new dynamic array if the allocation success, NULL otherwise
gc_dyn_array_t* gc_dyn_array_createFrom(void const *data, size_t typeSize, size_t nbElems, gc_dyn_array_finaliser objFinaliser);

/// @brief Destroy a dynamic array
/// @param darray The dynamic array to destroy
/// @pre darray The array cannot be equal to NULL
void gc_dyn_array_release(gc_dyn_array_t *darray);

/// @brief Add an object at the end of the array
/// @param darray The dynamic array
/// @param dataAddress The value where the array should copy
/// @pre darray The array cannot be equal to NULL
/// @return The address of the last object in the vector if allocation success, NULL otherwise
void* gc_dyn_array_push(gc_dyn_array_t *darray, void const *dataAddress);

/// @brief Remove the last object in the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
void  gc_dyn_array_pop(gc_dyn_array_t *darray);

/// @brief Add an element at pos index of the array
/// @param darray The dynamic array
/// @param dataAddress The value where the array should copy
/// @param pos The position of the array where you want to add the object
/// @pre darray cannot be equal to NULL
/// @pre pos must be smaller than the size of the array
/// @return The address of the new object inserted in success, NULL otherwise
void* gc_dyn_array_insert(gc_dyn_array_t *darray, void const *dataAddress, size_t pos);

/// @brief Remove an element that is in pos index
/// @param darray The dynamic array
/// @param pos The position of the array where you want to remove the object
/// @pre darray cannot be equal to NULL
/// @pre pos must be smaller than the size of the array
void  gc_dyn_array_erase(gc_dyn_array_t *darray, size_t pos);

/// @brief Remove all objects from pos1 index to posEnd index
/// @param darray The dynamic array
/// @param pos1 The first position where you want to erase element
/// @param posEnd The last position where you want to erase element
/// @pre darray cannot be equal to NULL
/// @pre pos1 must be smaller than posEnd
/// @pre pos1 must be smaller than the size of the array
/// @pre posEnd must be smaller than the size of the array
void  gc_dyn_array_eraseBtw(gc_dyn_array_t *darray, size_t pos1, size_t posEnd);

/// @brief Swap two array
/// @param darray The dynamic array
/// @param other The second array to swap
/// @pre darray cannot be equal to NULL
/// @pre other cannot be equal to NULL
void  gc_dyn_array_swap(gc_dyn_array_t *darray, gc_dyn_array_t *other);

/// @brief Remove all elements from the vector
/// @note Clear element don't change the capacity of the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
void  gc_dyn_array_clear(gc_dyn_array_t *darray);

/// @brief Access to the pos element from the array
/// @param darray The dynamic array
/// @param pos The index of the array you want to access
/// @pre darray cannot be equal to NULL
/// @pre pos must be smaller than the size of the array
/// @return The object at "pos" index
void* gc_dyn_array_at(gc_dyn_array_t *darray, size_t pos);

/// @brief Access to the first element of the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @pre The array shouldn't be empty
/// @return The address of the first object in the array
void* gc_dyn_array_front(gc_dyn_array_t *darray);

/// @brief Access to the last element of the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @pre The array shouldn't be empty
/// @return The address of the last object in the array
void* gc_dyn_array_back(gc_dyn_array_t *darray);

/// @brief Get the raw pointer that point on the buffer of object
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @return The raw pointer that point on the buffer of object
void* gc_dyn_array_data(gc_dyn_array_t *darray);

/// @brief Get the size of the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @return The size of the array
size_t gc_dyn_array_size(gc_dyn_array_t const *darray);

/// @brief Change the size of the array
/// @note If the new size is greater than the actual size, the array is extended and added allements are not initialised.
///		  If the new size is smaller than the actual size, destructor of the lasts elements are called.
///		  Otherwise, there is no change.
/// @param darray The dynamic array
/// @param newSize The new size of the array
/// @pre darray cannot be equal to NULL
/// @return 0 if the operation success, -1 on failure. In failure, the array is not changed.
int    gc_dyn_array_resize(gc_dyn_array_t *darray, size_t newSize);

/// @brief Get the capacity of the array
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @return The capacity of the array
size_t gc_dyn_array_capacity(gc_dyn_array_t const *darray);

/// @brief Let you know if the array is empty or not
/// @param darray The dynamic array
/// @pre darray cannot be equal to NULL
/// @return true if the array have 0 elements inside, false otherwise
bool   gc_dyn_array_empty(gc_dyn_array_t const *darray);

/// @brief Change the capacity if the array
/// @note If the new capacity is greater than the actual size, the capacity is changed to newCapacity and the array can hold up
///       more objects.
///		  If the new capacity is smaller or equal to the actual size, there's no change
/// @param darray The dynamic array
/// @param newCapacity The new capacity of the array
/// @pre darray cannot be equal to NULL
/// @return 0 if the operation success, -1 otherwise
int    gc_dyn_array_reserve(gc_dyn_array_t *darray, size_t newCapacity);
