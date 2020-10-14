#include "os_bitmap.h"
#include <math.h>

void os_bitmap(unsigned num, bool hex)
{
    if (num > 64)
    {
        return;
    }
    if (num != 0)
    {
        print_block(num, hex);
        fprintf(stderr, "\n");
    }
    else
    {
        uint busy_blocks = 0;
        for (int i = 1; i <= 64; i++)
        {
            busy_blocks += print_block(i, hex);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Cantidad de bloques ocupados: %d\n", busy_blocks);
        fprintf(stderr, "Cantidad de bloques libres: %d\n", (uint)pow(2, 20) - busy_blocks);
    }
}