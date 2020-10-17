#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "os_open.h"
#include "../os_exists/os_exists.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

osFile *os_open(char *path, char mode)
{
  // initialize new_file
  osFile *new_file = calloc(1, sizeof(osFile));

  // without a valid path, returns invalid
  if (!dir_exists(path))
  {
    fprintf(stderr, "ERROR: path does not exist.\n");
    new_file->filetype = INVALID;
    return new_file;
  }

  // if file exists in w mode or doesn't in r mode, returns invalid
  if ((mode == 'w' && os_exists(path)) || (mode == 'r' && !os_exists(path)))
  {
    new_file->filetype = INVALID;
    return new_file;
  }

  // load disk pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    printf("No se pudo acceder al archivo de disco\n");
    return 0;
  }

  // path parsing
  char tmp_path[29];
  strcpy(tmp_path, path);
  char *next_dir;
  next_dir = strtok(tmp_path, "/");
  if (!next_dir)
  {
    next_dir = strtok(tmp_path, "/");
  }

  // entry attributes
  unsigned char entry[32];
  int entry_type;
  uint32_t entry_pointer;
  char entry_name[29];
  char file_name[29];

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
    }

    if (i == 63)
    {
      strcpy(file_name, next_dir);
    }
  }

  printf("El nombre del archivo es: %s\n", file_name);

  // mode = 'w'
  if (mode == 'w' && !os_exists(path))
  {
    // get first empty block from bitmap
    uint32_t empty_block = get_empty_block_pointer(fp);

    // disk is completely full
    if (!empty_block)
    {
      new_file->filetype = INVALID;
      return new_file;
    }

    // write new entry
    char new_entry[32];
    int new_entry_type = 1 << 22;
    new_entry[0] = new_entry_type | empty_block;
    printf("entry[0]: %d\n", entry[0]);
    strcpy(&new_entry[3], file_name);

    // create
    new_file->filetype = OS_FILE;
    new_file->inode = empty_block;
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