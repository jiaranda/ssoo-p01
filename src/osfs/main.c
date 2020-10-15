#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  // os_bitmap(0, true);
  osFile *file = os_open("music", "r");
  os_close(file);
  return 0;
}