#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  int res = os_exists("/memes/no existo.peng");
  printf("Resultado os_exists: %d\n", res);
  os_ls("/memes");
  return 0;
}