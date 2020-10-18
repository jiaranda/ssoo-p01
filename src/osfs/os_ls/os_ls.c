#include "os_ls.h"
#include <string.h>

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

void os_ls(char *path)
{
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    fprintf(stderr, "ERROR: No se pudo leer el disco.\n");
    return;
  }

  // parse path
  char tmp_path[29];
  strcpy(tmp_path, path);
  char *next_dir;
  next_dir = strtok(tmp_path, "/");
  if (!next_dir)
  {
    next_dir = strtok(NULL, "/");
  }

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
    if (next_dir)
    {
      if (entry_type)
      {
        get_array_slice(entry, entry_name, 3, 31);
      }
      if (entry_type == 1 && !strcmp(entry_name, next_dir)) // archivo --> termina
      {
        fprintf(stderr, "ERROR: no se puede ejecutar comando os_ls sobre un archivo.\n");
        fclose(fp);
        return;
      }
      if (entry_type == 2 && !strcmp(entry_name, next_dir)) // directorio
      {
        next_dir = strtok(NULL, "/");
        i = 0;
        entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
        fseek(fp, 2048 * entry_pointer, SEEK_SET);
      }
    }
    else
    {
      if (entry_type)
      {
        get_array_slice(entry, entry_name, 3, 31);
        fprintf(stderr, "%s\n", entry_name);
      }
      if (i == 63)
      {
        fclose(fp);
        return;
      }
    }
  }
  fprintf(stderr, "ERROR: path no encontrado.\n");
};