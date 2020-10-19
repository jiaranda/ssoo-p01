# Documentación os_API

---

# Integrantes - Grupo Among OS

- Vicente Águila Cea
- Javier Aranda Pizarro
- Nicolás Riera Ovalle
- Claudio Scheihing Cuevas

# Índice

[os_mount](https://www.notion.so/os_mount-87adb776083249cf910b8455e1487a93)

[os_bitmap](https://www.notion.so/os_bitmap-f7d027e7dd6c4c9b94218ac3f20fc4b6)

[os_exists](https://www.notion.so/os_exists-2799866de6b849ddbfb3dbc9fdcb9bcd)

[os_ls](https://www.notion.so/os_ls-869cb004dd44405bbc6533725c08ec8a)

[os_mkdir](https://www.notion.so/os_mkdir-947371da6e0348139fed1bfb68b345b4)

[os_open](https://www.notion.so/os_open-9c8e8b8c59b2456d9af0775dfdd03155)

[os_rm](https://www.notion.so/os_rm-708f9cf49712404fbbcd6a0fbc35ec34)

[os_close](https://www.notion.so/os_close-15c2bc089e4e4898b7e6adbd2161bf68)

[os_hardlink](https://www.notion.so/os_hardlink-21496a498c764b04b60dbdffd720aecb)

# Funciones

## `os_mount(char* diskname)`

---

**SYNOPSIS**

```c
void os_mount(char *diskname);
```

**DESCRIPTION**

Establece la ubicación del disco disponible para el resto de las funciones de la API.

**PARAMETERS**

`char *diskname`: *path* relativo a la ubicación del disco.

**RETURN VALUE**

La función `os_mount` no tiene valor de retorno.

**ERROR HANDLING**

La función `os_mount`no maneja errores.

**EXAMPLE**

```c
os_mount("data/simdiskfilled.bin");
```

## `os_bitmap(char* path, char mode)`

---

**SYNOPSIS**

```c
#include "os_API.h"

void os_bitmap(unsigned num, bool hex);
```

**DESCRIPTION**

La función imprime en `stderr` el contenido del bloque `num` del bitmap del disco. Si `num` es 0, imprime todo el bitmap, junto con un resumen de la cantidad de bloques ocupados.

**PARAMETERS**

- `unsigned num`: indica el número de bloque a imprimir (entre 0 y 64).  Si es 0, imprime todo el bitmap, junto a un resumen de la cantidad de bloques ocupados.
- `bool hex`: si es `true` imprime el bitmap en hexadecimal. En otro caso, imprime en binario.

**RETURN VALUE**

La función `os_bitmap` no tiene valor de retorno.

**ERROR HANDLING**

Si `num > 64` no se imprime nada.

**EXAMPLE**

```c
#include "os_API.h"

void os_bitmap(10, true);
```

## `os_exists(char* path, char mode)`

---

**DESCRIPTION**

La función `os_exists` verifica la existencia de un archivo en el disco.

**PARAMETERS**

`char *path`: *path* al archivo que se está buscando.

**RETURN VALUE**

La función `os_exists` retorna un entero: 1 en caso de que exista y 0 en caso de que no.

**ERROR HANDLING**

- Si no se logra leer el disco, se imprime el error y se finaliza la ejecución.
- Si `char *path` no tiene el formato correcto, separado por `/` y  con un archivo al final, imprime

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
	os_mount("data/simdiskfilled.bin");

	// Se verifica la existencia de un archvio
	os_exists("/memes/generic (3).png");
}
```

## `os_ls(char* path)`

---

**SYNOPSIS**

```c
# include <string.h>
# include <stdint.h>

void os_ls(char* path);
```

**DESCRIPTION**

La función `os_ls` se encarga de listar todos elementos dentro de un directorio. Se imprimen en pantalla todos los archivos, *hardlinks* y directorios contenidos en la carpeta asociada al *path* entregado.

**PARAMETERS**

`char* path` : *Path* en el que se encuentra el directorio sobre el que se quiere listar su contenido.

**RETURN VALUE**

La función `os_ls` no tiene valor de retorno.

**ERROR HANDLING**

- Si no se logra leer el disco (debido a un error al ejecutar `fopen`), se imprime el error y finaliza la ejecución.
- Si se intenta ejecutar el la función sobre el *path* de un archivo, se imprime el error y finaliza la ejecución.
- Si el *path* entregado no existe, se muestra un mensaje en pantalla.

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
	os_mount("data/simdiskfilled.bin");
	
	// Se listan los elementos de un directorio
	os_ls("/memes");
}
```

## `os_open(char* path, char mode)`

---

**SYNOPSIS**

```c
#include "os_API.h"

osFile *os_open(char *path, char mode); 
```

**DESCRIPTION**

La función `os_open` se encarga de abrir un archivo y retornar un descriptor de este. 

**PARAMETERS**

- `char *path`: *path* en el que se encuentra el archivo.
- `char mode`: indica si el archivo se abre en modo lescritura `'w'` o o lectura `'r'`

**RETURN VALUE**

`os_open` retorna un struct del tipo `osFile` que funciona como descriptor del archivo abierto.

**ERROR HANDLING**

- Si hay un error al leer el disco, retornará un `osFile` inválido, con el mensaje "ERROR: os_open. Error while reading disk."

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
	os_mount("data/simdiskfilled.bin");
	
	// Se abre un archivo
	osFile *file = os_open("/memes/generic (123).png", 'w');
}

```

## `os_rm(char* path)`

---

**SYNOPSIS**

```c
#include "os_API.h"

int os_rm(char *path);
```

**DESCRIPTION**

`os_rm` es una función que permite eliminar un archivo que se encuentra en `path` el cual debe tener una cantidad de hardlinks iguales a 1 o bien, disminuir las referencias de el archivo representado en `path` en una unidad en el caso de que un archivo posea un hardlink mayor a 1. 

**PARAMETERS**

- `char *path`: path al archivo a eliminar.

**RETURN VALUE**

Retorna `1` si el archivo se eliminó o se le disminuyó en una unidad el parámetro hadrlink con éxito, `0` en otro caso.

**ERROR HANDLING**

- Si hay un error al leer el disco, retornará 0, con el mensaje "ERROR: os_open. Error while reading disk."
- Si el archivo ya fue eliminado, retornará 0, con el mensaje "El archivo ya se encontraba eliminado"

**EXAMPLE**

```c
#include "os_API.h"

int os_rm("/memes/generic (123).jpg");
```

## `os_close(osFile* file_desc)`

---

**SYNOPSIS**

```c
#include "os_API.h"

int os_close(osFile *file_desc);
```

**DESCRIPTION**

`os_close` libera el espacio en memoria utilizado por el descriptor `osFile`

**PARAMETERS**

`osFile *file_desc`: puntero al descriptor `osFile` de un archivo abierto.

**RETURN VALUE**

Retorna `1` si se libera el espacio exitosamente, `0` en otro caso.

**ERROR HANDLING**

`os_close` no tiene manejo de errores.

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
	os_mount("data/simdiskfilled.bin");

	// Se abre un archivo
	osFile *file = os_open("/memes/generic (123).png", 'w');

	// Se cierra
	os_close(file);
}
```

## `os_mkdir(char* path)`

---

**SYNOPSIS**

```c
#include <string.h>
#include "./../utils/utils.h"

int os_mkdir(char* path);
```

**DESCRIPTION**

La función `os_mkdir` se encarga de crear un nuevo directorio en el *path* entregado. Retorna `1` si es que la operación fue exitosa, y `0` en caso contrario.

**PARAMETERS**

- `char* path` : ruta en el que se debe crear el directorio.

**RETURN VALUE**

La función `os_mkdir` retorna 1 si es que el directorio se creó correcrtamente y 0 si es que no.

**ERROR HANDLING**

Si no se puede abrir el disco montado (tanto en formato `'r'` como `'w'` ), se imprime el error en pantalla y se retorna `0`.

Si el directorio ya existe, se imprime el error en pantalla y se retorna `0`.

Si el nombre del directorio a crear contiene un punto, entonces es interpretado como un archivo, por lo que se imprime dicho error y se retorna `0`

Si es que el bloque de directorio padre no posee entradas disponibles, se imprime el error en pantalla y se retorna `0`.

Si el disco no posee ningún bloque disponible, se imprime el error en pantalla y se retorna `0`.

Si el `path` tiene un formato incorrecto, se imprime el error en pantalla y se retorna `0` .

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
	os_mount("data/simdiskfilled.bin");

	// Se crea directorio
	int mkdir = os_mkdir("/test3");
	
	if (mkdir)
	{
		printf("Directorio creado exitosamente\n");
  }
	else
	{
		printf("Error al crear directorio (revisar STDERR)\n");
	}
	return 0;
}
```

## `os_hardlink(char* orig, char* dest)`

---

**SYNOPSIS**

```c
#include "os_API.h"

int os_hardlink(char* orig, char* dest);
```

**DESCRIPTION**

`os_hardlink` permite crear un archivo representado por el path dest que contiene el mismo contenido que el archivo representado por el path orig. Al momento de ejecutar esta función, se aumenta en una unidad la cantidad de hardlinks del archivo original. 

**PARAMETERS**

`char* orig` : path del archivo que será copiado.

`char* dest` : path del archivo que copiará el contenido del path orig.

**RETURN VALUE**

Retorna `1` si el archivo se creó con éxito, `0` en otro caso.

**ERROR HANDLING**

- Si hay un error al leer el archivo del path orig, retornará 0, con el mensaje "[ERROR] os_hardlink: could not open original_file path"
- Si hay un error al crear el archivo del path dest, retornará 0, con el mensaje "[ERROR] os_hardlink: could not open original_file path"
- Si hay un error al leer el disco, retornará 0, con el mensaje "ERROR: os_hardlink. Error while reading disk."

**EXAMPLE**

```c
#include "os_API.h"

int main(int argc, char *argv[])
{
	// Se monta el disco
  os_mount("data/simdiskfilled.bin");

	// Se escribe el hardlink
  os_hardlink("IMPORTANT.txt", "chao4.txt");
  return 0;
}
```