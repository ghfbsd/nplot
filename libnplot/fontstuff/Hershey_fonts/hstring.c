/*
   Program hacked from hertogrid.c (by J. Dellinger) to print out strings
   of characters through nplot.  */

#define Max(a,b)	((a) > (b) ? (a) : (b))
#define Min(a,b)	((a) < (b) ? (a) : (b))
#include <stdio.h>
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

int     scale = 11, offs = (int)'R';
char xl[] = "", yl[] = "", Title[] = "Test string";

struct character {
   int id, stroke_count;
   char stroke[100][2];
   };

   void nmove_(), nline_(), nplot_();

main (argc,argv)
int argc;
char **argv;
{
    int     ich, nch, i, x, y, k, kn;
    int     line[2][1000], line_letters[1000];
    int     move, ipnt;
    float para[22], xx, yy, ox, oy;
#define MAX_LETTERS 30
    int letters[MAX_LETTERS], letter_count = 0;
    struct character *chars[500],
       *new = NULL;

    /* Parameters are a string of numbers that we're to print. */

    for (i=1; i < argc && i < MAX_LETTERS; i++){
       /* Find next letter. */
       if (sscanf(argv[i],"%d",&letters[i-1]) != 1){
	  fprintf(stderr," Invalid letter code.\n");
	  exit(1);
       }
       letter_count++;
    }

    /* Now read in font files and search for the characters */
    for (k = 0; k < letter_count && (ich = scanint(5)) > 0; ) {
	int nch;
	char *malloc();

	if (new == NULL) new = (struct character *) 
	    malloc((unsigned) sizeof(struct character));
	if (new == NULL) {
	   fprintf(stderr,"Unable to allocate more storage.\n");
	   exit(1);
	}
	nch = scanint (3);

	new->stroke_count = nch;
	new->id = ich;
	for (i = 0; i < nch; i++) {
	    if ((i == 32) || (i == 68) || (i == 104) || (i == 140))
		getchar ();/* skip newlines */
	    new->stroke[i][0] = getchar ();
	    new->stroke[i][1] = getchar ();
	}
	getchar ();

	/* Save pointer for this letter */
	for (i = 0; i < letter_count; i++) {
	   if (ich == letters[i]) {
	      chars[ich] = new;
	      new = NULL;
	      k++;
	      break;
	   }
	}
    }

   /* Have located all the letters in the file that we need.  Print
      them out one-by-one. */

     for (i = 0; i < 22; i++)
	para[i] = 0.0;
     para[0] = 6.0; para[1] = 6.0; /* Size in inches */
     para[4] = 30*11*scale; para[5] = para[4] /* 5*11*scale */;
     para[16] = 1.0; para[17] = 1.0; /* Omit x and y grid */
     { int one = 1;
        nplot_(&one, &xx, &yy, xl, yl, Title, para);
     }

     ox = 11 * scale; oy = 11 * scale;
     for (i = 0; i < letter_count; i++) {
	/* Print out each letter in the string */
	int maxx, minx, lowx;

	new = chars[letters[i]];
	if (new == NULL) {
	   fprintf(stderr, "character %d isn't defined in this font.\n",
	      letters[i-1]);
	   continue;
	}
	lowx = new->stroke[0][0] - offs;
	fprintf(stdout, "character %d: left %d, right %d",
	   new->id, new->stroke[0][0] - offs,
		    new->stroke[0][1] - offs);
	maxx = new->stroke[1][0] - offs;
	minx = maxx;

	ox -= scale * (new->stroke[0][0] - offs);
	nmove_(&ox, &yy, para);
	move = 1;
	for (ipnt = 1; ipnt < new->stroke_count; ipnt++) {
	    if (new->stroke[ipnt][0] == ' ')
	       move = 1;
	    else {
	       int xv, yv;
	       xv = new->stroke[ipnt][0] - offs;
	       yv = new->stroke[ipnt][1] - offs;
	       xx = scale*xv + ox;
	       yy = oy - scale*yv;
	       maxx = Max(maxx,xv); minx = Min(minx,xv);
	       if (move)
		  nmove_(&xx, &yy, para);
	       else
		  nline_(&xx, &yy, para);
	       move = 0;
	    }
	}
	/*
	ox += scale * (new->stroke[0][1] - new->stroke[0][0] );
	ox += scale * (maxx - minx + 2);
	*/
	ox += scale * (new->stroke[0][1] - (int) 'R' );
	fprintf(stdout, "; max x %d, min x %d\n",
	   maxx, minx);
    }
}
