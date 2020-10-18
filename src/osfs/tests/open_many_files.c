#include "../os_API.h"

int test(int argc, char *argv[])
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

// void indent(int level)
// {
//   for (int j = 0; j < level; j++)
//   {
//     printf("-");
//   }
// }

// void print_directory_tree(FILE *fp, uint32_t block_pointer, int level)
// {
//   fseek(fp, 2048 * block_pointer, SEEK_SET);
//   unsigned char entry[32];
//   int entry_type;
//   uint32_t entry_pointer;
//   char entry_name[29];
//   for (int i = 0; i < 32; i++)
//   {
//     fread(entry, 32, 1, fp);
//     entry_type = entry[0] >> 6;
//     if (entry_type)
//     {
//       entry_pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
//       get_array_slice(entry, entry_name, 3, 31);
//       indent(level);
//       printf("%d\t%d\t%s\n", entry_type, entry_pointer, entry_name);
//       // if (entry_type == OS_DIRECTORY)
//       // {
//       //     print_directory_tree(fp, entry_pointer, level + 1);
//       // }
//     }
//   }
// }

// uint32_t get_empty_block_pointer()
// {
//   FILE *fp = fopen(disk_path, "rb");
//   if (!fp)
//   {
//     fprintf(stderr, "ERROR: os_open. Error while reading disk.\n");
//     return 0;
//   }
//   fseek(fp, 2048, SEEK_SET);
//   uint32_t byte_count = 0;
//   unsigned char byte[0];
//   fread(byte, 1, 1, fp);
//   while ((uint32_t)byte[0] == 255)
//   {
//     fread(byte, 1, 1, fp);
//     byte_count++;
//   }

//   for (uint32_t i = 0; i < 8; i++)
//   {
//     if (byte[0] & (128 >> i))
//     {
//       fclose(fp);
//       return (byte_count * 8) + i;
//     }
//   }
//   fclose(fp);
//   return 0;
// }