#include <stdio.h>

main()
{
    char line[256];
    unsigned long value;
    int i, j;

    printf("    ");
    value = 0;
    for (i=0; i<32; i++) {
	gets(line);
	if (strlen(line) < 32) {
	    fprintf(stderr, "line %s too short\n", line);
	    exit(1);
	}
	for (j=0; j<32; j++) {
	    value = (value << 1) | ((line[j] == '.') ? 0 : 1);
	}
	printf(" 0x%08lX,", (unsigned long)value);
	value = 0;
	if (3==(i&3)) printf("\n    ");
    }
    printf("\n");
}
