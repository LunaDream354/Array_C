#include <stdio.h>

#include "dynamic_array.h"

int list_items1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int list_items2[] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
int main(void) {
    errno = 0;
    Array_head *list1 = array_create(sizeof(int));
    for (size_t i = 0; i < sizeof(list_items1) / sizeof(list_items1[0]); i++)
        array_append(list1, &list_items1[i]);
    Array_head *list2 = array_create(sizeof(int));
    for (size_t i = 0; i < sizeof(list_items2) / sizeof(list_items2[0]); i++)
        array_append(list2, &list_items2[i]);
    Array_head *list_result = array_merge(list1, list2);
    for (size_t i = 0; i < list_result->count; i++) {
        int value;
        array_get(list_result, &value, i);
        printf("%d\n", value);
    }
    return 0;
}