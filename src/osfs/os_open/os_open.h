#ifndef UTILS_H
#define UTILS_H
#include "./../utils/utils.h"
#endif

osFile *os_open(char *path, char mode);

int os_close(osFile *file_desc);
