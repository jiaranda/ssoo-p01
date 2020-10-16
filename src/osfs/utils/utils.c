#include "utils.h"

extern char *disk_path;

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

// arg = strtok(path, "/");
// while (arg != NULL)
// {

//     arg = strtok(NULL, "/");
// }

void get_array_slice(unsigned char *array, unsigned char *sliced_array, uint from, uint to)
{
  uint index = 0;
  for (uint i = from; i < to; i++)
  {
    sliced_array[index] = array[i];
    index++;
  }
}

int check_block_in_bitmap(uint block_id)
{
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    printf("No se pudo leer el archivo\n");
    return 0;
  }
  uint block_offset = 1;
  uint bitmap_block = (uint)((block_id) / ((1 << 11) * 8));
  printf("Bloque: %d, Bloque de bitmap: %d\n", block_id, bitmap_block);

  // unsigned char bytes[2047];
  // fseek(fp, 2048 * block_offset, SEEK_SET);
  // fread(bytes, 2048, 1, fp);
  return 1;
  fclose(fp);
}