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
    fprintf(stderr, "ERROR: No se pudo leer el disco.\n");
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
      printf("[ERROR] mkdir: the directory already exists\n");
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
      fprintf(stderr, "ERROR: no se puede ejecutar comando os_mkdir sobre un archivo.\n");
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
      printf("ENTREEE\n");
      /* 
      Si termino de recorrer un directorio y no coincide nombre actual
      - Si nombre actual es el ultimo (···/actual)
      --- Si tiene un punto (···/actual.algo), error de archivo
      --- Si no --> yaaaay hago mkdir
      - Si quedan mas nombres (···/actual/foo/bar) path
      */
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
      // printf("fp before: %d\n", fp);
      fclose(fp);
      // printf("fp after: %d\n", fp);
      // printf("Estoy acaaaaa\n");

      char *next_final_dir = strtok(NULL, "/");
      if (!next_final_dir)
      {
        // --- Si tiene un punto (···/actual.algo), error de archivo
        if (strchr(next_dir, '.'))
        {
          // printf("%s", strchr(entry_name, '.'));
          fprintf(stderr, "ERROR: os_mkdir. cannot create directory with file's name.\n");
          // fclose(fp);
          return 0;
        }
        // --- Si no --> yaaaay hago mkdir
        else
        {
          if (block_is_full)
          {
            fprintf(stderr, "ERROR: os_mkdir. The address block is full.\n");
            // fclose(fp);
            return 0;
          }
          // get first empty block from bitmap and use it
          // fclose(fp);
          uint32_t empty_block = get_empty_block_pointer(1);

          // handle a completely filled disk
          if (!empty_block)
          {
            fprintf(stderr, "ERROR: os_mkdir. Disk is completely full.\n");
            // fclose(fp);
            return 0;
          }

          // write new entry
          unsigned char new_entry_head[3];
          uint32_t tmp_head;
          tmp_head = OS_DIRECTORY << 22 | empty_block;    // 4194368 -> 01000000 000000 01000000
          new_entry_head[0] = tmp_head >> 16;        // esto funciona, es 64
          new_entry_head[1] = (tmp_head >> 8) & 255; // WIP
          new_entry_head[2] = tmp_head & 255;
          printf("WA ESCRIBIR ENTRY\n");
          FILE *fpw = fopen(disk_path, "rb+");
          if (!fpw)
          {
            fprintf(stderr, "ERROR: os_mkdir. Couldn't write on file.\n");
            return 0;
          }
          // find empty entry address
          uint32_t dir_block_number = dir_exists(path);
          // printf("dir block number: %d\n", dir_block_number);
          int entry_number = get_empty_entry(dir_block_number);
          if (entry_number == -1)
          {
            printf("ERRROR: os_mkdir. Couldn't find empty entry\n");
            // fclose(fp);
            return 0;
          }
          // write new entry
          fseek(fpw, dir_block_number * 2048 + entry_number * 32, SEEK_SET);
          fwrite(new_entry_head, 3, 1, fpw);
          fwrite(file_name, 29, 1, fpw);
          fclose(fpw);
          // fclose(fp);
          return 1;

        }
      }
      else
      {
        // - Si quedan mas nombres (···/ACTUAL/foo/bar) path malo
        fprintf(stderr, "ERROR: el path está incorrecto.\n");
        // fclose(fp);
        return 0;
      }
    }
  }
  // fclose(fp);
  // printf("Ah taba el mkdir\n");
  return 0;
}