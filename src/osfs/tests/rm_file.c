#include "../os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  os_ls("/");
  os_rm("IMPORTANT.txt");
  os_ls("/");
  return 0;
}