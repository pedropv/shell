/*
  Fichero: tareas.c
  Autor: Pedro Pascual
  Fecha: Abril del 2003
  Versión: 0.1

  Módulo tareas.c, contiene el código de las funciones para
  manejar la lista de tareas.
*/

#include "tareas_priv.h"
#include "tareas.h"

/*
  Funciones internas
*/

tarea_t *crear_tarea(int id_tarea, int id_grupo, char *cad, int modo)
{
  tarea_t *tarea=(tarea_t *) malloc(sizeof(tarea_t));

  /* Sin error */
  if (tarea!=NULL){
    tarea->id_tarea=id_tarea;
    tarea->id_grupo=id_grupo;
    tarea->cad=cad;
    tarea->modo=modo;
    tarea->num_procesos=0;
    tarea->inicio_procesos=NULL;
    tarea->sig_tarea=NULL;
  }
  return (tarea);
}

proceso_t *crear_proceso(int id_proceso, int id_grupo, int id_tarea)
{
  proceso_t *proceso=(proceso_t *) malloc(sizeof(proceso_t));

  /* Sin error */
  if (proceso!=NULL){
    proceso->id_proceso=id_proceso;
    proceso->id_grupo=id_grupo;
    proceso->id_tarea=id_tarea;
    proceso->estado=EXEC;
    proceso->sig_proceso=NULL;
  }
  return (proceso);
}

tarea_t *borrar_tarea(tarea_t **tarea)
{
  proceso_t *proceso=(*tarea)->inicio_procesos;
  tarea_t *sig_tarea=(*tarea)->sig_tarea;

  /* Se eliminan todos los procesos de la tarea */
  while (proceso!=NULL)
    proceso=borrar_proceso(&proceso);

  /* Se elimina el comando y la tarea */
  free((*tarea)->cad);
  free(*tarea);
  /* Devolver tarea siguiente */
  return (sig_tarea);
}

proceso_t *borrar_proceso(proceso_t **proceso)
{
  proceso_t *sig_proceso=(*proceso)->sig_proceso;
  free(*proceso);
  return (sig_proceso);
}

tarea_t *buscar_tarea(lista_t *lista, int id_tarea)
{
  tarea_t *tarea=lista->inicio_tareas;
  int encontrado=NOK;

  /* Hasta el final o hasta que se encuentre */
  while (tarea!=NULL && encontrado==NOK){
    /* Si se encuentra */
    if (tarea->id_tarea==id_tarea)
      encontrado=OK;

    /* Si no se encuentra */
    else
      tarea=tarea->sig_tarea;
  }
  return (tarea);
}

proceso_t *buscar_proceso(tarea_t *tarea, int id_proceso)
{
  proceso_t *proceso=tarea->inicio_procesos;
  int encontrado=NOK;

  /* Hasta el final o hasta que se encuentre */
  while (proceso!=NULL && encontrado==NOK){
    /* Si se encuentra */
    if (proceso->id_proceso==id_proceso)
      encontrado=OK;

    /* Si no se encuentra */
    else
      proceso=proceso->sig_proceso;
  }
  return (proceso);
}

void insertar_tarea(lista_t *lista, tarea_t *tarea)
{
  tarea_t *donde=lista->inicio_tareas;

  /* Si la lista está vacía */
  if (donde==NULL)
    lista->inicio_tareas=tarea;

  /* Si la lista no está vacia */
  else{
    /* Se busca el final de la lista */
    while (donde->sig_tarea!=NULL)
      donde=donde->sig_tarea;

    /* Y se inserta al final */
    donde->sig_tarea=tarea;
  }
  /* Se incrementa el número de tareas de la lista */
  lista->num_tareas++;
}

void insertar_proceso(tarea_t *tarea, proceso_t *proceso)
{
  proceso_t *donde=tarea->inicio_procesos;

  /* Si la tarea está vacía */
  if (donde==NULL)
    tarea->inicio_procesos=proceso;

  /* Si la tarea no está vacia */
  else{
    /* Se busca el final de la tarea */
    while (donde->sig_proceso!=NULL)
      donde=donde->sig_proceso;

    /* Y se inserta al final */
    donde->sig_proceso=proceso;
  }
  /* Se incrementa el número de procesos de la tarea */
  tarea->num_procesos++;
}

/*
  Funciones exportadas
*/

Tlista crear_lista(void)
{
  lista_t *lista=(lista_t *) malloc(sizeof(lista_t));

  /* Si no hay error */
  if (lista!=NULL)
    {
      lista->num_tareas=0;
      lista->inicio_tareas=NULL;
    }
  return ((Tlista) lista);
}

void borrar_lista(Tlista *lista)
{
  lista_t *lista_aux=(lista_t *) *lista;
  tarea_t *tarea=lista_aux->inicio_tareas;

  /* Hasta el final se borran todas las tareas */
  while (tarea!=NULL)
    tarea=borrar_tarea(&tarea);

  /* Se elimina la lista */
  free(lista_aux);
}

int hay_tareas(Tlista lista)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea=lista_aux->inicio_tareas;
  int resultado=NOK;

  /* Hasta que se encuentre una tarea activa o hasta el final */
  while (tarea!=NULL && resultado==NOK){
    /* Si la tarea está activa */
    if (tarea->num_procesos>0)
      resultado=OK;

    tarea=tarea->sig_tarea;
  }
  return(resultado);
}


void mostrar_tareas(Tlista lista)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea=lista_aux->inicio_tareas;

  /* Hasta el final de la lista */
  while(tarea!=NULL){
    /* Si no esta en primer plano */
    if (tarea->modo!=PLANO_1){
      /* Mostramos la tarea */
      printf("[%d]\t", tarea->id_tarea);

      /* Si la tarea está finalizada */
      if (tarea->num_procesos<1)
	printf("FINALIZADA\t");

      /* Si la tarea no ha finalizado */
      else
	printf("EJECUTANDOSE\t");

      printf("%s\n", tarea->cad);
    }
    tarea=tarea->sig_tarea;
  }
}

void fin_proceso(Tlista lista, int id_proceso)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea=lista_aux->inicio_tareas;
  proceso_t *proceso;

  /* Hasta el final de la lista o hasta que se encuentre el proceso */
  while(tarea!=NULL){
    proceso=buscar_proceso(tarea, id_proceso);
    /* Si se encuentra el proceso se finaliza el proceso*/
    if (proceso!=NULL){
      proceso->estado=FIN;
      tarea->num_procesos--;
      tarea=NULL;
    }
    /* Sino se encuentra */
    else
      tarea=tarea->sig_tarea;
  }
}

void limpiar_lista(Tlista lista, int mostrar)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea=lista_aux->inicio_tareas;
  tarea_t *anterior=NULL;

  /* Hasta el final de la lista se comprueba si las tareas
     han finalizado, si es así se eliminan de la lista */
  while (tarea!=NULL){
    if (tarea->num_procesos<1){
      /* Si hay que mostrar y no estaba en primer plano */
      if (mostrar!=0 && tarea->modo!=PLANO_1)
	printf ("[%d]\tFINALIZADA\t%s\n", tarea->id_tarea, tarea->cad);

      /* Si es la primera */
      if (tarea==lista_aux->inicio_tareas){
	lista_aux->inicio_tareas=tarea->sig_tarea;
	tarea=borrar_tarea(&tarea);
	anterior=NULL;
      }
      /* Si no es la primera */
      else{
	tarea=borrar_tarea(&tarea);
	anterior->sig_tarea=tarea;
      }
      /* Si era la ultima */
      if (tarea==NULL)
	lista_aux->num_tareas--;
    }
    else{
      anterior=tarea;
      tarea=tarea->sig_tarea;
    }
  }

  /* Si la lista no tiene ninguna tarea ya */
  if (lista_aux->inicio_tareas==NULL)
    lista_aux->num_tareas=0;
}

int nuevo_proceso(Tlista lista, int id_tarea, char *cad, int modo, int id_grupo, int id_proceso)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea;
  proceso_t *proceso;

  /* Si la tarea ya existe */
  if (id_tarea!=0){
    tarea=buscar_tarea(lista_aux, id_tarea);
    /* Si no se encuentra error */
    if (tarea==NULL)
      return(NOK);
  }
  /* Si hay que crear una nueva tarea */
  else{
    tarea=crear_tarea(lista_aux->num_tareas+1, id_grupo, cad, modo);
    /* Si no se ha creado error */
    if (tarea==NULL){
      lista_aux->num_tareas--;
      return(NOK);
    }
  }
  /* Se crea un nuevo proceso */
  proceso=crear_proceso(id_proceso, id_grupo, tarea->id_tarea);
  /* Si no se ha creado error */
  if (proceso==NULL)
    return (NOK);
  /* Se inserta el proceso en la tarea*/
  insertar_proceso(tarea, proceso);
  /* Si se ha creado una tarea se inserta en la lista */
  if (id_tarea==0)
    insertar_tarea(lista_aux, tarea);
  return(tarea->id_tarea);
}

extern int dame_pid(Tlista lista, int id_tarea, int cual)
{
  int pid=0;
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea=buscar_tarea(lista_aux, id_tarea);
  proceso_t *proceso;

  /* Si se ha encontrado */
  if (tarea!=NULL){
    proceso=tarea->inicio_procesos;
    /* Se busca el proceso adecuado */
    while (proceso!=NULL && cual > 1){
      proceso=proceso->sig_proceso;
      cual--;
    }
    /* Si existe el proceso */
    if (proceso!=NULL)
      pid=proceso->id_proceso;
  }
  return(pid);
}

int exista_tarea(Tlista lista, int id_tarea)
{
  lista_t *lista_aux=(lista_t *) lista;
  tarea_t *tarea;
  int resultado=NOK;

  /* Se busca la tarea si está y se está ejecutando se devuelve OK*/
  tarea=buscar_tarea(lista_aux, id_tarea);
  if (tarea!=NULL){
    if (tarea->num_procesos>0)
      resultado=OK;
  }
  return(resultado);
}





































