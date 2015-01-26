/*
funciones de manejo de las listas de mandatos (MLIS) y parámetros (CLIS)
Autor: José luis López
Fecha: 13 - Marzo - 2001
Revisado: 7- Marzo - 2003
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmlis.h"

void liberarC ( CLIS c )
{
	CLIS aborrar;

	while ( c!=NULL )
	{
		aborrar = c;
		c=c->sig;
		free ( aborrar );
	}
}

void liberarM ( MLIS m )
{
	MLIS aborrar;

	while ( m!=NULL )
	{
		aborrar = m;
		m = m->sig;
		liberarC ( aborrar->params );
		free ( aborrar );
	}
}

void concatC ( CLIS *plis, CLIS lis, char cad[] )
{   
	CLIS nuevo;
	CLIS *recorrer;

	nuevo = malloc ( sizeof(struct cnodo) );
	strcpy ( nuevo->cad, cad );
	nuevo->sig = NULL;
	for ( recorrer = &lis; *recorrer!=NULL; recorrer=&((*recorrer)->sig) );
	*recorrer=nuevo;
	*plis=lis;
}   

void concatM ( MLIS *plis, MLIS lis, char mandato[], CLIS param )
{
	MLIS nuevo;
	MLIS *recorrer;

	nuevo = malloc ( sizeof(struct mnodo) );
	strcpy ( nuevo->mandato, mandato );
	nuevo->sig = NULL;
	nuevo->params = param;
	for ( recorrer = &lis; *recorrer!=NULL; recorrer=&((*recorrer)->sig) );
	*recorrer=nuevo;
	*plis=lis;
}

int nparams ( MLIS accion )
{
	int cont;
	CLIS plis;

	for ( cont=0, plis=accion->params; plis!=NULL; cont++, plis=plis->sig );
	return ( cont );
}

int nmandatos ( MLIS acciones )
{
	int cont;
	MLIS plis;

	for ( cont = 0, plis = acciones; plis != NULL; cont++, plis = plis->sig );
	return ( cont );
}

char **accion2argv ( MLIS accion )
{
	int tam;
	char **argv;
	CLIS plis;
	int i;

	tam = nparams ( accion );
	argv = malloc ( sizeof(char *) * (tam+2) );
	argv[0] = accion->mandato;
	for ( i=1, plis=accion->params; plis!=NULL; i++, plis=plis->sig )
		argv[i] = plis->cad;
	argv[i] = NULL;
	return ( argv );
}

char ***acciones2argvlis ( MLIS acciones )
{
	int num_mandatos;
	MLIS pmlis;
	int i;
	char ***argvlis;

	num_mandatos = nmandatos ( acciones );
	argvlis = malloc ( sizeof ( char ** ) * ( num_mandatos + 1 ) );
	for ( i = 0, pmlis = acciones; pmlis != NULL; i++, pmlis = pmlis->sig )
		argvlis[i] = accion2argv ( pmlis );
	argvlis[i] = NULL;
	return ( argvlis );
}

void liberarAV ( char **argv )
{
	free ( argv );
}

void liberarAVL ( char ***argvlis )
{
	int i;

	for ( i = 0; argvlis[i] != NULL; i++ )
		liberarAV ( argvlis[i] );
	free ( argvlis );
}
