#ifndef UTILS_H
#define UTILS_H
#include "./../utils/utils.h"
#endif

int os_rm(char *path);
int rm_first_block(int block_file);
int rm_indirect_block(uint32_t block_file);
int rm_first_block_2(int block_file);