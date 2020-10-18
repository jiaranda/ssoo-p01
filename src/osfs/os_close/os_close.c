#include "os_close.h"

int os_close(osFile *file_desc)
{
  free(file_desc);
  return 0;
}