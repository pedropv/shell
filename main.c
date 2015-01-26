/*
programa principal del intérprete de mandatos
Autor: José luis López
Fecha: 23 - Marzo - 2001
Revisado: 7 - Marzo - 2003
*/

#include <stdio.h>
#include "ejecucion.h"

extern int yyparse (void);

int main ()
{
	ejecucion_init();
	nuevo_mandato();
	yyparse();
	_exit_();
	return ( 1 ); /* nunca debería llegar aquí */
}
