#include "stdio.h"
#include "math.h"
#define MAX 1000000000

int intensive_function()
{
    int i;
    double x = 0;
    for (i = 0, i < MAX, i++) {
        x = x + sin(i);
    }
}

int main()
{
    double x;

    x = intensive_function();
    printf("%f", x);
}
