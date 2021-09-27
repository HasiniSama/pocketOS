#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#pragma once
#include "../type.h"

/* Define template to support multiple datatypes */
typedef void *type_t;

/* Function pointer for compare function, returns non zero if true else zero */
typedef u8int (*compare_predicate_t)(type_t, type_t);

typedef struct {
  type_t *array;
  u32int size;
  u32int maxSize;
  compare_predicate_t compare;
} ordered_array_t;

/* create_ordered_array:
 *  Creates an ordered array.
 *
 *  @param maxSize  Maximum size of array
 *  @param compare  Function pointer for compare (Array is sorted based on the
 *                  compare function either in ascending or in descending order)
 */
ordered_array_t create_ordered_array(u32int maxSize, compare_predicate_t compare);

/* place_ordered_array:
 *  Place array at specified address.
 *
 *  @param address  Pointer to address at which we want the array to be placed
 *                  (Assuming this memory is already allocated)
 *  @param maxSize  Maximum size of array
 *  @param compare  Function pointer for compare (Array is sorted based on the
 *                  compare function either in ascending or in descending order)
 */
ordered_array_t place_ordered_array(void *addr, u32int max_size, compare_predicate_t compare);

/* destroy_ordered_array:
 *  Destructor for the ordered array.
 *
 *  @param  array  Pointer to ordered array created earlier
 */
void destroy_ordered_array(ordered_array_t *array);

/* insert_ordered_array:
 *  Add an item into the array.
 *  @param item   Element to add casted to (void *)
 *  @param array  Pointer to ordered array
 */
void insert_ordered_array(type_t item, ordered_array_t *array);

/* peek_ordered_array:
 *  Get the item at index specified.
 *
 *  @param index  Index at which we want to peek
 *  @param array  Pointer to ordered array
 */
type_t peek_ordered_array(u32int index, ordered_array_t *array);

/* remove_ordered_array:
 *  Deletes the item at location from the array.
 *
 *  @param index  Index at which we want to lookup
 *  @param array  Pointer to ordered array
 */
void remove_ordered_array(u32int index, ordered_array_t *array);

#endif // ORDERED_ARRAY_H
