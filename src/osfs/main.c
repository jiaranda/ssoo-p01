#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");
  // os_bitmap(0, true);

  // os_open trials
  osFile *file = os_open("/memes/generic (200).png", 'w');
  if (file->filetype)
  {
    printf("Opened file\t%s\n", file->name);
    printf("File type\t%d\n", file->filetype);
    printf("Inode\t\t%d\n", file->inode);
  }
  os_close(file);

  // os_exists trials
  // int res = os_exists("/memes/generic (3).png");
  // int res = os_exists("/IMPORTANT.txt");
  // printf("Resultado os_exists: %d\n", res);
  return 0;
}