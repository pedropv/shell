/*
cabeceras de las funciones que implementan los mandatos del int�rprete
Autor: Jos� luis L�pez
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
   Es invocada una sola vez al empezar la ejecuci�n del int�rprete de
   mandatos para poder llevar a cabo las tareas de configuraci�n inicial
   y preparaci�n de las estructuras de datos que se consideren necesarias.
*/

extern void _pwd(void);
/*
   Implementa el mandato interno PWD.
*/

extern void _cd ( char *directorio );
/*
   Implementa el madato interno CD. Si el par�metro directorio es NULL,
   se cambia al directorio especificado por la variable de entorno HOME.
*/

extern void mostrar_mascara ( void );
/*
   Implementa el mandato interno UMASK sin par�metros.
   Muestra la m�scara actual para la creaci�n de nuevos ficheros.
*/

extern void establecer_mascara ( mode_t mascara );
/*
   Implementa el madato interno UMASK con par�metros.
   Establece como nueva m�scara la indicada por m�scara.
*/

extern void _kill ( int pogoj, int sig, int id );
/*
   Implementa el mandato interno KILL. Si pogoj=PROC, se env�a la se�al
   sig al proceso cuyo PID=id. Si pogoj=PGRP, se env�a la se�al sig al
   grupo de procesos cuyo PGID=id. Si pogoj=PJOB, se env�a la se�al
   sig a todos los procesos de la tarea cuyo n�mero de tarea indica id,
   y que corresponde al n�mero de tarea tal como se muestra mediante el
   mandato jobs.
*/

extern void ejecutar ( char *cadena, int pri, char **accion, char *fichent, char *fichsal, char amp );
/*
   Ejecuta un mandato simple (sin tuberias). Si pri=NONICE, el mandato se
   ejecutar� con la misma prioridad que tuviera el int�rprete de mandatos
   y si pri!=NONICE, el mandato se ejecutar� con la prioridad indicada.
   El ar�metro acci�n contendr� una estructura de datos de tipo argv que
   puede ser utilizada directamente como par�metro de la funcion execvp.
   Esta tabla no necesita ser liberada aqu�, sino que ser� liberada por
   la funci�n que la cre�. Sin embargo, cadena_mandato, que contiene la
   l�nea de mandato tal como la tecle� el usuario, no ser� liberada por la
   funci�n llamante y el alumno podr� utilizarla como desee y la liberar�
   cuando lo considere oportuno. Si fichent no es NULL, la entrada est�ndar
   se redirigir� al fichero cuyo nombre es referenciado por fichent. Si
   fichsal no es NULL, la salida est�ndar se redirigir� al fichero indicado
   por fichsal. Si amp='&', el mandato ser� ejecutado en segundo plano y en
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
   tantos segundos como indique el par�metro seg.
*/

extern void _exit_ ( void );
/*
   Es invocada una vez justo antes de terminar la ejecuci�n el int�rprete
   de mandatos. Realiza las tareas de finalizaci�n necesarias. Por ejemplo,
   revisar la lista de tareas para ver si hay alguna tarea de segundo plano
   en curso y mandarle una se�al indicando que el int�rprete va a terminar.
*/

extern void _wait ( void );
/*
   Implementa el mandato interno wait. Debe quedarse esperando hasta que
   finalicen todas las tareas de segundo plano. Esta espera no podra ser
   una espera activa en ning�n caso, pero debe garantizarse que se atienden
   las se�ales que reciba el proceso.
*/

extern void _jobs ( void );
/*
   Implementa el mandato interno jobs. Debe mostrar la lista de tareas
   de segundo plano. Como ejemplo v�ase el funcionamiento del mandato jobs
   de tcsh. En cualquier caso, ser� imprescindible mostrar como m�nimo: el
   n�mero de tarea, el estado de la misma (si est� en ejecuci�n o terminada)
   y la cadena correspondiente al mandato tal como lo escribi� el usuario
   y que fu� recibido por las funciones ejecutar o entubar. Las tareas que
   hayan terminado deben ser eliminadas de la lista de tareas una vez que
   han sido mostradas.
*/

extern void nuevo_mandato ( void );
/*
   Es llamada para mostrar el indicador (PROMPT) del int�rprete de mandatos
   justo antes de leer un nuevo mandato. Como indicador, puede utilizarse el
   definido anteriormente como PROMPT o uno m�s sofisticado que incluya por
   ejemplo el directorio actual, la hora actual, el nombre del usuario, etc.
   Adem�s, comprobar� el estado de las tareas de segundo plano e informar�
   al usuario de las tareas de segundo plana que hayan terminado durante la
   ejecuci�n del ultimo mandato de primer plano.
*/

extern void entubar ( char *cadena, char ***acciones, char *fichent, char *fichsal, char amp );
/*
   Es llamada para ejecutar los mandatos externos con tuber�as. Los mandatos
   a ejecutar vendr�n en la lista de mandatos acciones. acciones es una tabla
   de estructuras del tipo argv. La �ltima entrada de la tabla tendr� el valor
   NULL, para indicar que no hay m�s mandatos. La salida de cada uno de los
   procesos ir� conectada a trav�s de una tuber�a a la entrada del siguiente.
   El primero de los mandatos tendr� redirigida la entrada est�ndar si fichent
   no es NULL. Del mismo modo, si fichsal no es NULL, la salida del �ltimo
   proceso de la tuber�a ir� al fichero indicado por fichsal. Al igual que en
   el caso de ejecutar, la estructura de datos acciones ser� liberada por la
   funci�n que la cre�, mientras que cadena_mandato puede ser utilizada como
   se desee y ser� responsabilidad del alumno su liberaci�n. Todos los procesos
   de la tuber�a deber�n tener como PGID el PID del primer proceso de la tuber�a.
   De esta forma, se les puede enviar una se�al a todos ellos usando su PGID en
   lugar de los PIDs individuales de cada uno de los procesos.
*/

#endif





























