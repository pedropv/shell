/*
cabeceras de las funciones que implementan los mandatos del intérprete
Autor: José luis López
Fecha: 23 - Marzo - 2001
Modificado: 11 - Marzo - 2003
*/

#ifndef EJECUCION_H
#define EJECUCION_H

#include "cmlis.h"
#include "tareas.h"
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/param.h>

#define PROMPT ":-)"
#define ERROR -1
#define PROC 0
#define PGRP 1
#define PJOB 3
#define NONICE INT_MAX

extern void ejecucion_init ( void );
/*
   Es invocada una sola vez al empezar la ejecución del intérprete de
   mandatos para poder llevar a cabo las tareas de configuración inicial
   y preparación de las estructuras de datos que se consideren necesarias.
*/

extern void _pwd(void);
/*
   Implementa el mandato interno PWD.
*/

extern void _cd ( char *directorio );
/*
   Implementa el madato interno CD. Si el parámetro directorio es NULL,
   se cambia al directorio especificado por la variable de entorno HOME.
*/

extern void mostrar_mascara ( void );
/*
   Implementa el mandato interno UMASK sin parámetros.
   Muestra la máscara actual para la creación de nuevos ficheros.
*/

extern void establecer_mascara ( mode_t mascara );
/*
   Implementa el madato interno UMASK con parámetros.
   Establece como nueva máscara la indicada por máscara.
*/

extern void _kill ( int pogoj, int sig, int id );
/*
   Implementa el mandato interno KILL. Si pogoj=PROC, se envía la señal
   sig al proceso cuyo PID=id. Si pogoj=PGRP, se envía la señal sig al
   grupo de procesos cuyo PGID=id. Si pogoj=PJOB, se envía la señal
   sig a todos los procesos de la tarea cuyo número de tarea indica id,
   y que corresponde al número de tarea tal como se muestra mediante el
   mandato jobs.
*/

extern void ejecutar ( char *cadena, int pri, char **accion, char *fichent, char *fichsal, char amp );
/*
   Ejecuta un mandato simple (sin tuberias). Si pri=NONICE, el mandato se
   ejecutará con la misma prioridad que tuviera el intérprete de mandatos
   y si pri!=NONICE, el mandato se ejecutará con la prioridad indicada.
   El arámetro acción contendrá una estructura de datos de tipo argv que
   puede ser utilizada directamente como parámetro de la funcion execvp.
   Esta tabla no necesita ser liberada aquí, sino que será liberada por
   la función que la creó. Sin embargo, cadena_mandato, que contiene la
   línea de mandato tal como la tecleó el usuario, no será liberada por la
   función llamante y el alumno podrá utilizarla como desee y la liberará
   cuando lo considere oportuno. Si fichent no es NULL, la entrada estándar
   se redirigirá al fichero cuyo nombre es referenciado por fichent. Si
   fichsal no es NULL, la salida estándar se redirigirá al fichero indicado
   por fichsal. Si amp='&', el mandato será ejecutado en segundo plano y en
   caso contrario, en primer plano.
*/

extern void _renice ( int pog, int pri, int id );
/*
   Implementa el mandato interno renice. Si pog=PROC, se cambia la
   prioridad del proceso cuyo PID=id. Si pog=PGRP, se cambia la prioridad
   de todo el grupo de procesos con PGID=id.
*/

extern void _sleep ( int seg );
/*
   Implementa el mandato interno sleep. Deja el proceso detenido durante
   tantos segundos como indique el parámetro seg.
*/

extern void _exit_ ( void );
/*
   Es invocada una vez justo antes de terminar la ejecución el intérprete
   de mandatos. Realiza las tareas de finalización necesarias. Por ejemplo,
   revisar la lista de tareas para ver si hay alguna tarea de segundo plano
   en curso y mandarle una señal indicando que el intérprete va a terminar.
*/

extern void _wait ( void );
/*
   Implementa el mandato interno wait. Debe quedarse esperando hasta que
   finalicen todas las tareas de segundo plano. Esta espera no podra ser
   una espera activa en ningún caso, pero debe garantizarse que se atienden
   las señales que reciba el proceso.
*/

extern void _jobs ( void );
/*
   Implementa el mandato interno jobs. Debe mostrar la lista de tareas
   de segundo plano. Como ejemplo véase el funcionamiento del mandato jobs
   de tcsh. En cualquier caso, será imprescindible mostrar como mínimo: el
   número de tarea, el estado de la misma (si está en ejecución o terminada)
   y la cadena correspondiente al mandato tal como lo escribió el usuario
   y que fué recibido por las funciones ejecutar o entubar. Las tareas que
   hayan terminado deben ser eliminadas de la lista de tareas una vez que
   han sido mostradas.
*/

extern void nuevo_mandato ( void );
/*
   Es llamada para mostrar el indicador (PROMPT) del intérprete de mandatos
   justo antes de leer un nuevo mandato. Como indicador, puede utilizarse el
   definido anteriormente como PROMPT o uno más sofisticado que incluya por
   ejemplo el directorio actual, la hora actual, el nombre del usuario, etc.
   Además, comprobará el estado de las tareas de segundo plano e informará
   al usuario de las tareas de segundo plana que hayan terminado durante la
   ejecución del ultimo mandato de primer plano.
*/

extern void entubar ( char *cadena, char ***acciones, char *fichent, char *fichsal, char amp );
/*
   Es llamada para ejecutar los mandatos externos con tuberías. Los mandatos
   a ejecutar vendrán en la lista de mandatos acciones. acciones es una tabla
   de estructuras del tipo argv. La última entrada de la tabla tendrá el valor
   NULL, para indicar que no hay más mandatos. La salida de cada uno de los
   procesos irá conectada a través de una tubería a la entrada del siguiente.
   El primero de los mandatos tendrá redirigida la entrada estándar si fichent
   no es NULL. Del mismo modo, si fichsal no es NULL, la salida del último
   proceso de la tubería irá al fichero indicado por fichsal. Al igual que en
   el caso de ejecutar, la estructura de datos acciones será liberada por la
   función que la creó, mientras que cadena_mandato puede ser utilizada como
   se desee y será responsabilidad del alumno su liberación. Todos los procesos
   de la tubería deberán tener como PGID el PID del primer proceso de la tubería.
   De esta forma, se les puede enviar una señal a todos ellos usando su PGID en
   lugar de los PIDs individuales de cada uno de los procesos.
*/

#endif





























