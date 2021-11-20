# Makefile cleaned up for RHEL 8 2021-08-28 crb

DEST=	/usr/local/bin
MANDIR=	/usr/man/manl
OWNER=	eao578
GROUP=	eao578
CFLAGS=	-ansi -Wno-implicit

all:	plotdb iplotdb

install: all
	install -s -o ${OWNER} -g ${GROUP} plotdb ${DEST}
	install -s -o ${OWNER} -g ${GROUP} iplotdb ${DEST}

plotdb.o: plotdb.h
plotdb:	 plotdb.o
	cc plotdb.o -o plotdb /usr/local/lib/libnplot-64.a

iplotdb.o: iplotdb.h plotdb.h
iplotdb: iplotdb.o 
	cc -O iplotdb.o -o iplotdb /usr/local/lib/libnplot-64.a

install-man:
	install -c -o ${OWNER} -g ${GROUP} plotdb.1g ${MANDIR}/plotdb.l
	install -c -o ${OWNER} -g ${GROUP} plotfil.5 ${MANDIR}/plotfil.l

clean:
	rm -f *.o plotdb iplotdb core pltfil
