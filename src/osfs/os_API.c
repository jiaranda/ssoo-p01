#include "os_API.h"

void os_mount(char *diskname)
{
  FILE *fp = fopen(diskname, "rb");
  if (!fp)
  {
    printf("No se pudo leer el archivo\n");
    return;
  }
  for (int i = 0; i < 10000000; i++)
  {
    unsigned char bytes[4];
    uint n = fread(bytes, 4, 1, fp);
    printf("%d ", n);
  }
  fclose(fp);
}