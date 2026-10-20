#ifndef ARRAY_H
#define ARRAY_H

// removed the ; at the end of while (0) since the caller already has ; (e.g. array_push();)
// this prevents double semi-colon, since defines just substitute the function with its body
#define array_push(array, value)                                              \
    do {                                                                      \
        (array) = array_hold((array), 1, sizeof(*(array)));                   \
        (array)[array_length(array) - 1] = (value);                           \
    } while (0)

void* array_hold(void* array, int count, int item_size);
int array_length(void* array);
void array_free(void* array);

#endif
