/*
gramática para un intérprete de mandatos sencillo bajo UNIX
Autor: José luis López
Fecha: 13 - Marzo - 2001
Revisado: 19 - Marzo - 2003
*/

%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ejecucion.h"

extern int yylex (void);
extern void yyerror (char *cadena);

extern FILE *yyin;
extern char linea[];
extern int ilinea;

MLIS orden;
CLIS param;
char *cadena_accion;
char fichent[255];
char **argv;
char ***argvlis;

void linea2accion ( void )
{
	linea[ilinea] = '\0';
	cadena_accion = malloc ( (ilinea+1)*sizeof(char) );
	strcpy ( cadena_accion, linea );
}
%}

%start lineas

%union {
	int ival;
	char cad[256];
	CLIS clis;
	MLIS mlis;
	char car;
}

%token PWD
%token CD
%token UMASK
%token KILL NICE RENICE
%token TIME SLEEP
%token EXIT
%token WAIT
%token JOBS
%token <cad> FICHERO
%token <cad> OPCION
%token <cad> MENOSESE MENOSPE MENOSENE MENOSGE MENOSALGO MENOSJOTA
%token <cad> NUMERO

%type <mlis> cabecera_compleja
%type <clis> parametros
%type <car> ampersand

%%

lineas:
	lineas linea
	{	ilinea = 0; nuevo_mandato();
	}
	| /* nada */
	;

linea:
	mandato_interno
	| FICHERO parametros '>' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $1 );
		orden->sig = NULL;
		orden->params = $2;
		linea2accion();
		argv = accion2argv ( orden );
	 	ejecutar ( cadena_accion, NONICE, argv, NULL, $4, $5 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| FICHERO parametros '<' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $1 );
		orden->sig = NULL;
		orden->params = $2;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, NONICE, argv, $4, NULL, $5 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| FICHERO parametros '>' FICHERO '<' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $1 );
		orden->sig = NULL;
		orden->params = $2;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, NONICE, argv, $6, $4, $7 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| FICHERO parametros '<' FICHERO '>' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $1 );
		orden->sig = NULL;
		orden->params = $2;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, NONICE, argv, $4, $6, $7 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| FICHERO parametros ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $1 );
		orden->sig = NULL;
		orden->params = $2;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, NONICE, argv, NULL, NULL, $3 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| cabecera_compleja '|' FICHERO parametros '>' FICHERO ampersand '\n'
	{	concatM ( &orden, $1, $3, $4 );
		linea2accion();
		argvlis = acciones2argvlis ( orden );
		entubar ( cadena_accion, argvlis, (fichent[0]=='\0')?NULL:fichent, $6, $7 );
		liberarAVL ( argvlis );
		liberarM ( orden );
	}
	| cabecera_compleja '|' FICHERO parametros ampersand '\n'
	{	concatM ( &orden, $1, $3, $4 );
		linea2accion();
		argvlis = acciones2argvlis ( orden );
		entubar ( cadena_accion, argvlis, (fichent[0]=='\0')?NULL:fichent, NULL, $5 );
		liberarAVL ( argvlis );
		liberarM ( orden );
	}
	| '\n'
	| error '\n'
	{	yyerrok;
	}
	;

cabecera_compleja:
	FICHERO parametros '<' FICHERO
	{	strcpy ( fichent, $4 );
		concatM ( &$$, NULL, $1, $2 );
	}
	| FICHERO parametros
	{	fichent[0]='\0';
		concatM ( &$$, NULL, $1, $2 );
	}
	| cabecera_compleja '|' FICHERO parametros
	{	concatM ( &$$, $1, $3, $4 );
	}
	;

mandato_interno:
	PWD '\n'
	{	_pwd();
	}
	| CD '\n'
	{	_cd ( NULL );
	}
	| CD FICHERO '\n'
	{	_cd ( $2 );
	}
	| UMASK '\n'
	{	mostrar_mascara();
	}
	| UMASK NUMERO '\n'
	{	if ( no_es_octal ( $2 ) )
		{
			yyerror ( "error semántico" );
			yyerrok;
		}
		else
			establecer_mascara ( strtol ( $2, NULL, 8 ) );
	}
	| KILL MENOSESE NUMERO MENOSPE NUMERO '\n'
	{	_kill ( PROC, atoi($3), atoi($5) );
	}
	| KILL MENOSPE NUMERO MENOSESE NUMERO '\n'
	{	_kill ( PROC, atoi($5), atoi($3) );
	}
	| KILL MENOSGE NUMERO MENOSESE NUMERO '\n'
	{	_kill ( PGRP, atoi($5), atoi($3) );
	}
	| KILL MENOSESE NUMERO MENOSGE NUMERO '\n'
	{	_kill ( PGRP, atoi($3), atoi($5) );
	}
	| KILL MENOSESE NUMERO MENOSJOTA NUMERO '\n'
	{	_kill ( PJOB, atoi($3), atoi($5) );
	}
	| KILL MENOSJOTA NUMERO MENOSESE NUMERO '\n'
	{	_kill ( PJOB, atoi($5), atoi($3) );
	}
	| NICE MENOSENE NUMERO FICHERO parametros '>' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $4 );
		orden->sig = NULL;
		orden->params = $5;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, atoi($3), argv, NULL, $7, $8 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| NICE MENOSENE NUMERO FICHERO parametros '<' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $4 );
		orden->sig = NULL;
		orden->params = $5;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, atoi($3), argv, $7, NULL, $8 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| NICE MENOSENE NUMERO FICHERO parametros ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $4 );
		orden->sig = NULL;
		orden->params = $5;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, atoi($3), argv, NULL, NULL, $6 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| NICE MENOSENE NUMERO FICHERO parametros '>' FICHERO '<' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $4 );
		orden->sig = NULL;
		orden->params = $5;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, atoi($3), argv, $9, $7, $10 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| NICE MENOSENE NUMERO FICHERO parametros '<' FICHERO '>' FICHERO ampersand '\n'
	{	orden = malloc ( sizeof(struct mnodo) );
		strcpy ( orden->mandato, $4 );
		orden->sig = NULL;
		orden->params = $5;
		linea2accion();
		argv = accion2argv ( orden );
		ejecutar ( cadena_accion, atoi($3), argv, $7, $9, $10 );
		liberarAV ( argv );
		liberarM ( orden );
	}
	| RENICE MENOSENE NUMERO MENOSPE NUMERO '\n'
	{	_renice ( PROC, atoi($3), atoi($5) );
	}
	| RENICE MENOSPE NUMERO MENOSENE NUMERO '\n'
	{	_renice ( PROC, atoi($5), atoi($3) );
	}
	| RENICE MENOSENE NUMERO MENOSGE NUMERO '\n'
	{	_renice ( PGRP, atoi($3), atoi($5) );
	}
	| RENICE MENOSGE NUMERO MENOSENE NUMERO '\n'
	{	_renice ( PGRP, atoi($5), atoi($3) );
	}
	| EXIT '\n'
	{	YYACCEPT;
	}
	| WAIT '\n'
	{	_wait();
	}
	| JOBS '\n'
	{	_jobs();
	}
	| SLEEP NUMERO '\n'
	{	_sleep ( atoi($2) );
	}
	;

ampersand:
	/* nada */
	{	$$ = ' ';
	}
	| '&'
	{	$$ = '&';
	}
	;

parametros:
	/* nada */
	{	$$ = NULL;
	}
	| parametros MENOSENE
	{	concatC ( &$$, $1, $2 );
	}
	| parametros MENOSPE
	{	concatC ( &$$, $1, $2 );
	}
	| parametros MENOSESE
	{	concatC ( &$$, $1, $2 );
	}
	| parametros MENOSJOTA
	{	concatC ( &$$, $1, $2 );
	}
	| parametros MENOSALGO
	{	concatC ( &$$, $1, $2 );
	}
	| parametros FICHERO
	{	concatC ( &$$, $1, $2 );
	}
	| parametros NUMERO
	{	concatC ( &$$, $1, $2 );
	}
	;

%%

void yyerror ( char *cadena )
{
	fprintf ( stderr, "Error sintáctico\n" );
}

int no_es_octal ( char cadena[] )
{
	int i;

	for ( i = 0; i < strlen(cadena); i++ )
		if ( ( cadena[i] > '7' ) || ( cadena[i] < '0' ) )
			return ( 1 );
	return ( 0 );
}
