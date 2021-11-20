#define	RASTERS	300	/* number of pixels per inch */
#define PI 3.141592654	/* pi */
#define PLOTFILE "pltfil" /* default plot file name */
#define EV_PLOTFILE "pltfil" /* environment variable for plot file name */
#define EV_NPLOT_DEBUG "nplot_debug" /* environment variable for debug output */

extern FILE *plotfile;

extern short fatsave, origin_offset;

extern struct fonts font_list;

#define getisz()	(origin_offset)

#define MIN(a,b)	((a)>(b)?(b):(a))
#define MAX(a,b)	((a)>(b)?(a):(b))
