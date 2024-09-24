#include "../philosophers.h"

long ft_atoi(char *s)
{
    int i;
    long res;

    i = 0;
    res = 0;
    while (s[i])
    {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return (res);
}