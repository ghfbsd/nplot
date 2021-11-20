# Make file for sunplot and ancillary programs.
MANDEST=  /usr/local/man/man1
DEST=  /usr/local/bin
LIB=   /usr/local/lib
OWNER= george
GROUP= staff
FONT_FILE=	${LIB}/nplotfont
LDJUNK = `echo ${LD_LIBRARY_PATH} | awk -F: '{for(i=1;i<=NF;i++)printf "-L%s\n",$$(i)}'`
LDJUNK =

# Following are for SGI systems
#XLIBS=	-lXaw -lXmu -lXext -lXt -lX11 -lm
#CFLAGS = -g -I/usr/include/X11 -DSGI -xansi

# Following are for SunOS4.x systems
#XLIBS=	-lpixrect -lXaw -lXmu -lXext -lXt -lX11 -lm
#CFLAGS = -g -I/usr/openwin/include

# Following are for Linux systems
# XLIBS=	-L/usr/X11/lib -lXaw -lXmu -lXext -lXt -lX11 -lm
# CFLAGS = -g -I/usr/include/X11 -Wno-implicit

# Following are for Darwin systems
XLIBS=	-L/usr/X11R6/lib -lXaw -lXmu -lXext -lXt -lX11 -lm
CFLAGS = -g -I/usr/X11R6/include

OBJ =	clip.o doplot.o dump.o externs.o \
	is_scale.o xplot.o area.o putsym.o font.o \
	paths.o pattern.o circshad.o setshad.o setfat.o line.o color.o

SRC =	externs.c\
	xplot.c\
	is_scale.c\
	doplot.c\
	font.c\
	putsym.c\
	area.c\
	pattern.c\
	dump.c\
	paths.c\
	clip.c\
	circshad.c \
	setshad.c \
	setfat.c \
	line.c \
	color.c

xplot: ${OBJ}
	cc ${CFLAGS} -o xplot ${OBJ} ${LDJUNK} ${XLIBS}

font.o: font.c Makefile
	cc ${CFLAGS} -c '-DFONTFILE_PREFIX="${FONT_FILE}"' font.c

install: xplot
	install -m 775 -s -o ${OWNER} -g ${GROUP} xplot ${DEST}

install-man:
	-[ ! -d ${MANDEST} ] && mkdir ${MANDEST}
	install -m 444 -c -o ${OWNER} -g ${GROUP} xplot.man ${MANDEST}/xplot.1

clean:
	rm -f xplot *.o a.out core Makefile.bak

lint:
	lint -hxbac ${SRC} -lloc

depend:
	cat </dev/null >x.c
	for i in *.c ; do \
		(/usr/bin/grep '^#[ 	]*include' x.c $$i | sed \
			-e '/\.\.\/h/d' \
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
area.o:#include <stdio.h>
area.o: xplot.h
area.o: pat.h
circshad.o:#include <stdio.h>
circshad.o: xplot.h
circshad.o: pat.h
clip.o:#include <stdio.h>
clip.o: xplot.h
color.o:#include <stdio.h>
color.o: xplot.h
color.o: pat.h
doplot.o:#include	<stdio.h>
doplot.o:#include	<ctype.h>
doplot.o:#include	<strings.h>
doplot.o: xplot.h
doplot.o:#include <limits.h>
doplot.non-sgi.o:#include	<stdio.h>
doplot.non-sgi.o:#include	<ctype.h>
doplot.non-sgi.o:#include	<strings.h>
doplot.non-sgi.o: xplot.h
dump.o:#include <stdio.h>
dump.o: xplot.h
externs.o:#include <stdio.h>
externs.o: xplot.h
externs.non-sgi.o:#include <stdio.h>
externs.non-sgi.o: xplot.h
font.o:#include <stdio.h>
font.o:#include <string.h>
font.o: font.h
font.o: chars.h
is_scale.o:#include	<stdio.h>
is_scale.o:#include	<ctype.h>
is_scale.o: xplot.h
line.o:#include <stdio.h>
line.o: xplot.h
paths.o:#include <stdio.h>
paths.o: xplot.h
pattern.o:#include <stdio.h>
pattern.o: xplot.h
pattern.o: pat.h
pattern.non-sgi.o:#include <stdio.h>
pattern.non-sgi.o: xplot.h
pattern.non-sgi.o: pat.h
putsym.o:#include <stdio.h>
putsym.o: xplot.h
putsym.o: font.h
setfat.o:#include <stdio.h>
setfat.o: xplot.h
setshad.o:#include <stdio.h>
setshad.o: xplot.h
setshad.o: pat.h
xplot.o: xplot.h
xplot.o:#include <X11/Xaw/Label.h>
xplot.o:#include <sys/termios.h>
xplot.non-sgi.o: xplot.h
xplot.non-sgi.o:#include <X11/Xaw/Label.h>
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
