#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  // os_bitmap(0, true);
  // int res = os_exists("/memes/generic (3).png");
  int res = os_exists("/IMPORTANT.txt");
  printf("Resultado os_exists: %d\n", res);
  return 0;
}