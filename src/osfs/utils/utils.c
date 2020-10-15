#include <stdint.h>
#include "utils.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

uint count_bits_1(unsigned char byte)
{
    uint c;
    for (c = 0; byte; byte >>= 1)
    {
        c += byte & 1;
    }
    return c;
}

void print_byte_bin(unsigned char byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        fprintf(stderr, "%d", !!((byte << i) & 0x80));
    }
}

uint print_block(unsigned num, bool hex)
{
    FILE *fp = fopen(disk_path, "rb");
    if (!fp)
    {
        printf("No se pudo leer el archivo\n");
        return 0;
    }
    uint block_offset = num;

    unsigned char bytes[2047];
    fseek(fp, 2048 * block_offset, SEEK_SET);
    fread(bytes, 2048, 1, fp);
    uint busy_blocks = 0;
    for (int i = 0; i < 2048; i++)
    {
        if (hex)
        {
            fprintf(stderr, "%X", bytes[i]);
        }
        else
        {
            print_byte_bin(bytes[i]);
        }
        busy_blocks += count_bits_1(bytes[i]);
    }
    fclose(fp);
    return busy_blocks;
}

void get_array_slice(unsigned char *array, unsigned char *sliced_array, uint from, uint to)
{
    uint index = 0;
    for (uint i = from; i < to; i++)
    {
        sliced_array[index] = array[i];
        index++;
    }
}

void print_directory(FILE *fp, uint32_t block_pointer, int level)
{
    fseek(fp, 2048 * block_pointer, SEEK_SET);
    unsigned char entry[32];
    int entry_type;
    uint32_t entry_pointer;
    unsigned char entry_name[29];
    for (int i = 0; i < 32; i++)
    {
        fread(entry, 32, 1, fp);
        entry_type = entry[0] >> 6;
        if (entry_type)
        {
            entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
            get_array_slice(entry, entry_name, 3, 31);
            printf("%d\t%d\t%s\n", entry_type, entry_pointer, entry_name);
        }
    }
}