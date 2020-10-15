#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "os_open.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

osFile *os_open(char *path, char mode)
{
  // load disk pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    printf("No se pudo leer el archivo\n");
    return 0;
  }

  // get root dir block data
  for (int i = 0; i < 32; i++)
  {
    unsigned char entry[32];
    fread(entry, 32, 1, fp);
    int entry_type = entry[0] >> 6;
    if (entry_type)
    {
      unsigned char entry_name[29];
      get_array_slice(entry, entry_name, 3, 31);
      uint32_t entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
      printf("%d\t%s\n", entry_pointer, entry_name);
    }
    else
    {
      printf("Invalid entry\n");
    }
  }

  // fseek(fp, 2048 * block_num, SEEK_SET);
  // unsigned char block[2047];
  // fread(block, 2048, 1, fp);
  // int entry_type = block[0] >> 6;
  // printf("%d\n", entry_type);

  // parse path
  // char *arg;
  // arg = strtok(path, "/");
  // while (arg != NULL)
  // {
  //   arg = strtok(NULL, "/");
  // }

  // close file
  fclose(fp);

  // initialize new_file
  osFile *new_file = calloc(1, sizeof(osFile));
  return new_file;
}

int os_close(osFile *file_desc)
{
  free(file_desc);
  return 0;
}