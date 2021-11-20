# Makefile to construct nplot fonts.
#   G. Helffrich, May 8, 1988
#
#   The makefile assumes that the variable FONTFILE has been given on
#      the 'make' command line.
#
CFLAGS=	-g
FONTS=	fR fI fB fBI fGG fRG fEG fH fC fS

all: ${FONTS} buildfont

buildfont: buildfont.c
	cc ${CFLAGS} -I.. -o buildfont buildfont.c

install: ${FONTS}
	for i in [0-9][0-9][0-9]; do \
	   install -o ${OWNER} -g ${GROUP} -m 444 $$i ${FONTFILE}$$i ; \
	done

fR: 001
fI: 002
fB: 003
fBI: 004
fGG: 005
fRG: 006
fEG: 007
fH: 008
fC: 009
fS: 010

001: 001.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

002: 002.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

003: 003.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

004: 004.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

005: 005.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

006: 006.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

007: 007.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

008: 008.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

009: 009.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

010: 010.def buildfont
	zcat Hershey_fonts/hersh.oc* | buildfont $@ $@.def > $@.out

clean:
	(cd Hershey_fonts; make clean)
	rm -f *.out *.o buildfont [0-9][0-9][0-9]
