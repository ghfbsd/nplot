NPLOT=/usr/local/lib/libnplot.a

fontplot: fontplot.o rdch.o
	f77 -g -o fontplot fontplot.o rdch.o ${NPLOT}

clean: ; /bin/rm -f *.o core fontplot
