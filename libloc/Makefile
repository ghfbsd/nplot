#	@(#)Makefile	Loclib 		1/23/84
CFLAGS=	-O
OWNER=	george
GROUP=	staff
DEST=	/usr/local/lib

SRC =	puth.c putw.c geth.c getw.c synbaps.c
OBJ =	puth.o putw.o geth.o getw.o synbaps.o

.c.o:
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	-mv a.out $*.o

ALL =	libloc.a llib-lloc.ln
ALL =	libloc.a
all:	${ALL}

libloc.a: ${OBJ}
	ar rv libloc.a ${OBJ}

llib-lloc.ln:
	lint -Cloc ${SRC}

install: ${ALL}
	install -o ${OWNER} -g ${GROUP} libloc.a ${DEST}
	ranlib ${DEST}/libloc.a
	: install -o ${OWNER} -g ${GROUP} llib-lloc.ln /usr/lib/lint

man:

clean:
	rm -f *.o
	rm -f libloc.a llib-lloc.ln


