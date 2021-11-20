/*  Pic -- Program to take Postscript output and search it 
	   for picture designations, then splice the picture into the text 
	   of the page.  
	   
	   George Helffrich, May. 1990  */

/* PROGRAM should be defined via cc ... -DPROGRAM= to be the
      appropriate file name.  */

#include <stdio.h>
#include <sys/wait.h>
#include <strings.h>
#include "pic.h"

char *progname;

main(argc,argv)
    int argc; 
    char **argv;
{
    register char *cptr;
    register int c;
    int count = 0;
    int bufcount = 0;
    FILE *flist = NULL;
    char filename[FILE_NAME_LENGTH];
    char buf[BUFLEN];
    char pspat1[64], pspat2[64], fnpat[64], *pspat1s, *pspat2s;
    float scale;

    /*  Arguments are the list of plot file names to be associated with
	each picture.  They are used one-for-one as pictures are found
	in the input.  
	
	Arguments may also be -f file-name which designates a file in which
	to find the names of the plot files to include in the document.  */
    progname = *argv;
    if (--argc) {
       cptr = *++argv; scale = 1.0;
       if (!strcmp(cptr,"-f")) {  /* Appears to be a file name */
	  if ((!--argc) || ((flist = fopen(*++argv,"r")) == NULL))
	     error("unable to open file with plot file names");
	  if (--argc)
	     error("too many parameters");
	  cptr = filename; 
	  (void) sprintf(fnpat, "%%%ds %%f",FILE_NAME_LENGTH-1);
	  if (fscanf(flist, fnpat, cptr, &scale) < 1)
	     cptr = "/dev/null";
       }
    } else
       cptr = "/dev/null";

    /*  Pattern match on first characters of input to determine type
	of output to produce.  */
    (void) fgets(buf,BUFLEN,stdin); if (0 != strncmp(buf,"%!",2)) 
       error("File not Postscript output, quitting");
    (void) fputs(buf,stdout);

    /*  Read input looking for our string indicating a picture.  Build
	scan pattern, and short forms.  */
#if 0
    (void) sprintf(pspat1, "%%%d[^(](\\242)%%%d[^\n]",BUFLEN-1,BUFLEN-1);
    (void) sprintf(pspat2, "%%%d[^<]<a2>%%%d[^\n]",BUFLEN-1,BUFLEN-1);
    pspat1s = strstr(pspat1,"(\\242)"); pspat2s = strstr(pspat2,"<a2>");
#else
    pspat1s = "(\\242)"; pspat2s = "<a2>";
#endif
    while(NULL != fgets(buf,sizeof(buf),stdin)) {
       char tail[BUFLEN], head[BUFLEN], *p;
       int n;

rescan:
#if 0
       if ((2 == (n=sscanf(buf,pspat1,head,tail))) ||
	   (2 == (n=sscanf(buf,pspat2,head,tail))) ||
	   (1 == (n=sscanf(buf,pspat1s,tail)))     ||
	   (1 == (n=sscanf(buf,pspat2s,tail)))) {
	  /* Have a picture -- take name from argv value */
	  if (n == 2) (void) fprintf(stdout,"%s\n",head);
	  (void) sprintf(head,"(\\ )%s\n",tail);
#else
       if ((NULL != (p=strstr(buf,pspat1s))) ||
           (NULL != (p=strstr(buf,pspat2s)))) {
	  /* Have a picture -- take name from argv value */
	  if (p != buf) (void) fwrite(buf,sizeof(char),p-buf,stdout);
	  n = strncmp(p,pspat1s,strlen(pspat1s)) ?
	     strlen(pspat2s) : strlen(pspat1s);
	  (void) sprintf(head,"(\\ )%s\n",p+n);
#endif
	  getInit(head, stdin); while (oCmd != getObj())
	     /* repeatedly eat up input until an operator found */;
	  { char *p = getBuf(&n);
	    (void) strncpy(head,p,n); head[n]='\0'; (void) strcpy(tail,p+n);
	    (void) fprintf(stdout,"%s\n",head);
	  }
	  insert_plot(cptr, scale);
	  count = 0; bufcount = 0;
	  if (flist != NULL) {
	     scale = 1.0; if (fscanf(flist, fnpat, cptr, &scale) < 1)
		cptr = "/dev/null";
	  } else {
	     if (--argc)
		cptr = *++argv;
	     else
		cptr = "/dev/null";
	  }
	  if (!(*tail == '\n' && strlen(tail)==1)) {
	     (void) strcpy(buf,tail);
	     goto rescan;
	  }
       } else {
	  /*  Dump this output line. */
	  (void) fputs(buf, stdout);
	  *buf = '\0';
       }
    }
	       
    (void) fflush(stdout);
    exit(0);
}


insert_plot(s, scale)
   char *s;
   float scale;
{
   /* Fork ourselves and call PROGRAM to plot the file passed
      as the argument name. */
   int pid;
   int result;
   int i, c;
   char argscale[64];
   char buf[BUFLEN];
   char *eof;
   FILE *file;

   /* Flush any text out of the pipe, so stuff doesn't
      get interleaved with the text we've copied so far. */
   (void) fflush(stdout);

   /* Open file and examine first characters to see whether it is
      a plot file or a PostScript file */
   if (NULL == (file = fopen(s,"r"))) goto badfile;

   eof = fgets(buf,BUFLEN,file); 
   if ((eof != NULL) && (0 == strncmp(buf,"%!",2))) {
      /* File is PostScript, copy it to standard output as-is */
      puts("/npic_state save def");
      puts("/npic_#dict countdictstack def");
      puts("/npic_#op count 1 sub def");
      puts("currentpoint");       /* push point for translate before newpath */
      puts("userdict begin");
      puts("/showpage{}def");
      puts("0 setgray 0 setlinecap 1 setlinewidth");
      puts("0 setlinejoin 10 setmiterlimit");
      puts("[ ] 0 setdash newpath");
      puts("/languagelevel where {");
      puts("   pop languagelevel 1 ne {");
      puts("      false setstrokeadjust false setoverprint");
      puts("   } if");
      puts("} if");

      puts("translate"); /* Move origin before scaling */
      puts("/cmat matrix currentmatrix def"); /* Get current */
      puts("/dmat matrix defaultmatrix def"); /* Get default */
      puts("dmat 0 get cmat 0 get div"); /* calculate scale factors */
      puts("dmat 3 get cmat 3 get div");
      puts("scale"); /* set up new scale. */

      /* Copy file, but omit troublesome text that may spoil document structure
         conventions */
      for (; NULL != eof; eof = fgets(buf, BUFLEN, file))
	 if ((0 != strncmp("%%EOF",buf,5)) &&
	     (0 != strncmp("%%Trailer",buf,9))) (void) fputs(buf, stdout);
      puts("count npic_#op sub {pop} repeat");
      puts("countdictstack npic_#dict sub {end} repeat");
      puts("npic_state restore");
      (void) fclose(file);

   } else {
      /* File is plot file.  Fork ourselves and exec the program 
	 to convert the plot file to PostScript. */
      (void) fclose(file);
      (void) sprintf(argscale,"-%f",scale);
      if ((pid = fork()) == 0)
	 execl(PROGRAM, PROGRAM, "-e", argscale, s, 0);
      /*
      pid = wait(&result);
      */
      if (!wait4(pid,&result,0,NULL)) goto badfile;
      if ((WIFEXITED(result) && (WEXITSTATUS(result) != 0)) ||
	  !WIFEXITED(result)) {
badfile:
	 fprintf(stderr,"%s: error drawing figure in %s\n",progname,s);
      }
   }
   (void) fflush(stdout);
}

error(s)
   char *s;
/* Print error message string and terminate */
{
   fprintf(stderr,"%s: %s.\n",progname,s);
   exit(1);
}
