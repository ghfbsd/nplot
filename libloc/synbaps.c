#include <stdio.h>
#include "synbaps.h"

/*
 * Read in a hunk of the SYNBAPS data
 */
int synbaps_(array, xdarg, slona, flona, slata, flata)
    register float *array;
    register int *xdarg;
    int *slata, *flata, *slona, *flona;
{
    register int xdim, j, y, i, x;
    int xstart, xend, ystart, yend;
    int slat, flat, slon, flon, err=0;
    FILE *bapf;

    xdim = *xdarg;
    slat = *slata;
    flat = *flata;
    slon = *slona;
    flon = *flona;

    if (slat < -90 || slat > 90 || flat < -90 || flat > 90) {
	fprintf(stderr, "SYNBAPS: Latitudes must be between -90 and 90.\n");
	return(-1);
    }

    if (slat >= flat) {
	fprintf(stderr, "SYNBAPS: Slat must be less than Flat.\n");
	return(-2);
    }

    if (slon < 0)
	slon += 360;
    if (flon < 0)
	flon += 360;
    if (slon < 0 || slon > 360 || flon < 0 || flon > 360) {
	fprintf(stderr, "SYNBAPS: longitudes must be between -360 and 360.\n");
	return(-3);
    }

    bapf = fopen(BAPFILE, "r");
    if (bapf == NULL) {
	fprintf(stderr, "SYNBAPS: couldn't open %s.\n", BAPFILE);
	return(-4);
    }

    xstart = slon * PtPerDeg;
    xend   = flon * PtPerDeg;

    if (xend < xstart) {
	/*
	 * Crossing over the 0 meridian
	 */
	int xs, xf, err;
	if (xend + (360*PtPerDeg - xstart) + 1 > xdim) {
	    fprintf(stderr, "SYNBAPS: xdim is too small.\n");
	    err = -5;
	    goto cleanup;
	}
	xs = slon; xf = 360;
	err = synbaps_(array, xdarg, &xs, &xf, slata, flata);
	if (err)
	    goto cleanup;
	xs = 0; xf = flon;
	err = synbaps_(array + (360 - slon)*PtPerDeg,
					xdarg, &xs, &xf, slata, flata);
	goto cleanup;
    }

    if (xend - xend + 1 > xdim) {
	fprintf(stderr, "SYNBAPS: xdim is too small.\n");
	err = -5;
	goto cleanup;
    }

    ystart = (slat+90) * PtPerDeg;
    yend   = (flat+90) * PtPerDeg;

    printf("xstart,end = %d,%d;  ystart,end = %d,%d\n",
		xstart, xend, ystart, yend);

    for (y = ystart, j = 0; y <= yend; y++, j++) {
	err = fseek(bapf, (long)sizeof(short)*(y*PtPerRow + xstart), 0);
	if (err == -1) {
	    fprintf(stderr, "SYNBAPS: fseek returns error; can't read dat.!\n");
	    err = -6; goto cleanup;
	}
	for (x = xstart, i = 0; x <= xend; x++, i++) {
	    *(array + j*xdim + i) = sgeth(bapf);
	    if (ferror(bapf)) {
		fprintf(stderr, "SYNBAPS: error reading element %d,%d.\n",i,j);
		fclose(bapf);
		err = -6;
		goto cleanup;
	    }
	}
    }

cleanup:
    fclose(bapf);
    return(err);
}

