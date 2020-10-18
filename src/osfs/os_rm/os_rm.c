#include "os_rm.h"
#include "../os_open/os_open.h"
#include "../os_exists/os_exists.h"
#include <string.h>

extern char *disk_path;
extern uint32_t BLOCK_NUM_MASK;

int os_rm(char *path){

	int exists = os_exists(path);

	if (exists){
	
		//printf("El path es %s\n", path);
		osFile* file = os_open(path, 'r');
		//printf("%d\t%d\t%s\n", file->filetype, file->inode, file->name);
		//os_close(file);
		int block_file = file -> inode;
		char name[29];
		strcpy(name, file-> name);
		FILE *fp = fopen(disk_path, "rb+");
		if (!fp)
		{
			printf("No se pudo leer el archivo\n");
			os_close(file);
			return 0;
		}

		fseek(fp, 2048 * block_file , SEEK_SET);
		unsigned char hardlinks[0];
		fread(hardlinks, 1, 1, fp);
		//printf("La cantidad de hardlinks es %d\n", hardlinks[0]);
		// Si tenemos un hardlink, debemos marcar todos los bloques del archivo como 0 en bitmap ok :D
		if (hardlinks[0] == 1){
			int directory_entry = dir_exists(path);
			fseek(fp, 2048 * directory_entry, SEEK_SET);
			for (int i = 0; i < 64; i++){
				unsigned char entry[32];
				fread(entry, 32, 1, fp);
				char entry_name[29];
				
				get_array_slice(entry, entry_name, 3, 31);
				if (!strcmp(entry_name, name)){
					uint32_t mask = 1;
					for (int i = 0; i < 5; i++){
						mask = mask << 1 | 1;
					} 
					entry[0] = entry[0]&mask;

					fseek(fp, 2048 * directory_entry + 32* i, SEEK_SET);
					fwrite(entry, 3, 1, fp);

					//printf("Encontré el archivo\n");
				}
			}

			rm_first_block(block_file);

			uint32_t data_array[3];
			get_block_in_bitmap(block_file, data_array);
			FILE *write_file = fopen(disk_path, "rb+");
			if (!write_file)
			{
				printf("No se pudo leer el archivo\n");
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

			// marcamos el hardlink en 0
			fseek(fp, 2048 * block_file , SEEK_SET);
			hardlinks[0]= hardlinks[0] &0;
			//printf("El nuevo harlink es: %d\n", hardlinks[0]);
			fwrite("0", 1, 1, fp);
			fclose(fp);

			// marcamos los primeros dos bits como 0.
			os_close(file);
			return 1;
		}
		else if (hardlinks[0] == 0){
			printf("El archivo ya se encontraba eliminado\n");
			os_close(file);
			return 0;
		}

		// si los hardlinks son mayores o iguales a 2

		else{
			hardlinks[0] = hardlinks[0]-1;
			int directory_entry = dir_exists(path);
			fseek(fp, 2048 * directory_entry, SEEK_SET);
			for (int i = 0; i < 64; i++){
				unsigned char entry[32];
				fread(entry, 32, 1, fp);
				char entry_name[29];
				
				get_array_slice(entry, entry_name, 3, 31);
				if (!strcmp(entry_name, name)){
					uint32_t mask = 1;
					for (int i = 0; i < 5; i++){
						mask = mask << 1 | 1;
					} 
					entry[0] = entry[0]&mask;

					fseek(fp, 2048 * directory_entry + 32* i, SEEK_SET);
					fwrite(entry, 3, 1, fp);

					//printf("Encontré el archivo\n");
				}
			}

			// el i nodo es block_file
			fseek(fp, 2048 * block_file , SEEK_SET);
			fwrite(hardlinks, 1, 1, fp);
			printf("Se ha eliminado una referencia al archivo");
			os_close(file);
			return 1;
		}
	}
	return 0;
};


int rm_first_block(int block_file)
{	

	FILE *fp = fopen(disk_path, "rb");
	if (!fp)
	{
		printf("No se pudo leer el archivo\n");
		return 0;
	}
	fseek(fp, 2048 * block_file +1 , SEEK_SET);
  	unsigned char size[7];
	fread(size, 7, 1, fp);
	uint64_t size_of = 0 << 63;
	uint64_t mask = 1;
	for (int i = 0; i < 63; i++){
		mask = mask << 1 | 1;
	}

	size_of = (size[0] << 48 | size[1] << 40 |size[2] << 32 |size[3] << 24 |size[4] << 16 |size[5] << 8 |size[6])& mask;
	//printf("El tamaño del archivo es %ld\n", size_of);

	for (int i = 0; i < 509 ; i++){

		unsigned char bloque_direccion_char[4];
		fread(bloque_direccion_char, 4, 1, fp);
		
		uint32_t mask_direccion = 1;
		for (int i = 0; i < 31; i++){
			mask_direccion = mask_direccion << 1 | 1;
		}
		uint32_t next_block;
		next_block = (bloque_direccion_char[0] << 24 | bloque_direccion_char[1] << 16 | bloque_direccion_char[2] << 8 | bloque_direccion_char[3])& mask_direccion;

		// marcar el bitmap en 0 para este bloque

		if (next_block){

			uint32_t data_array[3];
			get_block_in_bitmap(next_block, data_array);
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

			//printf("El siguiente puntero es: %d\n", next_block);
			rm_indirect_block(next_block);
		}
	}

	unsigned char bloque_direccion_char[4];
	fread(bloque_direccion_char, 4, 1, fp);

	uint32_t mask_direccion = 1;
	for (int i = 0; i < 31; i++){
		mask_direccion = mask_direccion << 1 | 1;
	}
	uint32_t next_block;
	next_block = (bloque_direccion_char[0] << 24 | bloque_direccion_char[1] << 16 | bloque_direccion_char[2] << 8 | bloque_direccion_char[3])& mask_direccion;

	if (next_block){
		fclose(fp);
		return rm_first_block_2(next_block);
	}
	fclose(fp);
	return 1;

};

int rm_indirect_block(uint32_t block_file){
	FILE *file = fopen(disk_path, "rb");
	if (!file)
	{
		//printf("No se pudo leer el archivo\n");
		return 0;
	}
	fseek(file, 2048 * block_file, SEEK_SET);
	for (int i = 0; i < 512; i++){
		unsigned char bloque_direccion_char[4];
		fread(bloque_direccion_char, 4, 1, file);
		
		uint32_t mask_direccion = 1;
		for (int i = 0; i < 31; i++){
			mask_direccion = mask_direccion << 1 | 1;
		}
		uint32_t next_block;
		next_block = (bloque_direccion_char[0] << 24 | bloque_direccion_char[1] << 16 | bloque_direccion_char[2] << 8 | bloque_direccion_char[3])& mask_direccion;
		
		
		FILE *read_bit = fopen(disk_path, "rb+");
		if (!read_bit)
		{
			printf("No se pudo leer el archivo\n");
			return 0;
		}
		uint32_t bit_array_pos[3];
		get_block_in_bitmap(next_block, bit_array_pos);
		fseek(read_bit, 2048 * (bit_array_pos[0]+1) + bit_array_pos[1], SEEK_SET);
		unsigned char byte[0];
		fread(byte, 1, 1, read_bit);
		
		uint32_t bit = 0;
		bit = (byte[0] >> (7- bit_array_pos[2]))&1;
		//printf("El valor del bit es %d\n", bit);
		fclose(read_bit);

		if (next_block && bit){

			//printf("El siguiente puntero a los datos del archivos es: %d\n", next_block);
			uint32_t data_array[3];
			get_block_in_bitmap(next_block, data_array);
			//printf("Bloque: %d, byte: %d, bit: %d\n", data_array[0], data_array[1], data_array[2]);
			FILE *write_file = fopen(disk_path, "rb+");
			if (!write_file)
			{
				printf("No se pudo leer el archivo\n");
				fclose(file);
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
			FILE *read_file = fopen(disk_path, "rb+");
			if (!read_file)
			{
				printf("No se pudo leer el archivo\n");
				return 0;
			}
			fseek(read_file, 2048 * (data_array[0]+1) + data_array[1], SEEK_SET);
			unsigned char new_byte[0];
			fread(new_byte, 1, 1, read_file);
			//printf("El nuevo valor del byte en el archivo es: %d\n", byte[0]);
			fclose(read_file);
		}
	}
	fclose(file);
	return 1;
}

int rm_first_block_2(int block_file){
	FILE *fp = fopen(disk_path, "rb");
	if (!fp)
	{
		printf("No se pudo leer el archivo\n");
		return 0;
	}
	fseek(fp, 2048 * block_file , SEEK_SET);

	for (int i = 0; i < 511 ; i++){

		unsigned char bloque_direccion_char[4];
		fread(bloque_direccion_char, 4, 1, fp);
		
		uint32_t mask_direccion = 1;
		for (int i = 0; i < 31; i++){
			mask_direccion = mask_direccion << 1 | 1;
		}
		uint32_t next_block;
		next_block = (bloque_direccion_char[0] << 24 | bloque_direccion_char[1] << 16 | bloque_direccion_char[2] << 8 | bloque_direccion_char[3])& mask_direccion;

		// marcar el bitmap en 0 para este bloque
		
		if (next_block){

			uint32_t data_array[3];
			get_block_in_bitmap(next_block, data_array);
			FILE *write_file = fopen(disk_path, "rb+");
			if (!write_file)
			{
				printf("No se pudo leer el archivo\n");
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

			//printf("El siguiente puntero es: %d\n", next_block);
			rm_indirect_block(next_block);
		}
		
	}

	unsigned char bloque_direccion_char[4];
	fread(bloque_direccion_char, 4, 1, fp);

	uint32_t mask_direccion = 1;
	for (int i = 0; i < 31; i++){
		mask_direccion = mask_direccion << 1 | 1;
	}
	uint32_t next_block;
	next_block = (bloque_direccion_char[0] << 24 | bloque_direccion_char[1] << 16 | bloque_direccion_char[2] << 8 | bloque_direccion_char[3])& mask_direccion;

	if (next_block){
		fclose(fp);
		return rm_first_block_2(next_block);
	}
	fclose(fp);
	return 1;
}