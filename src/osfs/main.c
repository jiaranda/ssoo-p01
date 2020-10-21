#include "./os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");

  for (uint32_t i = 0; i < 66; i++)
  {
    char buffer[29];
    sprintf(buffer, "memes/meme %d.jpg", i);
    osFile *file = os_open(buffer, 'w');
    os_close(file);
  }
  os_ls("/memes");

  // os_exists trials
  // int res = os_exists("/memes/generic (3).png");
  // int res = os_exists("/memes/generic (200).png");
  // printf("Resultado os_exists: %d\n", res);
  return 0;
}