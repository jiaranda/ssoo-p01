#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  // os_bitmap(0, true);
  osFile *file = os_open("memes/generic (3).png", "r");
  printf("%d\t%d\t%s\n", file->filetype, file->inode, file->name);
  os_close(file);
  return 0;
}