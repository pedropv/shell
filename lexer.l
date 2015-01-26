/*
analizador léxico para el intérprete de mandatos bajo UNIX
Autor: José Luis López
Fecha: 13 - Marzo - 2001
Revisado: 19 - Marzo - 2003
*/

%{
#include <unistd.h>
#include <stdio.h>
#include <glob.h>
#include <string.h>
#include "cmlis.h"
#include "y.tab.h"

extern void expandir(void);
extern void concat_linea(void);
extern void concat_linea_c(char c);
char linea[1024];
int ilinea=0;
%}

numero	[+-]?[0-9]+
quot	\"[^\"]+\"
caresp	[\ \t\n\*\?\\\|&\<\>]
carnor	[^\ \t\n\*\?\\\|&\<\>]
opcion	\-{carnor}+
nombre	{carnor}+
prefix	\\
esccar	{prefix}{caresp}
comodin	[\*\?]
espec	{comodin}|{esccar}
patron	{nombre}*{espec}+{nombre}*
fichero	{patron}+

%START	ENMEDIO

%%

%{
/* BEGIN INITIAL; */
%}

" "|\t	{ concat_linea(); /* saltar los espacios en blanco y los tabuladores */ }

"<"	{ concat_linea(); return ( yytext[0] ); }
">"	{ concat_linea(); return ( yytext[0] ); }
"|"	{ concat_linea(); return ( yytext[0] ); }
"&"	{ concat_linea(); return ( yytext[0] ); }
\n	{ concat_linea(); BEGIN INITIAL; return ( yytext[0] ); }
-s	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSESE ); }
-p	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSPE ); }
-n	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSENE ); }
-g	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSGE ); }
-j	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSJOTA ); }

<ENMEDIO>pwd { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>cd	{ concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>kill { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>exit { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>sleep { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>nice { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>renice	{ concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>jobs { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>wait { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }
<ENMEDIO>umask { concat_linea(); strcpy(yylval.cad,yytext); return ( FICHERO ); }

<ENMEDIO>\"pwd\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"cd\"	{ concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"kill\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"exit\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"sleep\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"nice\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"renice\"	{ concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"jobs\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"wait\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }
<ENMEDIO>\"umask\" { concat_linea(); yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }

\"pwd\"	{ concat_linea(); BEGIN ENMEDIO; return ( PWD ); }
\"cd\"	{ concat_linea(); BEGIN ENMEDIO; return ( CD ); }
\"kill\" { concat_linea(); BEGIN ENMEDIO; return ( KILL ); }
\"exit\" { concat_linea(); BEGIN ENMEDIO; return ( EXIT ); }
\"sleep\" { concat_linea(); BEGIN ENMEDIO; return ( SLEEP ); }
\"nice\" { concat_linea(); BEGIN ENMEDIO; return ( NICE ); }
\"renice\" { concat_linea(); BEGIN ENMEDIO; return ( RENICE ); }
\"jobs\" { concat_linea(); BEGIN ENMEDIO; return ( JOBS ); }
\"wait\" { concat_linea(); BEGIN ENMEDIO; return ( WAIT ); }
\"umask\" { concat_linea(); BEGIN ENMEDIO; return ( UMASK ); }

pwd	{ concat_linea(); BEGIN ENMEDIO; return ( PWD ); }
cd	{ concat_linea(); BEGIN ENMEDIO; return ( CD ); }
kill { concat_linea(); BEGIN ENMEDIO; return ( KILL ); }
exit { concat_linea(); BEGIN ENMEDIO; return ( EXIT ); }
sleep { concat_linea(); BEGIN ENMEDIO; return ( SLEEP ); }
nice { concat_linea(); BEGIN ENMEDIO; return ( NICE ); }
renice { concat_linea(); BEGIN ENMEDIO; return ( RENICE ); }
jobs { concat_linea(); BEGIN ENMEDIO; return ( JOBS ); }
wait { concat_linea(); BEGIN ENMEDIO; return ( WAIT ); }
umask { concat_linea(); BEGIN ENMEDIO; return ( UMASK ); }

{quot}	{ concat_linea(); BEGIN ENMEDIO; yytext[yyleng-1]='\0'; strcpy(yylval.cad,&yytext[1]); return ( FICHERO ); }

{numero}	{ concat_linea(); strcpy(yylval.cad,yytext); return ( NUMERO ); }
{opcion}	{ concat_linea(); strcpy(yylval.cad,yytext); return ( MENOSALGO ); }

{nombre}	{ concat_linea(); BEGIN ENMEDIO; strcpy(yylval.cad,yytext); return ( FICHERO ); }
{fichero}	{ concat_linea(); expandir(); }

.	{ concat_linea(); return ( yytext[0] ); }

%%

void expandir ( void )
{
	glob_t pglob;
	int fildes[2];
	FILE *outfile;
	int i,c;
	int en_tuberia;

	en_tuberia = ( yyin!=stdin);
	/* printf ( "lexer: %s\n", yytext ); */
	pipe ( fildes );
	outfile = fdopen ( fildes[1], "w" );
	pglob.gl_offs = 0;
#ifdef __linux__
	glob ( yytext, GLOB_MARK | GLOB_NOCHECK | GLOB_TILDE, NULL, &pglob );
#else
#ifdef __FreeBSD__
	glob ( yytext, GLOB_MARK | GLOB_NOCHECK | GLOB_QUOTE | GLOB_TILDE, NULL, &pglob );
#endif
#endif
	i=0;
	while ( i<pglob.gl_pathc )
	{
		fprintf ( outfile, " \"%s\"", pglob.gl_pathv[i] );
		/* printf ( "\"%s\"\n", pglob.gl_pathv[i] ); */
		i++;
	}
	globfree ( &pglob );
	while ( (c=input())!='\n')
	{
		if (!en_tuberia) concat_linea_c ( c );
		fputc(c,outfile);
	}
	fputc(c,outfile);
	fclose(outfile);
	if ( en_tuberia )
		fclose(yyin);
	yyin = fdopen ( fildes[0], "r" );
}

int yywrap (void)
{
	if ( yyin==stdin )
		return ( 1 );
	else
	{
		fclose ( yyin );
		yyin = stdin;
		return ( 0 );
	}
}

void concat_linea ( void )
{
	int j, longitud;

	if ( yyin==stdin )
	{
		longitud = strlen ( yytext );
		for ( j=0; j<longitud; j++, ilinea++ )
			linea[ilinea] = yytext[j];
	}
}

void concat_linea_c ( char c )
{ linea[ilinea++] = c; }
