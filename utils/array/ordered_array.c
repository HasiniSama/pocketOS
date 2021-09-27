#include "ordered_array.h"
#include "../../memory/heap/kheap.h"

void custom_memset_array(u8int *address, u32int val, u32int size) {
  for (u32int i = 0; i < size; ++i) {
    *address = val;
    ++address;
  }
}

ordered_array_t create_ordered_array(u32int maxSize,
                                     compare_predicate_t compare) {
  ordered_array_t toRet;
  toRet.array = (type_t *)kmalloc(maxSize * sizeof(type_t));
  custom_memset_array((u8int *)toRet.array, 0, maxSize * sizeof(type_t));
  toRet.size = 0;
  toRet.maxSize = maxSize;
  toRet.compare = compare;
  return toRet;
}

ordered_array_t place_ordered_array(void *address, u32int maxSize,
                                    compare_predicate_t compare) {
  ordered_array_t toRet;
  toRet.array = (type_t *)address;
  custom_memset_array((u8int *)toRet.array, 0, maxSize * sizeof(type_t));
  toRet.size = 0;
  toRet.maxSize = maxSize;
  toRet.compare = compare;
  return toRet;
}

void destroy_ordered_array(ordered_array_t *array) {
  (void)array;
  //    kfree(array->array);
}

void insert_ordered_array(type_t item, ordered_array_t *array) {
	/*
  if (!array->compare) {
    print_screen(
        "\nError: insert_ordered_array : No compare function provided\n");
    return;
  }*/

  u32int iterator = 0;
  while ((iterator < array->size) &&
         (array->compare(array->array[iterator], item))) {
    iterator++;
  }

  // If we don't find spot in between to insert, insert to the end
  if (iterator == array->size) {
    array->array[array->size++] = item;
  }
  // if We find spot insert and shift all the elements to right
  else {
    type_t tmp = array->array[iterator];
    array->array[iterator] = item;
    while (iterator < array->size) {
      iterator++;
      type_t tmp2 = array->array[iterator];
      array->array[iterator] = tmp;
      tmp = tmp2;
    }
    array->size++;
  }
}

type_t peek_ordered_array(u32int index, ordered_array_t *array) {
	/*
  if (index >= array->size) {
    print_screen("\nError: peek_ordered_array : Index out of bound\n");
    return 0;
  }*/
  return array->array[index];
}

void remove_ordered_array(u32int index, ordered_array_t *array) {
  while (index < array->size) {
    array->array[index] = array->array[index + 1];
    index++;
  }
  array->size--;
}
