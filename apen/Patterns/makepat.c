#include <stdio.h>

main()
{
    char line[256];
    unsigned long value;
    int i, j;

    printf("    \"");
    value = 0;
    for (i=0; i<16; i++) {
	gets(line);
	if (strlen(line) < 16) {
	    fprintf(stderr, "line %s too short\n", line);
	    exit(1);
	}
	for (j=0; j<16; j++) {
	    value = (value << 1) + ((line[j] == '.') ? 0 : 1);
	}
	if (i % 2) {
	    printf("%08lX", (unsigned long)value);
	    value = 0;
	}
    }
    printf("\",\n");
}
	
