#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

uint count_bits_1(unsigned char byte);
void print_byte_bin(unsigned char byte);
uint print_block(unsigned num, bool hex);
void get_array_slice(unsigned char *array, char *sliced_array, uint from, uint to);
u_int32_t check_block_in_bitmap(u_int32_t block_id);