#include <stdio.h>
#include "nplot.h"

#define NUMCOL 23

typedef struct {
    unsigned char r, g, b;
} colrec;

colrec colors[NUMCOL] = {
    {   0,   0,   0 },	/* Black	*/
    { 255, 255, 255 },	/* White	*/
    { 255,   0,   0 },	/* Red		*/
    { 255, 150, 150 },	/* Pink		*/
    { 255, 128,   0 },	/* Orange	*/
    { 255, 255,   0 },	/* Yellow	*/
    {   0, 130,   0 },	/* Dark Green	*/
    {   0, 180,   0 },	/* Light Green	*/
    {   0, 255,   0 },	/* Bright-Green	*/
    {   0, 220, 180 },	/* Cyan		*/
    {   0, 200, 200 },	/* Cyan		*/
    {   0, 180, 220 },	/* Cyan		*/
    {   0,   0, 170 },	/* Navy Blue	*/
    {   0,   0, 255 },	/* Dark Blue	*/
    { 150, 150, 255 },	/* Light Blue	*/
    { 160,   0, 240 },	/* Violet	*/
    { 200,   0, 200 },	/* Purple	*/
    { 240,   0, 160 },	/* Magenta	*/
    { 255, 200, 139 },	/* Light "Brown"*/
    { 128, 100,  40 },	/* Brown	*/
    { 190, 190, 190 },	/* Light Gray	*/
    { 150, 150, 150 },	/* Gray		*/
    { 110, 110, 110 },	/* Dark Gray	*/
};

ncolor_(col, para)
    int *col;
    float *para;
{
    
	if(para[11]*para[19] == 0. ) {
	    fprintf(stderr, "NCOLOR: error, nplot has not been called\n");
	    return(-1);
	}


	if (*col < 0 || *col >= NUMCOL)
	    fprintf(stderr, "NCOLOR: Illegal color %d, using black\n", *col);

	putc('C', plotfile);
	putc(colors[*col].r, plotfile);
	putc(colors[*col].g, plotfile);
	putc(colors[*col].b, plotfile);
	return(0);
}

nrgb_(red, green, blue, para)
    int *red, *green, *blue;
    float *para;
{
    int r, g, b;

    if(para[11]*para[19] == 0. ) {
	fprintf(stderr, "NRGB: error, nplot has not been called\n");
	return(-1);
    }

    r = *red; g = *green; b = *blue;

    if (*red < 0 || *red > 255) {
	fprintf(stderr, "NRGB: error, red must be in range 0-255\n");
	*red = (*red < 0) ? 0 : 255;
    }

    if (*green < 0 || *green > 255) {
	fprintf(stderr, "NRGB: error, green must be in range 0-255\n");
	*green = (*green < 0) ? 0 : 255;
    }

    if (*blue < 0 || *blue > 255) {
	fprintf(stderr, "NRGB: error, blue must be in range 0-255\n");
	*blue = (*blue < 0) ? 0 : 255;
    }

    putc('C', plotfile);
    putc(r, plotfile);
    putc(g, plotfile);
    putc(b, plotfile);
    return(0);
}
