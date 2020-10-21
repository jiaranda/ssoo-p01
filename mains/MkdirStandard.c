#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  os_ls("/memes");
  printf("\n\nWa hacer mkdir\n");
  os_mkdir("/memes/test3");
  os_ls("/memes");
  return 0;
}