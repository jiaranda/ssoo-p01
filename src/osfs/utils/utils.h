#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum filetype
{
  TYPE_FILE,
  TYPE_DIRECTORY,
  TYPE_LINK
} Filetype;

typedef struct OSFile
{
  char name[255];
  uint inode;
  Filetype filetype;
  char path[255];
  uint disk_address;
  uint size; // bytes
} osFile;

uint count_bits_1(unsigned char byte);
void print_byte_bin(unsigned char byte);
uint print_block(unsigned num, bool hex);
void get_array_slice(unsigned char *array, unsigned char *sliced_array, uint from, uint to);
