#include "array.h"

#include <errno.h>
#include <string.h>
extern int errno;

#define MEMORY_BLOCK 20
void array_split_merge(const Array_head *b, const Array_head *a, size_t i_begin,
                       size_t i_end, const Array_classify sort);
void array_merge_down(const Array_head *b, const Array_head *a, size_t i_begin,
                      size_t i_middle, size_t i_end, const Array_classify sort);
void array_count_edit(const Array_head *list, size_t count);
void array_data_size_edit(const Array_head *list, size_t data_size);
void array_array_edit(const Array_head *list, uint8_t *start);
bool array_resize(const Array_head *list, size_t size_new);
void array_position_set(const Array_head *list, size_t position, const void *data);
void array_position_get(const Array_head *list, size_t position, void *data);

bool array_create(Array_head **array, size_t data_size) {
    if (!data_size || array == NULL || *array != NULL) {
        errno = EINVAL;
        return false;
    }
    *array = (Array_head *)calloc(1, sizeof(Array_head));
    if (array == NULL) {
        errno = ENOMEM;
        return false;
    }
    array_data_size_edit(*array, data_size);
    if (!array_resize(*array, 1)) {
        return false;
    }
    return true;
}

bool array_append(const Array_head *list, const void *data) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }
    if (list->count % MEMORY_BLOCK == 0) {
        if (!array_resize(list, list->count / MEMORY_BLOCK + 2)) {
            return false;
        }
    }
    array_position_set(list, list->count, data);
    array_count_edit(list, list->count + 1);
    return true;
}

inline bool array_push(const Array_head *list, const void *data) {
    return array_add_at(list, data, 0);
}

bool array_add_at(const Array_head *list, const void *data, size_t position) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }
    if (position > list->count) {
        errno = EINVAL;
        return false;
    }
    if (list->count % MEMORY_BLOCK == MEMORY_BLOCK - 1) {
        if (!array_resize(list, (list->count / MEMORY_BLOCK + 2))) 
            return false;
        
    }
    for (size_t i = list->count; i > position; i--)
        array_position_set(list, i, list->array + (i - 1) * list->data_size);
    array_position_set(list, position, data);
    array_count_edit(list, list->count + 1);
    return true;
}
bool array_set(const Array_head *list, const void *data, size_t position) {
    if (list == NULL || data == NULL || position >= list->count) {
        errno = EINVAL;
        return false;
    }
    array_position_set(list, position, data);
    return true;
}

bool array_get(const Array_head *list, void *data, size_t position) {
    if (list == NULL || data == NULL || position >= list->count) {
        errno = EINVAL;
        return false;
    }
    array_position_get(list, position, data);
    return true;
}
bool array_pop(const Array_head *list, void *data) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }
    if (!list->count) {
        errno = EPERM;
        return false;
    }

    if (data != NULL) array_position_get(list, list->count - 1, data);
    uint8_t *pointer = (uint8_t *)calloc(1, list->data_size);
    if (pointer == NULL) {
        errno = ENOMEM;
        return false;
    }
    array_position_set(list, list->count - 1, pointer);
    free(pointer);
    array_count_edit(list, list->count - 1);
    return true;
}

bool array_remove_at(const Array_head *list, size_t position, void *data) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }
    if (!list->count) {
        errno = EPERM;
        return false;
    }
    if (position >= list->count) {
        errno = EINVAL;
        return false;
    }
    if (data != NULL) array_position_get(list, list->count - 1, data);
    for (size_t i = position; i < list->count - 1; i++)
        array_position_set(list, i, list->array + (i + 1) * list->data_size);
    uint8_t *pointer = (uint8_t *)calloc(1, list->data_size);
    if (pointer == NULL) {
        errno = ENOMEM;
        return false;
    }
    array_position_set(list, list->count - 1, pointer);
    free(pointer);
    array_count_edit(list, list->count - 1);
    return true;
}

bool array_merge(Array_head **list_result, const Array_head *list1,
                 const Array_head *list2) {
    if (list_result == NULL || *list_result == NULL || list1 == NULL ||
        list2 == NULL) {
        errno = EINVAL;
        return false;
    }
    if (list1->data_size != list2->data_size) {
        errno = EINVAL;
        return false;
    }
    if ((unsigned long)list1->count + (unsigned long)list2->count >
        SIZE_MAX / list1->data_size) {
        errno = EINVAL;
        return false;
    }
    if (!array_create(list_result, list1->data_size)) {
        errno = ENOMEM;
        return false;
    }
    if (!(list1->count + list2->count)) {
        return true;
    }
    uint8_t *data = (uint8_t *)malloc((*list_result)->data_size);
    if (data == NULL) {
        array_delete(list_result);
        errno = ENOMEM;
        return false;
    }
    for (size_t i = 0; i < list1->count; i++) {
        array_get(list1, data, i);
        if (!array_append(*list_result, data)) {
            free(data);
            array_delete(list_result);
            return false;
        }
    }
    for (size_t i = 0; i < list2->count; i++) {
        array_get(list2, data, i);
        if (!array_append(*list_result, data)) {
            free(data);
            array_delete(list_result);
            return false;
        }
    }
    free(data);
    return true;
}

long array_search(const Array_head *list, const void *data,
                  const Array_classify search, Array_head **result) {
    if (list == NULL || data == NULL || search == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (result != NULL) {
        if (!array_create(result, sizeof(size_t))) {
            errno = ENOMEM;
            return -1;
        }
    }
    for (size_t i = 0; i < list->count; i++) {
        if (search(data, (void *)(list->array + i * list->data_size))) {
            if (result == NULL) return (long)i;
            if (!array_push(*result, &i)) {
                if (result != NULL) array_delete(result);
                return -1;
            }
        }
    }
    return -1;
}

bool array_sort(const Array_head *list, const Array_classify sort) {
    if (list == NULL || sort == NULL) {
        errno = EINVAL;
        return false;
    }
    Array_head *b;
    if (!array_create(&b, list->data_size)) {
        return false;
    }
    for (size_t i = 0; i < list->count; i++)
        if (!array_append(b, list->array + i * list->data_size)) {
            array_delete(&b);
            return false;
        }
    array_split_merge(list, b, 0, list->count, sort);
    array_delete(&b);
    return true;
}

void array_split_merge(const Array_head *b, const Array_head *a, size_t i_begin,
                       size_t i_end, const Array_classify sort) {
    if (i_end - i_begin <= 1) {
        return;
    }
    const size_t i_middle = (i_end + i_begin) / 2;
    array_split_merge(a, b, i_begin, i_middle, sort);
    array_split_merge(a, b, i_middle, i_end, sort);

    array_merge_down(b, a, i_begin, i_middle, i_end, sort);
}

void array_merge_down(const Array_head *b, const Array_head *a, size_t i_begin,
                      size_t i_middle, size_t i_end,
                      const Array_classify sort) {
    size_t i = i_begin;
    size_t j = i_middle;
    for (size_t k = i_begin; k < i_end; k++) {
        if (i < i_middle && (j >= i_end || sort(a->array + i * a->data_size,
                                                a->array + j * a->data_size))) {
            array_position_set(b, k, a->array + i * a->data_size);
            i++;
        } else {
            array_position_set(b, k, a->array + j * a->data_size);
            j++;
        }
    }
}

void array_delete(Array_head **list) {
    if (list == NULL || *list == NULL) return;
    if ((*list)->array != NULL) free((void *)(*list)->array);
    free(*list);
}
inline void array_count_edit(const Array_head *list, size_t count) {
    *(size_t *)(&list->count) = count;
}
inline void array_data_size_edit(const Array_head *list, size_t data_size) {
    *(size_t *)(&list->data_size) = data_size;
}
inline void array_array_edit(const Array_head *list, uint8_t *start) {
    *(uint8_t **)(&list->array) = start;
}
bool array_resize(const Array_head *list, size_t size_new) {
    size_new *= MEMORY_BLOCK;
    uint8_t *pointer =
        (uint8_t *)realloc((void *)list->array, size_new * list->data_size);
    if (pointer == NULL) {
        errno = ENOMEM;
        return false;
    }
    array_array_edit(list, pointer);
    for (size_t i = list->count * list->data_size;
         i < size_new * list->data_size; i++)
        ((uint8_t *)(list->array))[i] = 0;
    return true;
}
inline void array_position_set(const Array_head *list, size_t position,
                               const void *data) {
    for (size_t i = 0; i < list->data_size; i++) {
        ((uint8_t *)list->array)[position * list->data_size + i] =
            ((const uint8_t *)data)[i];
    }
}
inline void array_position_get(const Array_head *list, size_t position, void *data) {
    memcpy(data, &list->array[position * list->data_size], list->data_size);
}