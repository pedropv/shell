/*
  Nombre: ejecucion.c
  Autor: Pedro Pascual
  Fecha: Abril 2003
  Version: 0.1
  Módulo que implementa los mandatos internos del shell
*/

#include "ejecucion.h"
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

/* Error en las llamadas al sistema */
#define ERROR -1

/* Variable global del módulo para la lista de tareas */
Tlista lista;
/* Variable global para la máscara de señales */
sigset_t mascara;

/* Declaración de las funciones internas del módulo */
void gestor_chld(void);
/*
  Captura la señal SIGCHLD.
  Sincroniza el shell con su hijo muerto.
*/

/* Código de las funciones */

void ejecucion_init(void)
{
  /* Creamos una máscara */
  sigemptyset(&mascara);
  /* Añadimos a la máscara SIGCHLD y SIGINT */
  sigaddset(&mascara, SIGCHLD);
  /* Desbloqueamos las señales */
  sigprocmask(SIG_UNBLOCK, &mascara, NULL);

  /* Instalamos el gestor de la señal CHLD */
  signal (SIGCHLD, (void *)gestor_chld);

  /* Creamos la lista de tareas */
  lista=crear_lista();
  /* Si no se crea la lista salimos */
  if (lista==NULL)
    exit(1);
}

void nuevo_mandato (void)
{
  /* Bloqueamos la señal SIGCHLD */
  sigprocmask(SIG_BLOCK, &mascara, NULL);
  /* Limpiamos la lista y mostramos las finalizadas*/
  limpiar_lista(lista, 1);
  /* Desbloqueamos la señal SIGCHLD */
  sigprocmask(SIG_UNBLOCK, &mascara, NULL);
  /* Escribimos el prompt */
  printf(PROMPT);
}

void _cd (char *directorio)
{
  if (directorio==NULL){
    /* Si no está el parámetro directorio se cambia al HOME */
    if (chdir(getenv("HOME"))==ERROR)
      perror("CD");
    else
      setenv("PWD", getenv("HOME"), 1);
  }
  else{
    if (chdir(directorio)==ERROR)
      perror("CD");
    else
      setenv("PWD", directorio, 1);
  }
}
      
void _pwd (void)
{
  char *buf=malloc(MAXPATHLEN);
  if (buf!=NULL){
    buf= getcwd(buf, MAXPATHLEN);
    if (buf==NULL)
      perror("PWD");
    else{
      printf ("PWD: %s", buf);
      free (buf);
    }
  }
}

void mostrar_mascara (void)
{
  int mask=umask(1);
  umask(mask);
  printf("MASK: %d", mask);
}

void establecer_mascara (mode_t mascara)
{
  umask(mascara);
}

void _sleep (int seg)
{
  do{
    seg=sleep(seg);
  }while(seg!=0);
}

void _renice (int pog, int pri, int id)
{
  if (pog==PROC){
    /* Solo de un proceso */
    if (setpriority(PRIO_PROCESS, id, pri)==ERROR)
      perror("RENICE");
  }
  if (pog==PGRP){
    /* De un grupo de procesos */
    if (setpriority(PRIO_PGRP, id, pri)==ERROR)
      perror("RENICE");
  }
}

void _kill ( int pogoj, int sig, int id)
{
  int pid;
  int cual=1;
  
  /* Un proceso */
  if (pogoj==PROC){
    if(kill(id, sig)==ERROR)
      perror("KILL");
  }
  /* Grupo de procesos */
  else if(pogoj==PGRP){
    if(killpg(id, sig)==ERROR)
      perror("KILL");
  }
  /* Procesos de una tarea */
  else if(pogoj==PJOB){
    /* Bloqueamos la señal SIGCHLD */
    sigprocmask(SIG_BLOCK, &mascara, NULL);
    /* Recogemos los pid de los procesos */    
    pid=dame_pid(lista, id, cual);
    /* Desbloqueamos la señal SIGCHLD */
    sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    while (pid!=NOK){
      if(kill (pid,sig)==ERROR)
	perror("KILL");
      cual++;
      /* Bloqueamos la señal SIGCHLD */
      sigprocmask(SIG_BLOCK, &mascara, NULL);
      pid=dame_pid(lista, id, cual);
      /* Desbloqueamos la señal SIGCHLD */
      sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    }
  }
}

void _exit_ ( void )
{ 
  /* Bloqueamos la señal SIG_CHLD */
  sigprocmask(SIG_BLOCK, &mascara, NULL);
  /* Borramos la lista */
  borrar_lista(&lista);
  /* Enviamos señal de terminación a los procesos hijos */
  killpg(0, SIGKILL);
  /* Salimos */
  exit(0);
}

void _wait (void)
{ 
  /* Bloqueamos la señal CHLD */
  sigprocmask(SIG_BLOCK, &mascara, NULL);
  /* Hasta que no quede tareas */
  while (hay_tareas(lista)==OK){
    /* Desbloqueamos la señal SIGCHLD */
    sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    /* Suspendemos */
    pause();
    /* Bloqueamos la señal SIGCHLD */
    sigprocmask(SIG_BLOCK, &mascara, NULL);
  }
  /* Desbloqueamos la señal SIGCHLD */
  sigprocmask(SIG_UNBLOCK, &mascara, NULL);
}

void _jobs ( void )
{
  /* Bloqueamos la señal SIGCHLD */
  sigprocmask(SIG_BLOCK, &mascara, NULL);
  mostrar_tareas(lista);
  limpiar_lista(lista, 0);
  /* Desbloqueamos la señal SIGCHLD */
  sigprocmask(SIG_UNBLOCK, &mascara, NULL);
}

void ejecutar ( char *cadena, int pri, char **accion, char *fichent, char *fichsal, char amp )
{
  int tarea;
  int fd;
  int pid;
  
  /* Creamos al hijo */
  switch (pid=fork()){
    /* ERROR */
  case -1:
    perror("FORK");
    _exit_ ();

    /* HIJO */
  case 0:
    /* Si hay que cambiar la prioridad */
    if (pri!=NONICE){
      if (setpriority(PRIO_PROCESS, 0, pri)==ERROR)
	perror("SETPRIORITY");
    }
    
    /* Si hay que redireccionar la entrada */
    if (fichent!=NULL){
      /* Abrimos el fichero para lectura */
      fd=open(fichent, O_RDONLY);
      if (fd==ERROR){
	perror("OPEN");
	exit(1);
      }
      /* Redirreción de la entrada estándar */
      if (dup2 (fd, 0)!=0){
	perror("DUP2");
	exit (1);
      }
      close(fd);
    }

    /* Si hay que redirrecionar la salida */
    if (fichsal!=NULL){
      /* Abrimos el fichero para escritura */
      fd=open(fichsal, O_WRONLY | O_CREAT, 0666);
      if (fd==ERROR){
	perror("OPEN");
	exit(1);
      }
      /* Redirreción de la salida estándar */
      if (dup2 (fd, 1)!=1){
	perror("DUP2");
	exit (1);
      }
      close(fd);
    }

    /* Ejecutamos el programa */
    execvp(*accion, accion);
    /* Si llegamos aquí hay ERROR */
    perror("EXECVP");
    exit(1);
    
    /* PADRE */
  default:
    /* Si es de segundo plano */
    if (amp=='&'){
      /* Bloqueamos la señal SIGCHLD */
      sigprocmask(SIG_BLOCK, &mascara, NULL);
      /* Añadimos el proceso a la lista de tareas */
      tarea=nuevo_proceso(lista, 0, cadena, PLANO_2, pid, pid);
      /* Desbloqueamos la señal SIGCHLD */
      sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    
      /* Si no se crea la tarea finalizamos el shell */
      if (tarea==NOK)
	_exit_();
    }
    
    /* Si es primer plano */
    else{
      /* Bloqueamos la señal SIGCHLD */
      sigprocmask(SIG_BLOCK, &mascara, NULL);
      /* Añadimos el proceso a la lista de tareas */
      tarea=nuevo_proceso(lista, 0, cadena, PLANO_1, pid, pid);
      /* Desbloqueamos la señal SIGCHLD */
      sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    
      /* Si no se crea la tarea finalizamos el shell */
      if (tarea==NOK)
	_exit_();

      /* Suspendemos hasta que termine la tarea */
      do{
	/* Desbloqueamos la señal SIGCHLD */
	sigprocmask(SIG_UNBLOCK, &mascara, NULL);
	/* Suspendemos */
	pause();
	/* Bloqueamos la señal SIGCHLD */
	sigprocmask(SIG_BLOCK, &mascara, NULL);
      }while(exista_tarea(lista, tarea)==OK);
      /* Desbloqueamos la señal SIGCHLD */
      sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    }
    break;
  }
}
void entubar ( char *cadena, char ***acciones, char *fichent, char *fichsal, char amp )
{
  int fd, i, pid, tarea=0;
  int tubo_ant[2];
  int tubo_pos[2];
  
  /* Hasta que ya no queden más comandos */
  for (i=0;*(acciones+i)!=NULL;i++){
    
    /* El tubo posterior es ahora el anterior */
    tubo_ant[0]=tubo_pos[0];
    tubo_ant[1]=tubo_pos[1];
    
    /* Si no soy el último creo el tubo posterior */
    if (*(acciones+(i+1))!=NULL){
      
      if(pipe(tubo_pos)==ERROR){
	/* Si ocurre un ERROR */
	perror ("PIPE");
	/* Si no soy el primero cierro el tubo anterior */
	if (i!=0){
	  close(tubo_ant[0]);
	  close(tubo_ant[1]);
	}
	/* Finalizamos el shell */
	_exit_();
      }
    }
    
    /* Tenemos un hijo */
    pid=fork();
    switch (pid){    
      /* ERROR */
    case -1:
      perror("FORK");
      
      /* Si no soy el primero cierro el tubo anterior */
      if (i!=0){
	close(tubo_ant[0]);
	close(tubo_ant[1]);
      }

      /* Si no soy el ultimo cierro el tubo posterior */
      if (*(acciones+(i+1))!=NULL){
	close(tubo_pos[0]);
	close(tubo_pos[1]);
      }

      /* Finalizamos el shell */
      _exit_();
      
      /* HIJO */
    case 0:
      
      /* Si no soy el primero entrada a tubo anterior */
      if (i!=0){
	if (dup2(tubo_ant[0], 0)!=0){	  
	  /* Si ocurre un error */
	  perror("DUP2");
	  /* Cerramos el tubo anterior */
	  close(tubo_ant[0]);
	  close(tubo_ant[1]);
	  /* Si no soy el último cierro el tubo posterior */
	  if (*(acciones+(i+1))!=NULL){
	    close(tubo_ant[0]);
	    close(tubo_ant[1]);
	  }
	  /* Fin del hijo */
	  exit (1);
	}
	
	/* Si va bien */   
	/* El tubo anterior ya no nos sirve */
	close(tubo_ant[0]);
	close(tubo_ant[1]);
      }
      
      /* Si soy el primero y hay que redirrecionar la entrada */
      else if (fichent!=NULL){
	/* Abrimos el fichero para lectura */
	fd=open(fichent, O_RDONLY);
	
	if (fd==ERROR){
	  perror("OPEN");
	  exit(1);
	}
	
	/* Redirreción de la entrada estándar */
	if (dup2 (fd, 0)!=0){
	  perror("DUP2");
	  exit (1);
	}
	close(fd);
      }
      
      /* Si no soy el último salida al tubo posterior */
      if (*(acciones+(i+1))!=NULL){
	if (dup2(tubo_pos[1], 1)!=1){
	  /* Si ocurre un error */
	  perror ("DUP2");
	  /* Cerramos el tubo posterior  */
	  close (tubo_pos[0]);
	  close (tubo_pos[1]);
	  /* Si no soy el primero cerramos el tubo anterior */
	  if (i!=0){
	    close (tubo_ant[0]);
	    close (tubo_ant[1]);
	  }
	  /* Fin del hijo */
	  exit(1);
	}
	
	/* Si va bien */
	/* El tubo posterior ya no nos sirve */
	close (tubo_pos[0]);
	close (tubo_pos[1]);
      }
      
      /* Si soy el último y hay que redireccionar la salida */
      else if (fichsal!=NULL){
	/* Abrimos el fichero para escritura */
	fd=open(fichsal, O_WRONLY | O_CREAT, 0666);
	
	if (fd==ERROR){
	  perror("OPEN");
	  exit(1);
	}
	
	/* Redirreción de la salida estándar */
	if (dup2 (fd, 1)!=1){
	  /* Si error */
	  perror("DUP2");
	  exit (1);
	}
	close(fd);
      }
      
      /* Ejecutamos el mandato correspondiente */
      execvp(**(acciones+i), *(acciones+i));
      /* Si llegamos aquí ERROR */
      perror("EXECVP");
      exit(1);

      /* PADRE */
    default:
      /* Si no soy el primero cierro el tubo anterior */
      if (i!=0){
	close (tubo_ant[0]);
	close (tubo_ant[1]);
      }
      
      /* Si es de segundo plano */
      if (amp=='&'){
	/* Bloqueamos la señal SIGCHLD */
	sigprocmask(SIG_BLOCK, &mascara, NULL);
	/* Añadimos el proceso a la lista de tareas */
	tarea=nuevo_proceso(lista, tarea, cadena, PLANO_2, pid, pid);
	/* Desbloqueamos la señal SIGCHLD */
	sigprocmask(SIG_UNBLOCK, &mascara, NULL);
    
	/* Si no se crea el proceso finalizamos el shell */
	if (tarea==NOK)
	  _exit_();
      }
      
      /* Si es primer plano */
      else{
	/* Bloqueamos la señal SIGCHLD */
	sigprocmask(SIG_BLOCK, &mascara, NULL);
	/* Añadimos el proceso a la lista de tareas */
	tarea=nuevo_proceso(lista, tarea, cadena, PLANO_1, pid, pid);
	/* Desbloqueamos la señal SIGCHLD */
	sigprocmask(SIG_UNBLOCK, &mascara, NULL);
	
	/* Si no se crea el proceso finalizamos el shell */
	if (tarea==NOK)
	  _exit_();

	/* Si es el último suspendemos el shell */
	 if (*(acciones+(i+1))==NULL){
	   do{
	    /* Desbloqueamos la señal SIGCHLD */
	    sigprocmask(SIG_UNBLOCK, &mascara, NULL);
	    /* Suspendemos */
	    pause();
	    /* Bloqueamos la señal SIGCHLD */
	    sigprocmask(SIG_BLOCK, &mascara, NULL);
	  }while(exista_tarea(lista, tarea)==OK);
	 
	   /* Desbloqueamos la señal SIGCHLD */
	  sigprocmask(SIG_UNBLOCK, &mascara, NULL);
	}	
      }
      break;
    }
  }
}

void gestor_chld (void)
{
  int pid;
  
  /* Repetimos hasta que no quede ningun hijo muerto */
  do{
    /* Sincronizamos con algun hijo muerto */
    pid=waitpid(-1, NULL, WNOHANG);
    /* Finalizamos el proceso de la lista */
    if (pid!=0 && pid!=ERROR)
      fin_proceso(lista, pid);
  }while(pid!=ERROR && pid!=0);
}
















