#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

uint count_bits_1(unsigned char byte);
void print_byte_bin(unsigned char byte);
uint print_block(unsigned num, bool hex);
void get_array_slice(unsigned char *array, unsigned char *sliced_array, uint from, uint to);
int check_block_in_bitmap(uint block_id);