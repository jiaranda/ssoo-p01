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

void get_array_slice(unsigned char *array, char *sliced_array, uint from, uint to)
{
  uint index = 0;
  for (uint i = from; i < to; i++)
  {
    sliced_array[index] = array[i];
    index++;
  }
}

u_int32_t check_block_in_bitmap(u_int32_t block_id)
{
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    printf("No se pudo leer el archivo\n");
    return 0;
  }
  uint block_offset = 1;
  u_int32_t bitmap_block = (u_int32_t)((block_id) / ((1 << 11) * 8));
  // printf("Bloque: %d, Bloque de bitmap: %d\n", block_id, bitmap_block);
  u_int32_t byte_inside_bitmap_block = (u_int32_t)((block_id - bitmap_block * 2048 * 8) / 8);
  // printf("Byte dentro del bloque: %d\n", byte_inside_bitmap_block);
  u_int32_t bit_inside_byte = block_id % 8;
  // printf("Bit dentro del byte: %d\n", bit_inside_byte);
  fseek(fp, 2048 * (bitmap_block + 1), SEEK_SET);

  unsigned char byte[1];
  fseek(fp, 2048 * (block_offset + bitmap_block) + byte_inside_bitmap_block, SEEK_SET);
  fread(byte, 1, 1, fp);
  // printf("asd: %d\n", (u_int32_t)byte[0]);
  u_int32_t is_used = (u_int32_t)(byte[0] >> (7 - bit_inside_byte) & 1);
  // printf("is used: %d\n", is_used);
  return is_used;
  fclose(fp);
}