#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>


int main(void) {
    char constcr1[] = "Select an Option:";
    char constcr2[80];
    char buffer[80];
    snprintf(constcr2, 80, "To Call (match current bid) bid %d. - To Fold, bid -1.", 60000);


    printf("st1 = %s\n", constcr1);
    printf("st2 = %s\n", constcr2);
    return 0;

}