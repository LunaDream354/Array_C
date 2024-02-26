#include <stdio.h>

#include "array.h"

int list_items1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int list_items2[] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

bool search(const void *to_be_search, const void *data);
int main(void) {
    errno = 0;
    Array_head *list1;
    array_create(&list1,sizeof(int));
    for (size_t i = 0; i < sizeof(list_items1) / sizeof(list_items1[0]); i++)
        array_append(list1, &list_items1[i]);
    Array_head *list2;
    array_create(&list2,sizeof(int));
    for (size_t i = 0; i < sizeof(list_items2) / sizeof(list_items2[0]); i++)
        array_append(list2, &list_items2[i]);
    // Array_head *list_result;
    // array_merge(&list_result, list2, list1);
    // array_sort(list_result, search);
    for (size_t i = 0; i < list2->count; i++) {
        int value;
        array_get(list2, &value, i);
        printf("%d\n", value);
    }
    return 0;
}

bool search(const void *to_be_search, const void *data) {
    return *((int *)to_be_search) > *((int *)data);
}