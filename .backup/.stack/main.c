#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int* array = (int*)malloc(10 * sizeof(int));
    int i = 0;
    for (; i < 10; ++i) {
        array[i] = i + 1;
    }
    /* sizeof(array_pointer) / sizeof(array[0]) */
    const int array_size = sizeof(array) / sizeof(array[0]);

    char* str = (char*)malloc(10 * sizeof(char));
    printf("str: %s\n", str);
    str = "abcdefghijklmn";
    printf("str: %s\n", str);

    const int str_size = sizeof(str) / sizeof(str[0]);

    printf("array_size = %d, str_size = %d\n", array_size, str_size);

    return 0;
}

