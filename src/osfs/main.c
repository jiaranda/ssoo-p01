#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskformat.bin");
  os_bitmap(0, true);
  return 0;
}