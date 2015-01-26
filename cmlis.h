/*
tipos y funciones de uso de listas de mandatos (MLIS) y parámetros (CLIS)
Autor: José luis López
Fecha: 23 - Marzo - 2001
Revisado: 7 - Marzo - 2003
*/

#ifndef _CMLIS_H_
#define _CMLIS_H_

#define CLIS struct cnodo *
/* CLIS es una lista de cadenas. Se utiliza para almacenar los parámetros
   de un mandato. La cadena tendrá como máximo 255 caracteres incluido el
   carácter de fin de cadena '\0'.
*/
struct cnodo { CLIS sig; char cad[255]; };

#define MLIS struct mnodo *
/* MLIS es una lista de mandatos. Se utiliza para almacenar uno o varios
   mandatos (ésto último en el caso de que se usen tuberías. Cada elemento
   de la lista contendrá por un lado el mandato a ejecutar y por otro la
   lista de parámetros (CMLIS) para ese mandato.
*/
struct mnodo { MLIS sig; char mandato[255]; CLIS params; };

extern void liberarC ( CLIS c );
/* Libera toda la memoria correspondiente a una CLIS */

extern void liberarM ( MLIS m );
/* Libera toda la memoria que está utilizando una MLIS */

extern void concatC ( CLIS *plis, CLIS lis, char cad[] );
/* Recibe una CLIS (lis) y una cadena (cad) y crea un nuevo nodo cnodo
   que añade a la lista de cadenas, devolviendo a través de plis un puntero
   a la lista actualizada
*/

extern void concatM ( MLIS *plis, MLIS lis, char mandato[], CLIS param );
/* Recibe una MLIS (lis) un mandato y una lista de parámetros (param)
   y crea un nuevo struct mnodo con ellos y los añade a la lista, devolviendo
   un puntero a la lista actualizada a través de plis
*/

extern char **accion2argv ( MLIS accion );
/* Recibe una acción en formato MLIS (de un solo nodo) en una estructura
   de tipo argv para poder ser utilizada por ejemplo en la función execvp.
   Para ello reservará memoria adicional
*/

extern char ***acciones2argvlis ( MLIS acciones );
/* Recibe una lista de acciones en formato MLIS y devuelve una tabla de
   punteros a estructuras del tipo argv. La última entrada de la tabla
   valdra NULL para indicar que no hay más acciones en la lista.
*/

extern void liberarAV ( char **argv );
/* Libera la memoria ocupada por una estructura de tipo argv.
*/

extern void liberarAVL ( char ***argvlis );
/* Libera la memoria ocupada por una tabla de estructuras de tipo argv.
*/

#endif /* !_CMLIS_H_ */
