/*  Program to build up a font file for use with nplot routines.  Code
   hacked from the program hertovplot.c written by Joe Dellinger, Stanford.
   G. Helffrich / Northwestern Geology / May 1988.

 This program converts the Hershey fonts in the form distributed with
    mod.sources into an nplot font file, given a destination file as the 
    first argument.
 First line of the file is the font height, font baseline position, and the
    scale factor.  The font height is in nplot units; 10 is a typical
    font size.  The font baseline position is to locate a Hershey font at
    the proper vertical position, since it is not given in the font
    definition.  The scale factor scales the size of the font to the size
    of 'font 0', the default nplot font.  This is an empirical factor that
    must be eyeballed.
 Subsequent lines of the file are: decimal starting font code, decimal Hershey 
    character code or range xxx-yyy.  Characters are copied from the
    range starting with the decimal font.  
 The "undefined" character in
    the font is a Hershey code and is used whenever a code not defined
    in the font is requested.  See "font.h" for the assignment of this
    code.  In must be defined in every font.
 Output is written to standard out giving information about the font.

 Use as follows:
   cat hersh.oc* | buildfont font001 > romanc_font

 *
 * You have to go in yourself and add such information as the letter spacing,
 * top height, bottom, etc, etc. The hershey fonts provide the left and right
 * width data, which is used, but do not provide data about where to center
 * the symbol vertically if it is used as a marker. This must be edited in
 * by hand if the default calculated by "makefont" is unacceptable.
 *
 * SEE ALSO: makefont.c, a program which "packs" Vplot font files into
 * ".include" fonts so that they can be included into font_definitions.h
 *  and thus be used with "gentext.c". It also makes runtime-loadable
 *  ".bin" fonts. It also gives the format of a vplot font in the comments
 *  at the start of the source!
 *
 * - Joe Dellinger, Stanford University Dept of Geophysics
 *
 * Bits of this code are stolen from "hershey.c" by James Hurt.
 *
 */

#include <stdio.h>
#include <strings.h>
#include "nplot.h"
#include "font.h"

/*
 * scanint: a function to scan an integer, using n characters of
 *          the input file, ignoring newlines. (scanf won't work
 *          because it also ignores blanks)
 */
int     scanint (n)
int     n;
{
    char    buf[20];
    int     i, c;

    for (i = 0; i < n; i++)
    {
	while ((c = getchar ()) == '\n');/* discard spare newlines */
	if (c == EOF)
	    return (-1);
	buf[i] = c;
    }

    buf[i] = 0;
    return (atoi (buf));
}

main (argc, argv)
int     argc;
char  **argv;
{
    int     ich, nch, i, j, x, y;
    int     font[MAX_CODE+1];
    FILE *fp, *dp;
    char    string[100];
    int     nc, font_v, baseline;
    float   scale;

    for (i=0; i<= MAX_CODE+1; i++) font[i]=0;

    if (argc != 3) {
	fprintf (stderr, 
	  "usage: %s font-file font-description\n", argv[0]);
	exit (1);
    }
    dp = fopen (argv[2], "r");
    if (dp == NULL) {
	fprintf (stderr, "%s: Can't open the file %s.\n", argv[0], argv[2]);
	exit (1);
    }
    fp = fopen (argv[1], "w");
    if (fp == NULL) {
	fprintf (stderr, "%s: Can't open the output file %s.\n",
	   argv[0], argv[1]);
	exit (1);
    }

    fscanf(dp, "%d %d %f", &font_v, &baseline, &scale);
    nc = 0; i = 0;
    while (fscanf (dp, "%s", string) == 1) {
	if (strlen(string) == 1) 
	   ich = (int) string[0];
	else
	   if (sscanf(string, "%d", &ich) != 1) {
	      fprintf(stderr, "Invalid font character code %s.\n", string);
	      exit(1);
	   }
	if (fscanf (dp, "%s", string) != 1) {
	   fprintf(stderr, "%s: Invalid format in file %s\n",
	      argv[0], argv[2]);
	      exit(1);
	}
	   
	if (index (string, '-')) {
	    for (j = atoi (string); j <= atoi (index (string, '-') + 1); j++) {
		if (font[ich] != 0) {
		   fprintf(stderr, 
		      "%s: Assignment to character %d previously as %d, now %d.\n",
		      argv[0], ich, font[ich], j);
		}
		font[ich++] = j;
		nc++;
	    }
	} else {
	    j = atoi (string);
	    if (font[ich] != 0) {
	      fprintf(stderr, 
		 "%s: Assignment to character %d previously as %d, now %d.\n",
		 argv[0], ich, font[ich], j);
	    }
	    font[ich] = j;
	    nc++;
	}
    }

    fprintf (stdout, "%d glyphs in this font.\n", nc);
    if (font[(int)fc_undefined] == 0) {
       fprintf(stderr, "'Undefined' character not defined in the font.\n");
       exit(1);
    }
    fprintf (stdout, "'Undefined' character is %d\n",
       font[(int)fc_undefined]);
    fprintf (fp, "%d %d %f\n", font_v, baseline, scale);

    for(;;) {
#define copyc(where) {char c; c=getc(stdin); if (!skipping) (void)putc(c,where);}
#define max(a,b) (a>b ? a : b)
#define min(a,b) (a<b ? a : b)
	int skipping = 1,
	   fc, lw, rw, hmin, hmax, vmin, vmax;

	if ((ich = scanint (5)) < 1) exit (0);
	nch = scanint (3);
	/* Are we going to use this character? */
	for (fc = 0; fc <= MAX_CODE; fc++)
	   if (font[fc] == ich) {
	      skipping = 0;
	      fprintf(fp,"%5d%3d", fc, nch);
	      break;
	   }

	for (i = 0; i < nch; i++) {
	    char c1, c2;
	    int v1, v2;

	    if ((i == 32) || (i == 68) || (i == 104) || (i == 140))
		copyc(fp); /* skip newlines */
	    c1 = getc(stdin); c2 = getc(stdin);
	    v1 = (int) c1 - (int)'R'; v2 = (int) c2 - (int)'R';
	    if (!skipping) {
	       (void) putc(c1, fp); (void) putc(c2, fp);
	    }
	    /* Accumulate max horiz. and vert. values. */
	    if (i == 0) {
	       lw = v1; rw = v2;
	       hmin = 1000; hmax = -1000;
	       vmin = 1000; vmax = -1000;
	    } else if (v1 != (int)' ' - (int)'R') {
	       hmin = min(hmin, v1); hmax = max(hmax, v1);
	       vmin = min(vmin, v2); vmax = max(vmax, v2);
	    }
	}
	copyc(fp);

	if (!skipping)
	   /* Print out values for this character. */
	   fprintf(stdout,
	      "Hershey- %d; Font code- %d: widths- %d, %d; horiz.- %d, %d; vert.- %d, %d.\n",
	      ich, fc, lw, rw, hmin, hmax, vmin, vmax);
    }
}
