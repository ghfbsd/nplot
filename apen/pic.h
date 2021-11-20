
/* Various defines for pic */

#define FILE_NAME_LENGTH 256
#define BUFLEN 1024

typedef int Object;
#define oInt 1
#define oReal 2
#define oString 3
#define oName 4
#define oArray 5
#define oDict 6
#define oHex 7
#define oBool 8
#define oNull 9
#define oCmd 10

void getInit();

char *getBuf();

Object getObj();
