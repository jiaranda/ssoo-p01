#include <stdint.h>

char *disk_path;
uint32_t BLOCK_NUM_MASK = 4194303; // 00111111 11111111 11111111
uint32_t READ_BUFFER_SIZE = 512;