#include "os_API.h"

int main(int argc, char *argv[])
{
  os_mount("data/simdiskfilled.bin");

  osFile *file = os_open("/memes/generic (123).png", 'r');
  osFile *file = os_open("/memes/generic (123).png", 'w');
  
  if (file->filetype)
  {
    printf("Opened file\t%s\n", file->name);
    printf("File type\t%d\n", file->filetype);
    printf("Inode\t\t%d\n", file->inode);
  }
  os_close(file);
  return 0;
}