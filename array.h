#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
typedef bool (*Array_classify)(const void *, const void *);

typedef struct Array_head {
    uint8_t const *const array;
    const size_t count;
    const size_t data_size;
} Array_head;

bool array_create(Array_head **array, size_t data_size);
bool array_append(const Array_head *list, const void *data);
bool array_push(const Array_head *list, const void *data);
bool array_add_at(const Array_head *list, const void *data, size_t position);
bool array_set(const Array_head *list, const void *data, size_t position);
bool array_get(const Array_head *list, void *data, size_t position);

bool array_pop(const Array_head *list, void *data);
bool array_remove_at(const Array_head *list, size_t position, void *data);
bool array_sort(const Array_head *list, const Array_classify sort);
long array_search(const Array_head *list, const void *data,
                  const Array_classify search, Array_head **result);
bool array_merge(Array_head **list_result, const Array_head *list1,const Array_head *list2);

void array_delete(Array_head **list);