#include "os_exists.h"
#include <string.h>
#include <stdint.h>

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

uint search(char *name, uint block_pointer, FILE *fp)
{
  unsigned char entry[32];
  uint entry_count = 0;
  // printf("%u\n", block_pointer);
  fseek(fp, block_pointer * 2048, SEEK_SET);
  while (entry_count < 64)
  {
    fread(entry, 32, 1, fp);
    if (entry[0] >> 6 == 0)
    {
      printf("invalido\n");
    }
    else
    {
      unsigned char entry_name[29];
      get_array_slice(entry, entry_name, 3, 31);
      int diff = strcmp(name, (char *)entry_name);
      printf("buscando en %s\n", entry_name);
      if (!diff) // son iguales
      {
        // printf("lo encontre\n");
        unsigned char sliced_entry[3];
        get_array_slice(entry, sliced_entry, 0, 2);
        // sliced_entry[0] = sliced_entry[0] >> 2;
        // 0011111111 11111111 111111
        return (sliced_entry[0] << 16 | sliced_entry[1] << 8 | sliced_entry[2]) & BLOCK_NUM_MASK;
      }
    }
    entry_count++;
  }
  return 0;
}

int os_exists(char *path)
{
  char new_path[29];
  strcpy(new_path, path);
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    // printf("No se pudo leer el archivo\n");
    return 0;
  }
  char *arg;
  arg = strtok(new_path, "/");
  uint block_pointer = 0;
  while (arg != NULL)
  {
    block_pointer = search(arg, block_pointer, fp);
    // revisar bitmap
    check_block_in_bitmap(block_pointer);
    if (block_pointer == 0)
    {
      fclose(fp);
      return 0;
    }
    arg = strtok(NULL, "/");
  }

  fclose(fp);
  return 1;
}