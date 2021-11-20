# Makefile for nplot stuff
#
#	make		Compiles source code
#	lib		Makes object library
#	install		Puts object code in library, compiling source
#			if necessary
#	clean		removes object files

# Use code optimizers
FC = gfortran
FFLAGS = -O -m32
FFLAGS = -g
CFLAGS = -O
CFLAGS = -g

OBJS=	nplot.o const.o geth.o getw.o nshad.o npat.o ncolor.o puth.o putw.o font.o
LIB=	libnplot
DEST=	/usr/local/lib
MAN=	/usr/local/man/man3
OWNER=	george
GROUP=	staff

FONTFILE= ${DEST}/nplotfont

LIB32= ${LIB}-32.a
LIB64= ${LIB}-64.a

libs: ${LIB32} ${LIB64}

lib ${LIB}: ${OBJS}
	ar rv ${LIB} ${OBJS}
	ranlib ${LIB}

${LIB32}:
	/bin/rm -f ${OBJS}
	make ${MFLAGS} \
		LIB="${LIB32}" \
		CFLAGS="${CFLAGS} -m32" \
		FFLAGS="${FFLAGS} -m32" \
		lib

${LIB64}:
	/bin/rm -f ${OBJS}
	make ${MFLAGS} \
		LIB="${LIB64}" \
		CFLAGS="${CFLAGS} -m64" \
		FFLAGS="${FFLAGS} -m64" \
		lib

install-lib: ${LIB32} ${LIB64}
	for f in ${LIB32} ${LIB64} ; do \
	   install -m 775 -o ${OWNER} -g ${GROUP} $$f ${DEST} ; \
	   ranlib ${DEST}/$f ; \
	done

fonts:
	(cd fontstuff; \
	 make ${MFLAGS} all FONTFILE=${FONTFILE} )

install-fonts:
	(cd fontstuff; \
	 make ${MFLAGS} install \
	    FONTFILE=${FONTFILE} GROUP=${GROUP} OWNER=${OWNER} )

objs: ${OBJS}

font.o: font.c font.h
	cc ${CFLAGS} -c font.c -DFONTFILE_PREFIX='"${FONTFILE}"'

nplot.o: nplot.h nplot.c
	cc ${CFLAGS} -c nplot.c

install: install-lib install-fonts install-man

man install-man:
	-[ ! -d ${MAN} ] && mkdir ${MAN}
	install -c -m 664 -o ${OWNER} -g ${GROUP} nplot.3x ${MAN}/nplot.3

debug: nplot.c const.f
	cc  -c -g nplot.c
	$(FC) -c -g const.f
	ar rv debuglib.a nplot.o const.o
	ranlib debuglib.a

clean:
	rm -f *.o
	rm -f ${LIB}*.a debuglib.a
	rm -f pltfil a.out core
	rm -f nplot.l
	(cd fontstuff; make ${MFLAGS} clean)
