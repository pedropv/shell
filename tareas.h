/*
  Fichero: tareas.h
  Autor: Pedro Pascual
  Fecha: Abril del 2003
  Versión: 0.1

  Contiene el tipo de datos exportado, y la definición de constantes, 
  así como las cabeceras de las funciones exportadas.
*/

#ifndef TAREAS_H
#define TAREAS_H

#include <stdlib.h>
#include <stdio.h>

/*
  Constantes
*/

/* Estados de los procesos */
#define FIN 0
#define EXEC 1

/* Modo de ejecucion */
#define PLANO_1 0
#define PLANO_2 1

/* Errores */
#define NOK 0
#define OK 1

/*
  Tipo de datos exportado (como puntero a void)
*/

typedef void * Tlista;

/*
  Funciones exportadas
*/

extern Tlista crear_lista(void);
/*
  Crea una lista de tareas vacía.
  devuelve un puntero (tipo Tlista) a ella.
  Si no ha sido posible devuelve un puntero a NULL
*/

extern void borrar_lista(Tlista *lista);
/*
  Elimina la lista de tareas indicada por lista
*/

extern int hay_tareas(Tlista lista);
/*
  Devuelve OK si hay alguna tarea ejecutándose en la lista. 
  Si no hay ninguna tarea ejecutándose devuelve NOK
*/

extern void mostrar_tareas(Tlista lista);
/*
  Hace un listado de todas las tareas de la lista.
*/

extern void fin_proceso(Tlista lista, int id_proceso);
/*
  Finaliza el proceso cuyo id_proceso coincida con el dado
*/

extern void limpiar_lista(Tlista lista, int mostrar);
/*
  Elimina las tareas finalizadas (no tienen ningún proceso activo)
  de la lista, y las muestra por pantalla si mostrar no es 0
*/

extern int nuevo_proceso(Tlista lista, int id_tarea, char *cad, int modo, int id_proceso, int id_grupo);
/*
  Crea un nuevo proceso con id_proceso. Si su tarea asociada no está
  creada id_tarea debe ser 0 y se creará una nueva tarea con cad.
  Si la tarea ya ha sido creada el proceso se asociará con la tarea
  que tenga como id_tarea el indicado.
  Devuelve el número de tarea asociada con el proceso nuevo, si id_tarea
  no es cero este valor debe ser igual al id_tarea dado.
  Si ocurre algún error devuelve NOK (0).
*/

extern int dame_pid(Tlista lista, int id_tarea, int cual);
/*
  Devuelve el id de un proceso de la tarea que tenga como identificador
  el indicado. El proceso cuyo pid se devuelve está indicado por cual,
  de modo que si cual=1 devuelve el primer proceso de la tarea y así
  sucesivamente hasta que no haya más procesos que devuelve 0.
  Tambien devuelve 0 si no está la tarea o si ésta no tiene procesos.
*/

extern int exista_tarea(Tlista lista, int id_tarea);
/*
  Devuelve OK si la tarea cuyo id_tarea se está ejecutando 
  Sino se encuentra la tarea o ya ha finalizado devuelve NOK
*/

#endif










