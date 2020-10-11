#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct osFile_t
{

} osFile;

// General Functions
void os_mount(char *diskname);
void os_bitmap(unsigned num, bool hex);
int os_exists(char *path);
void os_ls(char *path);

// File Management Functions
osFile *os_open(char *path, char mode);
int os_read(osFile *file_desc, void *buffer, int nbytes);
int os_read(osFile *file_desc, void *buffer, int nbytes);
int os_write(osFile *file_desc, void *buffer, int nbytes);
int os_close(osFile *file_desc);
int os_rm(char *path);
int os_hardlink(char *orig, char *dest);
int os_mkdir(char *path);
int os_rmdir(char *path, bool recursive);
int os_unload(char *orig, char *dest);
int os_load(char *orig);