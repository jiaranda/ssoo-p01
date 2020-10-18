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
  new_file->total_read_bytes = 0;

  // without a valid path, returns invalid
  if (dir_exists(path) == -1)
  {
    fprintf(stderr, "ERROR: os_open. Path does not exist.\n");
    new_file->filetype = INVALID;
    return new_file;
  }

  // if file exists in w mode or doesn't in r mode, returns invalid
  if ((mode == 'w' && os_exists(path)) || (mode == 'r' && !os_exists(path)))
  {
    fprintf(stderr, "ERROR: os_open. Invalid operation.\n");
    new_file->filetype = INVALID;
    return new_file;
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
  char file_name[29] = {0};
  int block_is_full = 1;

  // load disk pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    fprintf(stderr, "ERROR: os_open. Error while reading disk.\n");
    return 0;
  }

  // look for file
  for (int i = 0; i < 64; i++)
  {
    fread(entry, 32, 1, fp);
    entry_type = entry[0] >> 6;
    if (entry_type)
    {
      get_array_slice(entry, entry_name, 3, 31);
      entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
    }

    if (entry_type == OS_DIRECTORY && !strcmp(entry_name, next_dir))
    {
      next_dir = strtok(NULL, "/");
      i = -1;
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
      fseek(fp, 2048 * entry_pointer, SEEK_SET);
      for (int k = 0; k < 64; k++)
      {
        fread(entry, 32, 1, fp);
        entry_type = entry[0] >> 6;
        if (!entry_type)
        {
          block_is_full = 0;
          break;
        }
      }
    }
  }
  fclose(fp);

  // handle write mode
  if (mode == 'w')
  {
    // handle full address block
    if (block_is_full)
    {
      fprintf(stderr, "ERROR: os_open. The address block is full.\n");
      new_file->filetype = INVALID;
      return new_file;
    }

    // get first empty block from bitmap and use it
    uint32_t empty_block = get_empty_block_pointer(1);

    // handle a completely filled disk
    if (!empty_block)
    {
      fprintf(stderr, "ERROR: os_open. Disk is completely full.\n");
      new_file->filetype = INVALID;
      return new_file;
    }

    // write new entry
    unsigned char new_entry_head[3];
    uint32_t tmp_head;
    tmp_head = OS_FILE << 22 | empty_block;    // 4194368 -> 01000000 000000 01000000
    new_entry_head[0] = tmp_head >> 16;        // esto funciona, es 64
    new_entry_head[1] = (tmp_head >> 8) & 255; // WIP
    new_entry_head[2] = tmp_head & 255;

    FILE *fpw = fopen(disk_path, "rb+");
    if (!fpw)
    {
      fprintf(stderr, "ERROR: os_open. Couldn't write on file.\n");
      new_file->filetype = INVALID;
      return new_file;
    }

    // find empty entry address
    uint32_t dir_block_number = dir_exists(path);
    printf("dir block number: %d\n", dir_block_number);
    int entry_number = get_empty_entry(dir_block_number);
    if (entry_number == -1)
    {
      printf("ERRROR: os_open. Couldn't find empty entry\n");
      new_file->filetype = INVALID;
      fclose(fpw);
      return new_file;
    }

    // write new entry
    fseek(fpw, dir_block_number * 2048 + entry_number * 32, SEEK_SET);
    fwrite(new_entry_head, 3, 1, fpw);
    fwrite(file_name, 29, 1, fpw);
    fclose(fpw);

    // initialize new osFile
    new_file->filetype = OS_FILE;
    new_file->inode = empty_block;
    strcpy(new_file->name, file_name);
  }
  return new_file;
}