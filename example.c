
#include <stdio.h>

__attribute__((noinline)) int add(int a, int b)
{
    return a + b;
}

__attribute__((noinline)) int read_value(const int *p)
{
    return *p;
}

__attribute__((noinline)) void write_value(int *p, int value)
{
    *p = value;
}

int main(void)
{
    int x = 0;
    write_value(&x, 40);
    int result = add(read_value(&x), 2);
    printf("%d\n", result);
    return 0;
}
