# fichero para compilar y enlazar el intérprete de mandatos
# no debe ser modificado por el alumno si no está seguro de
# lo que está haciendo.
# Autor: José Luis López
# Fecha: 15 - Marzo - 2001

OBJS = ejecucion.o main.o lex.yy.o cmlis.o tareas.o y.tab.o 

CFLAGS += -g -Wall -pedantic

all: mish

lex.yy.o: lex.yy.c y.tab.h cmlis.h

cmlis.o: cmlis.c cmlis.h

tareas.o: tareas.c tareas.h

ejecucion.o: ejecucion.c ejecucion.h cmlis.h tareas.h

main.o: main.c ejecucion.h

y.tab.o: y.tab.c cmlis.h ejecucion.h

y.tab.c: grammar.y
	yacc -d grammar.y

y.tab.h: grammar.y
	yacc -d grammar.y

lex.yy.c: lexer.l
	lex lexer.l

mish: $(OBJS)
	cc -o mish $(OBJS)

clean:
	rm -f *.o y.tab.* lex.yy.c mish

# fin del Makefile
