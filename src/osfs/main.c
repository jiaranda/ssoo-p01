#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  // os_bitmap(0, true);
  osFile *file = os_open("/IMPORTANTS.txt", 'w');
  printf("%d\t%s\n", file->filetype, file->name);
  os_close(file);
  // int res = os_exists("/memes/generic (3).png");
  // int res = os_exists("/IMPORTANT.txt");
  // printf("Resultado os_exists: %d\n", res);
  return 0;
}