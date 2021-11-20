/*
 * plotfile debugger, part 2 of 2
 * modified i/o and text routines for PDP->VAX conversion
 *
 * siplotdb.c     Craig Bina, 1982, 1985
 */

#include	<stdio.h>
#include        <stdlib.h>	/* added stdlib.h 2021-08-29 crb */
#include	"plotdb.h"
#include	"iplotdb.h"

#define MAXLINE		256
#define err(f,s)	{ fprintf(stderr,f,s); exit(1); }
#define ck(e,f)		if(e) err(f,line)
#define cck(e)		ck(e,CFORMERR)
#define pck(e)		ck(e,PFORMERR)
#define tck(e)		ck(e,TFORMERR)
#define rck(e)		ck(e,RFORMERR)
#define putlong(w)	vputw(w,stdout)

FILE *pltin, *temp;
char line[MAXLINE];
char *cptr;

main(argc, argv)
    int argc;
    char **argv;
{
    FILE *freopen();

    pltin = stdin;

    do {	/* loop over arg list */
	argc--;
	argv++;
	cptr = *argv;
	if (argc && (*cptr == '-')) {
	    fprintf(stderr, "Usage: iplotdb [file] ...\n");
	    exit(1);
	}
	if (argc) {
	    if ((temp = freopen(cptr, "r", pltin)) == NULL) {
		fprintf(stderr, "iplotdb: cannot open %s\n", cptr);
		continue;
	    } else {
		pltin = temp;
	    }
	}
	doproc();
    } while (argc);
    exit(1);	/* fixed empty argument 2021-08-28 crb */
}

doproc()
{
    register int e;
    int x, y, xmax, ymax, fat, r, g, b;

    while (getline(line, MAXLINE) != EOF) {
	switch (*line) {	/* command list */
	  case '*':	/* error or comment line */
	    break;
	  case 'm':	/* move */
	    e = sscanf(line, MOVE, &x, &y);
	    cck(e - 2);
	    putchar('m');
	    putword(x);
	    putword(y);
	    break;
	  case 'd':	/* draw */
	    e = sscanf(line, DRAW, &x, &y);
	    cck(e - 2);
	    putchar('d');
	    putword(x);
	    putword(y);
	    break;
	  case 't':	/* text */
	    procsym();
	    break;
	  case 'b':	/* break */
	    e = strcmp(line, BREAK);
	    cck(e);
	    putchar('b');
	    break;
	  case 'e':	/* erase */
	    e = strcmp(line, ERASE);
	    cck(e);
	    putchar('e');
	    break;
	  case 'f':	/* fat */
	    e = sscanf(line, FATBASE, &fat);
	    cck(e - 1);
	    putchar('f');
	    putword(fat);
	    break;
	  case 'r':	/* raster data */
	    procras();
	    break;
	  case 'w':	/* window */
	    e = sscanf(line, WINDOW, &x, &xmax, &y, &ymax);
	    cck(e - 4);
	    putchar('w');
	    putword(x);
	    putword(xmax);
	    putword(y);
	    putword(ymax);
	    break;
	  case 'x':	/* display x-hairs */
	    e = strcmp(line, XHAIRS);
	    cck(e);
	    putchar('x');
	    break;
	  case 'p':	/* purge pltout buffers */
	    e = strcmp(line, PURGE);
	    cck(e);
	    putchar('p');
	    break;
	  case 'n':	/* no op */
	    e = strcmp(line, NOOP);
	    cck(e);
	    putchar('n');
	    break;
	  case 'a':	/* shade polygon */
	    procarea();
	    break;
	  case 'A':	/* shade polygon with pattern */
	    procapat();
	    break;
	  case 'c':	/* shade circle */
	    proccirc();
	    break;
	  case 'l':	/* draw line */
	    procline();
	    break;
	  case 'C':	/* RGB color */
	    e = sscanf(line, COLOR, &r, &g, &b);
	    cck(e - 3);
	    putchar('C');
	    putchar(r);
	    putchar(g);
	    putchar(b);
	    break;
	  default:	/* error */
	    err(COMERR, line);
	}
    }
}

procarea()
{
    int x, y;
    int npts, shade, xmask, ymask;
    register int i, e;

    e = sscanf(line, AREAKEY, &xmask, &ymask, &npts);
    cck(e - 3);
    if (xmask == ymask)
	ymask = 0;
    shade = (xmask & 0377) | ((ymask & 0377) << 8);
    putchar('a');
    putword(shade);
    putword(npts);
    for (i = 0; i <= npts; i++) {
	egetline(line, MAXLINE);
	e = sscanf(line, AREAPOINT, &x, &y);
	pck(e - 2);
	putword(x);
	putword(y);
    }
}

procline()
{
    int x, y;
    int npts, shade, xmask, ymask;
    register int i, e;

    e = sscanf(line, LINE, &npts);
    cck(e - 1);
    putchar('l');
    putword(npts);
    for (i = 0; i < npts; i++) {
	egetline(line, MAXLINE);
	e = sscanf(line, POINT, &x, &y);
	pck(e - 2);
	putword(x);
	putword(y);
    }
}

procapat()
{
    int x, y;
    int npts, pattern;
    register int i, e;

    e = sscanf(line, APATKEY, &pattern, &npts);
    cck(e - 2);
    putchar('A');
    putword(pattern);
    putword(npts);
    for (i = 0; i <= npts; i++) {
	egetline(line, MAXLINE);
	e = sscanf(line, AREAPOINT, &x, &y);
	pck(e - 2);
	putword(x);
	putword(y);
    }
}

proccirc()
{
    int x, y, rad, xmask, ymask, shade, e;

    e = sscanf(line, CIRCLE, &xmask, &ymask, &x, &y, &rad);
    cck(e - 5);
    if (xmask == ymask)
	ymask = 0;
    shade = (xmask & 0377) | ((ymask & 0377) << 8);
    putchar('c');
    putword(shade);
    putword(x);
    putword(y);
    putword(rad);
}

procsym()
{
    register int e;
    int size, orient, key, temp, font;
    char c;

    e = sscanf(line, TEXTKEY, &size, &orient, &font);
    cck(e - 3);
    key = (size & 037) | ((orient << 5) & 0140) | ((font << 8) & 0xff00);
    putchar('t');
    putlong(key);
    while (egetline(line, MAXLINE)) {
	switch (*line) {
	  case '0':
	    e = sscanf(line, TEXTNUM, &temp);
	    tck(e - 1);
	    if ((temp == 0377) || (temp == 0)) {
		putchar(temp);
		textend();
		return;
	    }
	    c = temp;	/* Put into an normal character variable */
	    break;
	  case ' ':
	    e = sscanf(line, TEXTCHAR, &c);
	    tck(e - 1);
	    break;
	  case 'E':
	    e = sscanf(line, TEXTCESC, &c);
	    if (e - 1)
		e = sscanf(line, TEXTNESC, &c);
	    tck(e - 1);
	    putchar(033);
	    if ((c == -1) || (c == 0)) {
		putchar((c == 0) ? 0 : 0377);
		textend();
		return;
	    }
	    break;
	  case '\\':
	    line[0] = ' ';
	    e = sscanf(line, TEXTCHAR, &c);
	    tck(e - 1);
	    switch (line[1]) {
	      case 'b':
		c = 010;
		break;
	      case 't':
		c = 011;
		break;
	      case 'n':
		c = 012;
		break;
	      case 'r':
		c = 015;
		break;
	      default:
		line[0] = '\\';
		err(TFORMERR, line);
	    }
	    break;
	  default:
	    err(TFORMERR, line);
	}
	putchar(c);
    }
}

procras()
{
    register int e;
    int w, count, offset;

    e = strcmp(line, RASTHEAD);
    cck(e);
    putchar('r');
    while (egetline(line, MAXLINE)) {
	e = sscanf(line, RASTKEY, &count, &offset);
	pck(e - 2);
	w = (count & 0377) | ((offset & 0377) << 8);
	putword(w);
	if (w == -1) {
	    egetline(line, MAXLINE);
	    e = strcmp(line, RASTEND);
	    ck(e, RENDERR);
	    return;
	}
	while (count-- > 0) {
	    egetline(line, MAXLINE);
	    e = sscanf(line, RASTWORD, &w);
	    rck(e - 1);
	    putword(w);
	}
    }
}

putword(w)
    int w;
{
    vputh(w, stdout);
    return;
}

getline(s, max)
    char s[];
int max;

{
    int i;
    int c;

    i = 0;
    while ((--max) && ((c = getc(pltin)) != EOF) && (c != '\n'))
	s[i++] = c;
    if (c == '\n')
	s[i++] = c;
    s[i] = '\0';
    if (feof(pltin))
	i = EOF;
    return (i);
}

egetline(s, m)
    char s[];
int m;

{
    register int i;

    if ((i = getline(s, m)) == EOF) {
	fprintf(stderr, "iplotdb:  premature EOF: %s\n", cptr);
	exit(1);
    }
    return (i);
}

textend()
{
    register int e;

    getline(line, MAXLINE);
    e = strcmp(line, TEXTEND);
    ck(e, TENDERR);
}
