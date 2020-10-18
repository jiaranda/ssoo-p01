#include "os_hardlink.h"
#include <string.h>
#include "../os_open/os_open.h"

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

int os_hardlink(char* orig, char* dest)
{
  /* 
  Buscar el bloque de directorio en el que esta orig  y guardar en variable
    int dir_exists(char *path) retorna el directorio de un path
  Entrar al bloque indice y aumentar en uno el hardlink
	os_open(orig) entraga un struct con el i nodo del archivo.

	Pasos a seguir:
	1. abrir el archivo con op_open(orig) para obtener el inodo (ok) ✔
	2. abrir el archivo con os_open(dest, 'w') para crear la entrada a un archivo (ok)
	3. buscarmos el directorio con dir_exists(dest) para obtener el directorio del archivo destino (hardlink) (ok)
	4. buscamos dentro ese directorio el la posición del puntero del archivo creado.
	5. modificamos el bloque índice agregando 1 a los hardlinks
	6. seteamos el puntero del a la dirección final del archivo
  */

	osFile* orig_file = os_open(orig, 'r');
	osFile* dest_file = os_open(dest, 'w');
  if (orig_file->filetype == INVALID )
  {
    fprintf(stderr, "[ERROR] os_hardlink: could not open original_file path\n");
		os_close(orig_file);
		os_close(dest_file);
    return 0;
  }
  if (dest_file->filetype == INVALID )
  {
    fprintf(stderr, "[ERROR] os_hardlink: could not open destiny_file path (hardlink)\n");
		os_close(orig_file);
		os_close(dest_file);
    return 0;
  }
	
	uint32_t dest_directory = dir_exists(dest);
	char dest_name[29];
	strcpy(dest_name, dest_file -> name);
	printf("El directorio padre del destino se encuentra en la posición %d\n", dest_directory);
	FILE* fp = fopen(disk_path, "rb+");
  if(!fp)
  {
    fprintf(stderr, "[ERROR] os_hardlink: could not open disk\n");
  }

	fseek(fp, 2048 * dest_directory, SEEK_SET);
	int pos_in_directory;
	for (int i = 0; i < 64; i++){
		unsigned char entry[32];
		fread(entry, 32, 1, fp);
		char entry_name[29];
		
		get_array_slice(entry, entry_name, 3, 31);
		if (!strcmp(entry_name, dest_name))
    {
			pos_in_directory = i;
			break;
		}
	}

	fseek(fp, 2048 * dest_directory + pos_in_directory, SEEK_SET);
	unsigned char entry[32];
	fread(entry, 32, 1, fp);
	uint32_t pointer = (entry[0] << 16 | entry[1] << 8 | entry[2]) & BLOCK_NUM_MASK;
	printf("El bloque al que apunta originalmente es %d\n", pointer);
	
	unsigned char new_entry_head[3] = {0};
	uint32_t tmp_head;
	tmp_head = OS_FILE << 22 | orig_file->inode;    
	new_entry_head[0] = tmp_head >> 16;       
	new_entry_head[1] = (tmp_head >> 8) & 255; 
	new_entry_head[2] = tmp_head & 255;
	
	// Escribimos en la dirección antigua la nueva dirección
	// printf("La nueva dirección será %d\n", (int) new_entry_head);
	fseek(fp, 2048 * dest_directory + pos_in_directory, SEEK_SET);
	fwrite(new_entry_head, 3, 1, fp);

	// aumentamos en 1 los hardlinks
	fseek(fp, 2048 * orig_file->inode , SEEK_SET);
	
	unsigned char hardlinks[0];
	fread(hardlinks, 1, 1, fp);
	hardlinks[0] ++; 
	
	printf("El nuevo hardlink es :%d\n", hardlinks[0]);
	fseek(fp, 2048 * orig_file->inode , SEEK_SET);
	fwrite(hardlinks, 1, 1, fp);

	uint32_t data_array[3];
	get_block_in_bitmap(pointer, data_array);
	FILE *write_file = fopen(disk_path, "rb+");
	if (!write_file)
	{
		printf("No se pudo leer el archivo\n");
		fclose(fp);
		return 0;
	}
	fseek(write_file, 2048 * (data_array[0]+1) + data_array[1], SEEK_SET);
	unsigned char byte[0];
	fread(byte, 1, 1, write_file);
	//printf("El valor del byte es: %d\n", byte[0]);
	if (data_array[2]==0){
		byte[0] = byte[0] & 127; 
	}
	else if (data_array[2]==1){
		byte[0] = byte[0] & 191; 
	}
	else if (data_array[2]==2){
		byte[0] = byte[0] & 223; 
	}
	else if (data_array[2]==3){
		byte[0] = byte[0] & 239; 
	}
	else if (data_array[2]==4){
		byte[0] = byte[0] & 247; 
	}
	else if (data_array[2]==5){
		byte[0] = byte[0] & 251; 
	}
	else if (data_array[2]==6){
		byte[0] = byte[0] & 253; 
	}
	else if (data_array[2]==7){
		byte[0] = byte[0] & 254; 
	}
	//printf("El nuevo valor del byte es: %d\n", byte[0]);
	fseek(write_file, 2048 * (data_array[0]+1) + data_array[1], SEEK_SET);
	fwrite(byte, 1, 1, write_file);
	fclose(write_file);

	fseek(fp, 2048 * orig_file->inode , SEEK_SET);
	
	unsigned char new_hardlinks[0];
	fread(new_hardlinks, 1, 1, fp);
	printf("El valor final del hardlink es :%d\n", new_hardlinks[0]);

	fclose(fp);
	os_close(orig_file);
	os_close(dest_file);
  return 1;
}