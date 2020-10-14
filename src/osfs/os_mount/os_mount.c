#include "os_mount.h"

extern char *disk_path;

void os_mount(char *diskname)
{
  disk_path = diskname;
}