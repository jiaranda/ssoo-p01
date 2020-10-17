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

void indent(int level)
{
  for (int j = 0; j < level; j++)
  {
    printf("-");
  }
}

void print_directory_tree(FILE *fp, uint32_t block_pointer, int level)
{
  fseek(fp, 2048 * block_pointer, SEEK_SET);
  unsigned char entry[32];
  int entry_type;
  uint32_t entry_pointer;
  char entry_name[29];
  for (int i = 0; i < 32; i++)
  {
    fread(entry, 32, 1, fp);
    entry_type = entry[0] >> 6;
    if (entry_type)
    {
      entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
      get_array_slice(entry, entry_name, 3, 31);
      indent(level);
      printf("%d\t%d\t%s\n", entry_type, entry_pointer, entry_name);
      // if (entry_type == OS_DIRECTORY)
      // {
      //     print_directory_tree(fp, entry_pointer, level + 1);
      // }
    }
  }
}

uint32_t get_empty_block_pointer(char* filename, bool use_block)
{
  FILE* fp = fopen(filename, "rb");
  fseek(fp, 2048, SEEK_SET);
  uint32_t byte_count = 0;
  unsigned char byte[0];
  fread(byte, 1, 1, fp);
  while ((uint32_t)byte[0] == 255)
  {
    fread(byte, 1, 1, fp);
    byte_count++;
  }

  for (uint32_t i = 0; i < 8; i++)
  {
    if (byte[0] & (128 >> i))
    {
      if (use_block)
      {
        printf("%d\n", byte_count);
        FILE* fp_write = fopen(filename, "wb");
        fseek(fp_write, 2048 + byte_count, SEEK_SET);
        byte[0] = byte[0] | (1 << (7-i));
        fwrite(byte[0], 1, 1, fp_write);
        fclose(fp_write);
      }
      fclose(fp);
      return (byte_count * 8) + i;
    }
  }
  fclose(fp);
  return 0;
}