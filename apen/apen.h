#include <sys/types.h>
#include <stdlib.h>
#ifdef SYSV
#undef BSD
#else
#define BSD
#endif
/* Size of paper in points */
#define XSIZE 		(8*300)		/* short way across paper */
#define YSIZE 		(11*300)	/* long way across paper */

/*
 * The following magic constants are the extent by which the
 * Imagen's claimed 300 dots/inch differs from reality
 */
#define XSCL		1.00925
#define YSCL		0.983875

#define ENDRAS		-1	/* end of raster data flag */
#define MAXPATHLEN	1000	/* Max Path length for PostScript (guessed) */
#define MOVE		1
#define DRAW		0

#define DRAW_PATH	0	/* Two different things that can be done */
#define FILL_PATH	1	/* to a path */

#define INITFILE "apen.setup"
#define TERMFILE "apen.wrapup"
#define PATFILE  "apen.pattern"

struct foot_info {		/* Information to put on page footer */
   int fi_footer;		/* 1 if footer desired */
   char *fi_user, *fi_dir, *fi_filename, *fi_date;
};

void rot();
void tran();
void scal();
void pusht();
void popt();
void area();
void dump();
void line();
void setshad();
void setfat();
void drawpath();
void putpoint();
void circshad();
void dopat();
void doplot();
void init_color();
void initpage();
void dumpfile();
void putras();
void putsym();
void error();
int is_scale();
int clip();
int solve();
int color();
int setpathop();
