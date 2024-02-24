#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
typedef bool (*Array_classify)(void *, void *);

typedef struct Array_head {
    uint8_t *const array;
    const size_t count;
    const size_t data_size;
} Array_head;

Array_head *array_create(size_t data_size);
Array_head *array_append(Array_head *list, void *data);
Array_head *array_push(Array_head *list, void *data);
Array_head *array_add_at(Array_head *list, void *data, size_t position);
bool array_get(Array_head *list, void *data, size_t position);

bool array_pop(Array_head *list, void *data);
bool array_remove_at(Array_head *list, size_t position, void *data);
Array_head *array_sort(Array_head *list, Array_classify sort);
long array_search(Array_head *list, void *data, Array_classify search,
                  Array_head **result);
Array_head *array_merge(Array_head *list1, Array_head *list2);