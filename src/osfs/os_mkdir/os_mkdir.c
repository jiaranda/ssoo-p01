#include <string.h>
#include "os_mkdir.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

int os_mkdir(char *path)
{
  // load disk pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    fprintf(stderr, "[ERROR] os_mkdir: could not open disk.\n");
    return 0;
  }

  // parse path
  char tmp_path[29];
  strcpy(tmp_path, path);
  char file_name[29] = {0};
  int block_is_full = 1;
  char *next_dir;
  next_dir = strtok(tmp_path, "/");
  if (!next_dir)
  {
    next_dir = strtok(NULL, "/");
  }

  // osFile attributes
  unsigned char entry[32];
  int entry_type;
  uint32_t entry_pointer = 0;
  char entry_name[29];

  for (int i = 0; i < 64; i++)
  {
    fread(entry, 32, 1, fp);

    if (!next_dir)
    {
      printf("[ERROR] os_mkdir: the directory already exists\n");
      fclose(fp);
      return 0;
    }
    
    entry_type = entry[0] >> 6;
    if (entry_type)
    {
      get_array_slice(entry, entry_name, 3, 31);
    }
    if (entry_type == 1 && !strcmp(entry_name, next_dir)) // archivo --> termina
    {
      fprintf(stderr, "[ERROR] os_mkdir: cannot execute os_mkdir with filename\n");
      fclose(fp);
      return 0;
    }
    if (entry_type == 2 && !strcmp(entry_name, next_dir)) // directorio
    {
      next_dir = strtok(NULL, "/");
      i = 0;
      entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
      fseek(fp, 2048 * entry_pointer, SEEK_SET);
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
      fclose(fp);
      char *next_final_dir = strtok(NULL, "/");
      if (!next_final_dir)
      {
        // --- Si tiene un punto (···/actual.algo), error de archivo
        if (strchr(next_dir, '.'))
        {
          fprintf(stderr, "[ERROR] os_mkdir: cannot create directory with file's name.\n");
          return 0;
        }
        // --- Si no --> yaaaay hago mkdir
        else
        {
          if (block_is_full)
          {
            fprintf(stderr, "[ERROR] os_mkdir: The address block is full.\n");
            // fclose(fp);
            return 0;
          }
          // get first empty block from bitmap and use it
          uint32_t empty_block = get_empty_block_pointer(1);
          // handle a completely filled disk
          if (!empty_block)
          {
            fprintf(stderr, "[ERROR] os_mkdir: Disk is completely full.\n");
            return 0;
          }
          // write new entry
          unsigned char new_entry_head[3];
          uint32_t tmp_head;
          tmp_head = OS_DIRECTORY << 22 | empty_block;  
          new_entry_head[0] = tmp_head >> 16;
          new_entry_head[1] = (tmp_head >> 8) & 255;
          new_entry_head[2] = tmp_head & 255;
          FILE *fpw = fopen(disk_path, "rb+");
          if (!fpw)
          {
            fprintf(stderr, "[ERROR] os_mkdir: Couldn't write on file.\n");
            return 0;
          }
          // find empty entry address
          uint32_t dir_block_number = dir_exists(path);
          int entry_number = get_empty_entry(dir_block_number);
          if (entry_number == -1)
          {
            printf("[ERRROR] os_mkdir: Couldn't find empty entry\n");
            return 0;
          }
          // write new entry
          fseek(fpw, dir_block_number * 2048 + entry_number * 32, SEEK_SET);
          fwrite(new_entry_head, 3, 1, fpw);
          fwrite(file_name, 29, 1, fpw);
          fclose(fpw);
          return 1;
        }
      }
      else
      {
        // - Si quedan mas nombres (···/ACTUAL/foo/bar) path malo
        fprintf(stderr, "[ERROR] os_mkdir: incorrect format for given path.\n");
        return 0;
      }
    }
  }
  return 0;
}