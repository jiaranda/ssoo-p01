#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "os_open.h"
#include "../os_exists/os_exists.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

uint32_t get_empty_block_pointer(FILE *fp)
{
  fseek(fp, 2048, SEEK_SET);
  uint32_t byte_count = 0;
  unsigned char byte[1];
  fread(byte, 1, 1, fp);
  while ((uint32_t)byte[0] != 255)
  {
    fread(byte, 1, 1, fp);
    byte_count++;
  }

  for (uint32_t i = 0; i < 8; i++)
  {
    if (byte[0] & (128 >> i))
    {
      return (byte_count * 8) + i;
    }
  }
  return 0;
}

osFile *os_open(char *path, char mode)
{
  // load disk pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    printf("No se pudo acceder al archivo de disco\n");
    return 0;
  }

  // parse path
  char tmp_path[29];
  strcpy(tmp_path, path);
  char *next_dir;
  next_dir = strtok(tmp_path, "/");
  if (!next_dir)
  {
    next_dir = strtok(tmp_path, "/");
  }

  // initialize new_file
  osFile *new_file = calloc(1, sizeof(osFile));

  // osFile attributes
  unsigned char entry[32];
  int entry_type;
  uint32_t entry_pointer;
  char entry_name[29];

  // look for file
  for (int i = 0; i < 64; i++)
  {
    fread(entry, 32, 1, fp);
    entry_type = entry[0] >> 6;
    if (entry_type)
    {
      get_array_slice(entry, entry_name, 3, 31);
    }

    if (entry_type == OS_DIRECTORY && !strcmp(entry_name, next_dir))
    {
      next_dir = strtok(NULL, "/");
      i = 0;
      entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
      fseek(fp, 2048 * entry_pointer, SEEK_SET);
    }

    if (entry_type == OS_FILE && !strcmp(entry_name, next_dir))
    {
      new_file->filetype = entry_type;
      new_file->inode = entry_pointer;
      strcpy(new_file->name, entry_name);
      // strcpy(new_file->path, path);
    }
  }

  char mode_tmp[1];
  strcpy(mode_tmp, &mode);
  if (!strcmp(mode_tmp, "w") && !os_exists(path))
  {
    // get first empty block from bitmap
    printf("EMPTY BLOCK: %u\n", get_empty_block_pointer(fp));

    // write new entry

    // return
    new_file->filetype = OS_FILE;
    new_file->inode = entry_pointer;
    strcpy(new_file->name, next_dir);
  }

  // close file
  fclose(fp);

  return new_file;
}

int os_close(osFile *file_desc)
{
  free(file_desc);
  return 0;
}