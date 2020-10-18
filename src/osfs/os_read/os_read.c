#include "os_read.h"

extern char *disk_path;
extern uint32_t READ_BUFFER_SIZE;

int os_read(osFile *file_desc, void *buffer, int nbytes)
{
  // open file pointer
  FILE *fp = fopen(disk_path, "rb");
  if (!fp)
  {
    fprintf(stderr, "ERROR: os_read. Couldn't open file\n");
    return 0;
  }
  uint64_t file_size = read_file_size(file_desc->inode + 1, 7);
  uint64_t disk_address = file_desc->inode * 2048 + 8;
  uint64_t bytes_goal = file_size - file_desc->total_read_bytes;
  if (nbytes < file_size - file_desc->total_read_bytes)
  {
    bytes_goal = nbytes;
  }

  // define bytes to read
  int bytes_to_read = bytes_goal;
  if (509 * 512 * 2048 < bytes_to_read)
  {
    bytes_to_read = 509 * 512 * 2048;
  }

  // read first index block
  if (file_desc->total_read_bytes < 509 * 512 * 2048)
  {
    read_index_block(disk_address, buffer, bytes_to_read);
    bytes_goal -= bytes_to_read;
  }

  fseek(fp, file_desc->inode * 2048 + 2044, SEEK_SET);
  disk_address = fread(fp, 4, 1, "rb") * 2048;
  while (bytes_goal > 0)
  {
    bytes_to_read = bytes_goal;
    if (511 * 512 * 2048 < bytes_to_read)
    {
      bytes_to_read = 511 * 512 * 2048;
    }
    read_index_block(disk_address, buffer, bytes_to_read);
    bytes_goal -= bytes_to_read;
    fseek(fp, disk_address + 2044, SEEK_SET);
    disk_address = fread(fp, 4, 1, "rb") * 2048;
  }

  fclose(fp);
}
// Bloque índice
// - (1) hardlinks
// - (7) tamaño
// - (509 * 4) punteros a bloques de direccionamiento indirecto simple
// - (4) puntero a otro bloque índice

// Bloque índice que no es el primero
// - (511 * 4) punteros a bloques de direccionamiento indirecto simple
// - (4) puntero a otro bloque índice
// cada bloque índice tiene

// Bloque de direccionamiento indirecto simple
// - (4 * 512)B puntero a bloque de datos

// Bloque de datos
// - (2048)B puros datos