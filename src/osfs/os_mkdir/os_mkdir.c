#include "os_mkdir.h"
#include <string.h>
#include <stdint.h>

extern char* disk_path;
extern uint32_t BLOCK_NUM_MASK;

int os_mkdir(char* path)
{
  /* 
  Buscar y llegar hasta el  path que corresponde (el padre)
  si ya existe con el mismo nombre
    retornar error (i)
  si esta disponible
    buscar la primera entrada disponible del padre y escribir dentro
      el tipo --> 2 (directorio)
      buscar en el bitmap un directorio libre, dejarlo ocupado, recibir el puntero y guardarlo
      el nombre
    
  */
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
        // if termino de recorrer un bloque de directorio y no coincide el nombre
        char* next_final_dir = strtok(NULL, "/");
        if (!next_final_dir)
        {
          // if no sigue nada mas --> mkdir yaaaaay
          fseek(fp, 2048 * entry_pointer, SEEK_SET); //vuelvo al principio
          for (int k = 0; k < 64; k++)
          {
            fread(entry, 32, 1, fp);
            entry_type = entry[0] >> 6;
            if (!entry_type) // esta vacio
            {
              // FILE* fp_write = fopen(disk_path, "wb");
              // // Escribo el directorio como ocupado por directorio --> 01
              // unsigned char new_entry_type = 1 << 22;
              // // Busco espacio disponible en el bitmap, lo dejo como ocupado y obtengo pointer
              // // Escribo poinyer entre los bits 3 y 22
              // uint32_t new_pointer; // la wea del vicho
              // unsigned char pico =  new_entry_type | new_pointer; // 3 primeros bytes definitivos
              // int tamano_nombre = sizeof(entry_name); // cantidad de bytes del nombre
              // unsigned char arreglo_29[29]
              // // 2 bits de tipo de archivoo
              // // 22 bits de pointer
              // // 29 bytes de nombre del directorio

              



              // Escribo el nombre
            }
            if (k == 63)
            {
              fprintf(stderr, "ERROR: el bloque asociado al path padre no contiene entradas disponibles.\n");
              fclose(fp);
              return 0;
            }       
          }
                   
        }
        else
        {
          // if sigue --> ERROR: path malo
          fprintf(stderr, "ERROR: el path está incorrecto.\n");
          fclose(fp);
          return 0;
        }        
      } 
    }
    else
    {
      // 
      if (entry_type)
      {
        get_array_slice(entry, entry_name, 3, 31);
        fprintf(stderr, "%s\n", entry_name);
      }
      if (i == 63) return;
    }
  }
  printf("Ola hice os_mkdir\n");
  return 1;
}