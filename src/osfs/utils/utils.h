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
  char path[255];
  uint32_t size; // bytes
} osFile;

uint count_bits_1(unsigned char byte);
void print_byte_bin(unsigned char byte);
uint print_block(unsigned num, bool hex);
void get_array_slice(unsigned char *array, unsigned char *sliced_array, uint from, uint to);
uint32_t get_file_size(uint32_t inode);
void print_directory_tree(FILE *fp, uint32_t block_pointer, int level);
