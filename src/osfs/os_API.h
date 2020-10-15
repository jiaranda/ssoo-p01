#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "os_bitmap/os_bitmap.h"
#include "os_mount/os_mount.h"
#include "os_open/os_open.h"

// General Functions
int os_exists(char *path);
void os_ls(char *path);

// File Management Functions
int os_read(osFile *file_desc, void *buffer, int nbytes);
int os_write(osFile *file_desc, void *buffer, int nbytes);
int os_rm(char *path);
int os_hardlink(char *orig, char *dest);
int os_mkdir(char *path);
int os_rmdir(char *path, bool recursive);
int os_unload(char *orig, char *dest);
int os_load(char *orig);