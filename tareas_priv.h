/*
  Fichero: tareas_priv.h
  Autor: Pedro Pascual
  Fecha: Abril del 2003
  Versión: 0.1

  Contiene la parte privada del módulo tareas.
  Estructuras de datos y declaración de funciones internas.
*/

#ifndef TAREAS_PRIV_H
#define TAREAS_PRIV_H

/* 
   Estructura de datos: proceso_t
   Para los procesos asociados a cada tarea
   Contiene: 
   id_proceso: Es el pid del proceso.
   id_grupo: Es el identificador del grupo de procesos.
   estado: Es el estado del proceso (ejecutándose o finalizado)
   tarea: Es el número de tarea a la que pertenece el proceso
   sig_proceso: Puntero al siguiente proceso
*/

typedef struct proc
{
  int id_proceso;
  int id_grupo;
  int estado;
  int id_tarea;
  struct proc *sig_proceso;
}proceso_t;

/*
  Estructura de datos: tarea_t
  Para las tareas creadas
  Contiene:
  id_tarea: Es el número de tarea
  cad: Puntero a caracter, que contiene el comando tecleado
  modo: Contiene el modo de ejecucion (primer o segundo plano)
  id_grupo: identificador del grupo de procesos asociado
  num_procesos: Indica el número de procesos activos que tiene
  inicio_procesos: Puntero a los procesos asociados
  sig_tarea: Puntero a la siguiente tarea
*/

typedef struct job
{
  int id_tarea;
  int id_grupo;
  char *cad;
  int modo;
  int num_procesos;
  proceso_t *inicio_procesos;
  struct job *sig_tarea;
}tarea_t;

/* 
   Estructura de datos: lista_t
   Para la lista de las tareas.
   Contiene:
   num_tareas: indica el número de tareas que tiene.
   inicio_tareas: puntero a las tareas que contiene.
*/

typedef struct
{
  int num_tareas;
  tarea_t *inicio_tareas;
}lista_t;

/*
  Funciones internas
*/

tarea_t *crear_tarea(int id_tarea, int id_grupo, char *cad, int modo);
/*
  Crea una tarea con los valores id_tarea , id_grupo y cad.
  Devuelve un puntero ha dicha tarea.
  Si no ha sido posible crearla devuelve un puntero a NULL
*/

proceso_t *crear_proceso(int id_proceso, int id_grupo, int id_tarea);
/* 
   Crea un proceso con los valores id_proceso, id_grupo y id_tarea.
   Devuelve un puntero ha dicho proceso.
   Si no ha sido posible crearlo devuelve un puntero a NULL
*/

tarea_t *borrar_tarea(tarea_t **tarea);
/*
  Borra la tarea dada y sus procesos asociados.
  Devuelve un puntero a la tarea siguiente de la borrada.
*/

proceso_t *borrar_proceso(proceso_t **proceso);
/*
  Borra el proceso apuntado por proceso.
  Devuelve un puntero al siguiente proceso del borrado.
*/

tarea_t *buscar_tarea(lista_t *lista, int id_tarea);
/*
  Busca una tarea en la lista indicada, cuyo id_tarea coincida con el indicado.
  Devuelve un puntero a la tarea buscada si lo encuentra. 
  Si no la encuentra devuelve un puntero a NULL.
*/

proceso_t *buscar_proceso(tarea_t *tarea, int id_proceso);
/*
  Busca un proceso en la tarea indicada, cuyo id_proceso coincida con 
  el indicado. Devuelve un puntero al proceso buscado si lo encuentra.
  Si no la encuentra devuelve un puntero a NULL.
*/

void insertar_tarea(lista_t *lista, tarea_t *tarea);
/*
  Inserta al final de la lista dada, la tarea a la que apunta
  el puntero tarea.
*/

void insertar_proceso(tarea_t *tarea, proceso_t *proceso);
/*
  Inserta al final de la tarea dada, el proceso al que apunta
  el puntero proceso.
*/

#endif

