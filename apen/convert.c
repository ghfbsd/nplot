#include <stdio.h>
main()
{
	int number;
	char string[100];
	do {
	    scanf("%d %s\n", &number, string);
	    printf("s/%o$/o %s/\n", number, string);
	}while(!feof(stdin));
}
