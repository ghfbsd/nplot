/*
 * Include file for functions that read and write the digital bathymetry
 */

#define	PtPerDeg	12			/* Points per degree */
#define	DegPerRow	360			/* Degrees around globe */
#define	PtPerRow	(DegPerRow*PtPerDeg+1)	/* Store end twice */

#define	BAPFILE		"/usr3/topog/synbaps"

