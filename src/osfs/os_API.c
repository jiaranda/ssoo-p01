#include "os_API.h"
#include <math.h>

char *disk_path;

void os_mount(char *diskname)
{
  disk_path = diskname;
}

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