#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum filetype
{
  INVALID,
  OS_FILE,
  OS_DIRECTORY,
  OS_LINK
} Filetype;

typedef struct OSFile
{
  Filetype filetype;
  uint32_t inode; // disk address
  char name[29];
} osFile;

uint count_bits_1(unsigned char byte);
void print_byte_bin(unsigned char byte);
uint print_block(unsigned num, bool hex);
void get_array_slice(unsigned char *array, char *sliced_array, uint from, uint to);
int check_block_in_bitmap(uint block_id);
