#	Makefile	1.00	83/08/20
#
DEST=		/usr/local/bin
LIB=		/usr/local/lib
MANDEST=	/usr/man/manl
CFLAGS=		-g
OWNER=		bin
GROUP=		staff

# Programs that live in subdirectories, and have makefiles of their own.
#
SUBDIR= apen geochem ghostview-1.5 gs262 gzip kermit mac map md5 mm2.7 \
	netpbm onnet perl \
	tek patch plates plotdb pmotion sunplot top unproto xdvi xplot \
	xpdf-0.80 xv-3.10a

# Shell scripts that need only be installed and are never removed.
#   Left out:  tipunhang where
SCRIPT= blook cdmnt deblock enblock enscript f77list findex gmt lq lpqrot \
	man pad passwd pstogif q refprint run speller vgrind xdpy

# C programs that live in the current directory and do not need
# explicit make lines.
#
STD=	chtimes isatty label mktemp rewind rstat tplook unhqx
STD=

# C programs that live in the current directory and need explicit make lines.
# (make depend has special rules for these files)
#
NSTD= ansitar asciitops cpm digit execasroot moonphase npic sunrise timediff \
   umodem pltcomb
NSTD= pltcomb

# Programs that must run setuid to root
#   Left out:  tipunhang
SETUIDSCRIPT=	cdmnt fdmnt lpqrot
SETUIDCODE= execasroot

# Programs that have man pages to be installed.
#
MAN=	ansitar chtimes cpm digit f77list isatty moonphase pad npic pltcomb \
	pstogif recipes sunrise timediff tplook unhqx

.f.o: ;	-f77 ${FFLAGS} -c $< 

all:	${SUBDIR} ${STD} ${NSTD}

${SUBDIR}: ALWAYS
	test -d $@ && ( cd $@; make ${MFLAGS} DEST=${DEST} LIB=${LIB} ) || true

${STD}:
	-test -f $@.c && ${CC} ${CFLAGS} -o $@ $@.c

ALWAYS:

# Nonstandard programs requiring explicit make lines:
#
ansitar:	ansitar.c
	cc ${CFLAGS} -o ansitar -DBSD_42 ansitar.c

asciitops:	asciitops.c
	cc ${CFLAGS} -o asciitops -DA4 asciitops.c

cpm: cpm.c
	cc -o -Bstatic cpm cpm.c

digit:	digit.o
	cc ${CFLAGS} -o digit digit.o -lm

execasroot: execasroot.o ingroup.o
	cc ${CFLAGS} `uname | awk '/Linux/{print "-static"}/SunOS/{print "-Bstatic"}'` -o execasroot execasroot.o ingroup.o

moonphase: moonphase.c moontx.c moontx.h
	cc ${CFLAGS} -o moonphase moonphase.c moontx.c -lm

# Ncomb needs -lloc for vgeth and vputh...
ncomb:	ncomb.o
	cc ${CFLAGS} -o ncomb ncomb.o -L/usr/local/lib -lloc

npic: npic.c Makefile
	cc ${CFLAGS} -DPROGRAM=\"${DEST}/apen\" -o npic npic.c

pltcomb: pltcomb.c
	cc ${CFLAGS} -o pltcomb pltcomb.c -L/usr/local/lib -lloc

sunrise: sunrise.c
	cc ${CFLAGS} -o sunrise sunrise.c -lm

timediff: timediff.c julday.c
	cc ${CFLAGS} -o timediff timediff.c julday.c -lm

umodem:	umodem.c
	cc ${CFLAGS} -o umodem -DVER7 umodem.c

install: install-script install-src install-man

install-src: ${STD} ${NSTD}
	if [ ! -d ${DEST} ] ; then \
	   mkdir ${DEST} ; \
	fi
	-for i in ${SUBDIR}; do \
		test -d $$i && (cd $$i; make ${MFLAGS} DEST=${DEST} install) ||\
                true; \
        done
	-for i in ${STD} ${NSTD}; do \
		(install -s -o ${OWNER} -g ${GROUP} $$i ${DEST}/$$i); \
	done
	-for i in ${SETUIDCODE}; do \
		(chown root ${DEST}/$$i; chmod o+s ${DEST}/$$i); done

install-script:
	if [ ! -d ${DEST} ] ; then \
	   mkdir ${DEST} ; \
	fi
	-for i in ${SCRIPT}; do \
		(install -c -o ${OWNER} -g ${GROUP} $$i.sh ${DEST}/$$i); done
	-ln ${DEST}/cdmnt ${DEST}/fdmnt
	if [ ! -f /usr/bin/f77 ]; then \
	   install -c -o ${OWNER} -g ${GROUP} missing.sh ${DEST}/f77; fi
	if [ ! -f /usr/ucb/pc ]; then \
	   install -c -o ${OWNER} -g ${GROUP} missing.sh ${DEST}/pc; fi
	-for i in ${SETUIDSCRIPT}; do \
		(chown root ${DEST}/$$i; chmod u+s ${DEST}/$$i); done

install-man:
	if [ ! -d ${MANDEST} ] ; then \
	   mkdir ${MANDEST} ; \
	fi
	-for i in ${MAN}; do \
           test -f $$i.1 && (install -c -m 444 $$i.1 ${MANDEST}/$$i.l); \
        done
	-for i in ${SUBDIR}; do \
	   test -d $$i && (cd $$i; make ${MFLAGS} install-man) || true; \
	done
	PATH=/etc:/usr/bin catman l &

clean:
	rm -f a.out core *.s *.o
	for i in ${SUBDIR}; do \
           test -d $$i && (cd $$i; make ${MFLAGS} clean) || true; \
        done
	rm -f ${STD} ${NSTD}; rm -rf *.dSYM

depend:
	cat </dev/null >x.c
	for i in ${STD}; do \
		(echo $$i: $$i.c >>makedep; \
		/usr/bin/grep '^#[ 	]*include' x.c $$i.c | sed \
			-e '/\.\.\/h/d' \
			-e 's/:[^"]*"\([^"]*\)".*/: \1/' \
			-e 's/\.c//' >>makedep); done
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep x.c
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

# Files listed in ${NSTD} have explicit dependencies given below
#
# DO NOT DELETE THIS LINE -- make depend uses it

chtimes: chtimes.c
isatty: isatty.c
label: label.c
mktemp: mktemp.c
rewind: rewind.c
rstat: rstat.c
tplook: tplook.c
unhqx: unhqx.c
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
