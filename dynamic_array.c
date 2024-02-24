#include "dynamic_array.h"

#include <errno.h>

extern int errno;

#define MEMORY_BLOCK 20

void array_count_edit(Array_head *list, size_t count);
void array_data_size_edit(Array_head *list, size_t data_size);
void array_array_edit(Array_head *list, uint8_t *start);
bool array_resize(Array_head *list, size_t size_new);
void array_position_set(Array_head *list, size_t position, void *data);
void array_position_get(Array_head *list, size_t position, void *data);

Array_head *array_create(size_t data_size) {
    if (!data_size) {
        errno = EINVAL;
        return NULL;
    }
    Array_head *array = (Array_head *)calloc(1, sizeof(Array_head));
    if (array == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    array_data_size_edit(array, data_size);
    if (!array_resize(array, 1)) {
        return NULL;
    }
    return array;
}

Array_head *array_append(Array_head *list, void *data) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (list->count % MEMORY_BLOCK == 0) {
        bool resize_result = array_resize(list, list->count / MEMORY_BLOCK + 2);
        if (!resize_result) {
            return NULL;
        }
    }
    array_position_set(list, list->count, data);
    array_count_edit(list, list->count + 1);
    return list;
}

inline Array_head *array_push(Array_head *list, void *data) {
    return array_add_at(list, data, 0);
}

Array_head *array_add_at(Array_head *list, void *data, size_t position) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (position > list->count) {
        errno = EINVAL;
        return NULL;
    }
    if (list->count % MEMORY_BLOCK == MEMORY_BLOCK - 1) {
        bool resize_result =
            array_resize(list, (list->count / MEMORY_BLOCK + 2));
        if (!resize_result) {
            return NULL;
        }
    }
    for (size_t i = list->count; i > position; i--)
        array_position_set(list, i, list->array + (i - 1) * list->data_size);
    array_position_set(list, position, data);
    array_count_edit(list, list->count + 1);
    return list;
}
bool array_get(Array_head *list, void *data, size_t position) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }
    if (position >= list->count) {
        errno = EINVAL;
        return false;
    }
    array_position_get(list, position, data);
    return true;
}
bool array_pop(Array_head *list, void *data) {
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
        return NULL;
    }
    array_position_set(list, list->count - 1, pointer);
    free(pointer);
    array_count_edit(list, list->count - 1);
    return true;
}

bool array_remove_at(Array_head *list, size_t position, void *data) {
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
        return NULL;
    }
    array_position_set(list, list->count - 1, pointer);
    free(pointer);
    array_count_edit(list, list->count - 1);
    return data;
}

Array_head *array_merge(Array_head *list1, Array_head *list2) {
    if (list1 == NULL || list2 == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (list1->data_size != list2->data_size) {
        errno = EINVAL;
        return NULL;
    }
    if ((unsigned long)list1->count + (unsigned long)list2->count >
        SIZE_MAX / list1->data_size) {
        errno = EINVAL;
        return NULL;
    }
    Array_head *list_result = array_create(list1->data_size);
    if (list_result == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    if (!(list1->count + list2->count)) {
        return list_result;
    }
    // const size_t array_block_size =
    //     (list1->count + list2->count) / MEMORY_BLOCK +
    //     ((list1->count + list2->count) % MEMORY_BLOCK ? 1 : 0);
    // if (!array_resize(list_result, array_block_size)) {
    //     return NULL;
    // }
    uint8_t *data = (uint8_t *)malloc(list_result->data_size);
    if (data == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    for (size_t i = 0; i < list1->count; i++) {
        array_get(list1, data, i);
        array_append(list_result, data);
    }
    for (size_t i = 0; i < list2->count; i++) {
        array_get(list2, data, i);
        array_append(list_result, data);
    }
    free(data);
    return list_result;
}

inline void array_count_edit(Array_head *list, size_t count) {
    *(size_t *)(&list->count) = count;
}
inline void array_data_size_edit(Array_head *list, size_t data_size) {
    *(size_t *)(&list->data_size) = data_size;
}
inline void array_array_edit(Array_head *list, uint8_t *start) {
    *(uint8_t **)(&list->array) = start;
}
bool array_resize(Array_head *list, size_t size_new) {
    size_new *= MEMORY_BLOCK;
    uint8_t *pointer =
        (uint8_t *)realloc(list->array, size_new * list->data_size);
    if (pointer == NULL) {
        errno = ENOMEM;
        return false;
    }
    array_array_edit(list, pointer);
    for (size_t i = list->count * list->data_size;
         i < size_new * list->data_size; i++)
        list->array[i] = 0;
    return true;
}
inline void array_position_set(Array_head *list, size_t position, void *data) {
    for (size_t i = 0; i < list->data_size; i++) {
        list->array[position * list->data_size + i] = ((uint8_t *)data)[i];
    }
}
inline void array_position_get(Array_head *list, size_t position, void *data) {
    for (size_t i = 0; i < list->data_size; i++) {
        ((uint8_t *)data)[i] = list->array[position * list->data_size + i];
    }
}