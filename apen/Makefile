# BIN and LIB are dummy entries set by the Makefile in the directory above
DEST = /usr/local/bin
LIB  = /usr/local/lib
MAN  = /usr/local/man
LPR = /usr/bin/lpr
MANEXT= 1
OWNER = george
GROUP = staff
FONT_FILE=	nplotfont
PRINTER= PostScript
PRINTER= 
PROGRAM= ${DEST}/apen
LDJUNK = `echo ${LD_LIBRARY_PATH} | awk -F: '{for(i=1;i<=NF;i++)printf "-L%s\n",$$(i)}'`
LDJUNK =

# Files that apen needs to read from a library directory
LIBFILES= apen.setup apen.wrapup apen.pattern

CFLAGS = -O
CFLAGS = -g -Wno-implicit

OBJ =	clip.o color.o doplot.o dump.o externs.o initpage.o\
	is_scale.o io.o putras.o apen.o area.o putsym.o font.o\
	paths.o pattern.o circshad.o setshad.o setfat.o line.o

SRC =	externs.c\
	apen.c\
	is_scale.c\
	io.c\
	doplot.c\
	initpage.c\
	putras.c\
	putsym.c\
	area.c\
	pattern.c\
	dump.c\
	paths.c\
	clip.c\
	circshad.c \
	color.c \
	setshad.c \
	setfat.c \
	font.c \
	line.c

apen: ${OBJ}
	cc ${CFLAGS} -o apen ${OBJ} ${LDJUNK} -lm

parseps.o: pic.h
npic: pic.c pic.h parseps.o
	cc ${CFLAGS} -o npic pic.c parseps.o '-DPROGRAM="${PROGRAM}"'

font.o: font.c Makefile
	cc ${CFLAGS} -c '-DFONTFILE_PREFIX="${FONT_FILE}"' font.c

apen.o: apen.c Makefile
	cc ${CFLAGS} -c '-DPRINTER="${PRINTER}"' \
			'-DLPR="${LPR}"' \
			'-DLIB="${LIB}"' \
	   apen.c

install: apen npic
	-install -m 775 -s -o ${OWNER} -g ${GROUP} apen ${DEST}
	-install -m 775 -s -o ${OWNER} -g ${GROUP} npic ${DEST}
	-for i in ${LIBFILES}; do \
	   install -c -m 664 -o ${OWNER} -g ${GROUP} $$i ${LIB}; \
	done

install-man:
	-[ ! -d ${MAN}/man${MANEXT} ] && mkdir ${MAN}/man${MANEXT}
	-install -m 444 -c -o ${OWNER} -g ${GROUP} apen.1 ${MAN}/man${MANEXT}/apen.${MANEXT}
	-install -m 444 -c -o ${OWNER} -g ${GROUP} pic.1 ${MAN}/man${MANEXT}/npic.${MANEXT}

clean:
	rm -f apen npic *.o a.out core Makefile.bak

lint:
	lint -hxbac ${SRC} -lloc

save:
	rm -f Save/*
	cp *.c *.h Makefile apen.csh Save

depend:
	cat </dev/null >x.c
	for i in *.c ; do \
		(grep '^#[ 	]*include' x.c $$i | sed \
			-e '/\.\.\/h/d' \
			-e '/<\(.*\)>/d' \
			-e 's/:[^"]*"\([^"]*\)".*/: \1/' \
			>>makedep); done
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo 'r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/,$$s/\.c:/.o:/' >> eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile
	/bin/rm -f eddep makedep x.c


# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
apen.o: apen.h
area.o: apen.h
area.o: pat.h
circshad.o: apen.h
circshad.o: pat.h
color.o: apen.h
color.o: pat.h
doplot.o: apen.h
dump.o: apen.h
externs.o: apen.h
font.o: font.h
font.o: chars.h
initpage.o: apen.h
line.o: apen.h
parseps.o: pic.h
paths.o: apen.h
pattern.o: apen.h
pattern.o: pat.h
pic.o: pic.h
putras.o: apen.h
putsym.o: font.h
setshad.o: apen.h
setshad.o: pat.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
