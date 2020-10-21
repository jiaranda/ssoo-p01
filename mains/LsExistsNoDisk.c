#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilleds.bin");
  int res = os_exists("/no existo.peng");
  return 0;

}