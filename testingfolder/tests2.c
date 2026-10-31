#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

int main(void) {
    int max_n = 5;
    for (int i = 0; i< 100; i++) {
        printf("%d\n", i-1%max_n);
    }
    printf("\a");
}