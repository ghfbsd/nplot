#Test programs for some apen components.  This assmes that 'iplotdb', 
#  'pic', and 'apen' are all installed and functional.  The PostScript
#  generating version of troff is assumed to be called 'atroff.'

TROFF=ptroff

all: testpat testpic 

testpat: testpat.o
	f77 -o testpat testpat.o -lnplot

testpic: pic.plot
	apen -v -PPostScript pic.plot
	${TROFF} -me -t pictest.me | pic pic.plot | lpr -PPostScript
	${TROFF} -ms -t pictest.ms | pic pic.plot | lpr -PPostScript

pic.plot: pic.plot.db
	cp /dev/null pic.plot
	iplotdb pic.plot.db > pic.plot

clean:
	/bin/rm -f pic.plot testpat.o testpat
