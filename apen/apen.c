/*  Apen -- Program to take pltfil(5) format and print it on any Postscript
	    printer in Postscript.  Refer to a Postscript manual for
	    implementation details.  */
/* Change history:
	    3/88 - Written by John Werner.
	    6/88 - Implement Hershey and hardware-resident fonts.
		   G. Helffrich
	    7/88 - Make this work with the 'pic' program.
	    4/90 - Add -P option
		   G. Helffrich
	    4/90 - Search PRINTER and PLOTTER environment variables
		   G. Helffrich
	    9/90 - Implement -c and -# options in Postscript
	    3/92 - Implement footer on page
	    3/94 - Add color
	    9/02 - EPS output support
	    10/0 - Null printer name allowed
	    10/10 - Alternate data directory for PostScript preamble and
	           nplot font info.
            10/20 - Expand bounding box by +1 point so all lines included.
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <pwd.h>
#include "apen.h"

extern int vertical, embedded, pageno;
extern int pxmax, pymax, xmax, ymax, llx, lly, urx, ury;
extern int forcegray;
extern char *progname, *lib;

char *progname, *lib;
void lprfork();

int main(argc,argv)
    int argc; char **argv;
{
    register char *cptr;
    int standard_out=0, 
       triedplot=0, 
       fatbase=0, 
       copies=0;
    int epsfd;
    char *lpdev, *evar;
    char *getenv();
    struct foot_info footer;

    lib = LIB;
    progname = argv[0]; lpdev = PRINTER;

    /* Check if PRINTER environment variable is about.  
       If so, use its value as a plot file name.  */
    evar = getenv("PLOTTER");
    if (evar != NULL)
       lpdev = evar;
    else {
       evar = getenv("PRINTER");
       if (evar != NULL)
	  lpdev = evar;
    }

    /* Set up default footer information. */
    {  long now = time(0);
       struct timeval tp;
       struct passwd *pwd = (struct passwd *)getpwuid(getuid());
       footer.fi_footer = 1;
       footer.fi_user = malloc(1+strlen(pwd->pw_name));
       strcpy(footer.fi_user,pwd->pw_name);
       footer.fi_dir = getcwd(malloc(MAXPATHLEN),MAXPATHLEN);
       footer.fi_date = malloc(26);
       strcpy(footer.fi_date,ctime(&now));
    }


    while(--argc) {
	cptr = *++argv;
	if( is_scale(cptr) )
	    continue;
	if (*cptr == '-') {
	    cptr++;
	    switch(*cptr) {
		case '#':
		case 'c':
		    copies = atoi(++cptr);
		    break;
		case 'f':
		    fatbase = atoi(++cptr);
		    break;
		case 'g':
		    forcegray = 1;
		    break;
		case 'h':
		    vertical = 0;
		    /*
		     * Make the x axis on the long side eof the page.
		     * Make sure the Min's and Max's get set right.
		     */
		    break;
		case 'v':
		    vertical = 1;
		    cptr++;
		    if (*cptr == 'b') vertical += 2;
		    /*
		     * Make the y axis on the long side of the page.
		     * Make sure the Min's and Max's get set right.
		     */
		    break;
		case 'e':
		    /*
		     * EPS output
		     */
		    embedded = 1; standard_out = 1; copies = 0; vertical = 3;
		    footer.fi_footer = 0;
		    break;
		case 'r':
		    /*
		     * rotate to other orientation
		     */
		    vertical = !vertical;
		    break;
		case 't':
		    /*
		     * Send output to stdout; -e implies this
		     */
		    standard_out = 1;
		    break;
		case 'L':
		    /*
		     * Set explicit library directory name
		     */
		    lib = cptr+1;
		    break;
		case 'P':
		    /*
		     * Set explicit printer name
		     */
		    lpdev = cptr+1;
		    break;
		case 'F':
		    /*
		     * Suppress footer
		     */
		     footer.fi_footer = 0;
		     break;
		default:
		    fprintf(stderr, "%s: unknown option %s\n", 
		       progname, cptr);
		    exit(1);
		    break;
	    }
	} else 
	    break;
    }

    if (vertical) {
	pxmax = XSIZE;
	pymax = YSIZE;
    } else {
	pxmax = YSIZE;
	pymax = XSIZE;
    }
    xmax = pxmax; ymax = pymax; /* Set clipping bounds to max plot bounds */
    init_color(); /* Initialize color table */

    if (!standard_out)
	lprfork(lpdev);	/* Open up a pipe to "lpr" */

    if (!embedded) 
       printf("%%!PS-Adobe-3.0\n%%%%Pages: (atend)\n");
    else {
       /* Make temporary file, attach to stdout */
       char *epsnm = tempnam(NULL,"apen"); 
       epsfd = dup(1);
       if (epsnm == NULL || epsfd == -1 ||
          NULL == freopen(epsnm,"w+", stdout)) {
	  fprintf(stderr, "%s: unable to make temporary output file\n", 
		       progname);
	  exit(1);
       }
       unlink(epsnm);
    }
    fprintf(stdout, "%%%%Creator: apen (%s)\n", getenv("LOGNAME"));
    fprintf(stdout, "%%%%CreationDate: %s", footer.fi_date);
       
    dumpfile(INITFILE);		/* Some Postscript procedures we need */

    /* Handle multiple copies if requested */
    if (copies > 0)
       fprintf(stdout, "%%%%BeginSetup\n/#copies %d def\n%%%%EndSetup\n",
	  copies);

    while (argc--) {
	cptr = *argv++;
	triedplot++;
	if(freopen(cptr,"r", stdin) == NULL) {
	    (void) fflush(stdout);
	    fprintf(stderr,"%s:  cannot open %s\n",
	       progname,cptr);
	} else {
	    footer.fi_filename = cptr;
	    doplot(&footer,fatbase);
	}
    }
    if (!triedplot) {
	footer.fi_filename = "standard input";
	doplot(&footer,fatbase);
    }

    if (!embedded) printf("%%%%Trailer\n%%%%Pages: %d\n",pageno);
    dumpfile(TERMFILE);		/* Finish up output nicely */
    printf("%%%%EOF\n");
    if (embedded) {
       /* Insert magic lines, copy temp file to output */
       char bb[1024];
       int n;
       (void)strcpy(bb, "%!PS-Adobe-3.1 EPSF-3.0\n");
       (void)write(epsfd, bb, strlen(bb));
       snprintf(bb, sizeof(bb), "%%%%BoundingBox: %d %d %d %d\n",
          llx-1,lly-1,urx+1,ury+1);
       (void)write(epsfd, bb, strlen(bb));
       snprintf(bb, sizeof(bb), "%%%%Title: %s\n", footer.fi_filename);
       (void)write(epsfd, bb, strlen(bb));
       rewind(stdout);
       for(n = read(fileno(stdout), bb, sizeof(bb));
           n>0;
           n = read(fileno(stdout), bb, sizeof(bb))
       )
          (void)write(epsfd, bb, n);
    }

    return(0);
}


void error(s)
register char *s;
{
   (void) fflush(stdout);
   fprintf(stderr,"%s\n",s);
   exit(1);
}

/* dumpfile()
 * Copy a file into our output stream.  Useful for files that define
 * PostScript procedures and such things.
 */
void dumpfile(s)
char *s;
{
    register FILE *f;
    register int c, d;

    char *fn = malloc(strlen(lib)+strlen(s)+2);
    
    if (!fn) {
        fprintf(stderr, "%s:  Cannot allocate memory for file name.\n",
	    progname);
        exit(1);
    }
    sprintf(fn, "%s%s%s", lib, lib[strlen(lib)] == '/' ? "" : "/", s);

    if ((f = fopen(fn, "r")) == NULL) {
	fprintf(stderr, "%s: couldn't open %s\n", progname, fn);
	exit(1);
    }

    while ((c = getc(f)) != EOF)
	putchar(d=c);

    if (d != '\n') putchar('\n'); (void) fclose(f); (void) fflush(stdout);

    free(fn);
}

int	pipe_des[2];		/* pipe_des[0] is the pipe output,
				   pipe_des[1] is the input */
void lprfork(pname)
   char *pname;
{
	int child;

	/* Open pipe to lpr	 */
	if (pipe(pipe_des) != 0) {
		fprintf(stderr, "Cannot open pipe to lpr.\n");
		exit(1);;
	}

	/* fork off lpr */
	if ((child = fork()) < 0) {
		fprintf(stderr, "Cannot fork off lpr.\n");
		exit(1);
	}

	if (child == 0) {
		char printer[128], *lparg;

		if (strlen(pname)) {
#ifdef BSD
			sprintf(printer,"-P%s",pname);
#else
			sprintf(printer,"-d%s",pname);
#endif
			lparg = printer;
		} else
			lparg = NULL;
		/*
		 * This code block runs as a child process
		 */
		/* Close unused input end of pipe; "lpr" doesn't need it. */
		close(pipe_des[1]);

		/* Attach output end of pipe to the standard input of "lpr" */
		dup2(pipe_des[0], 0);

		/* exec lpr */
#ifdef BSD
		execl(LPR, "lpr", lparg, (char *)0);
#else
		execl(LPR, "lp", lparg, (char *)0);
#endif

		/* if we get here something is wrong */
		fprintf(stderr, "Cannot exec lpr.\n");
		exit(1);
	}

	/* Close unused output end of pipe; "apen" doesn't need it. */
	close(pipe_des[0]);

	/*
	 * Attach input end of pipe to our standard output.
	 * This may not be the most portable thing in the world.
	 */
	dup2(pipe_des[1], fileno(stdout));
}
