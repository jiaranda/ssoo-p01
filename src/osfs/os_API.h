#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "os_bitmap/os_bitmap.h"
#include "os_mount/os_mount.h"
#include "os_open/os_open.h"
#include "os_exists/os_exists.h"
#include "os_ls/os_ls.h"
#include "os_rm/os_rm.h"
#include "os_mkdir/os_mkdir.h"
#include "os_hardlink/os_hardlink.h"


// General Functions
int os_exists(char *path);

// File Management Functions
int os_read(osFile *file_desc, void *buffer, int nbytes);
int os_write(osFile *file_desc, void *buffer, int nbytes);
int os_rm(char *path);
int os_rmdir(char *path, bool recursive);
int os_unload(char *orig, char *dest);
int os_load(char *orig);