#include <stdio.h>

#include "rcl.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse(void);
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

extern char error_msg[256];

int main(int argc, char ** argv){
#ifdef _DEBUG
	printf("Debug mode. %s %s\n", __DATE__, __TIME__);
#endif
	char string[256];

	printf("Robot Control Language\n\n");
	strcpy(error_msg, "");
	for(;;){
		printf("> ");
		fgets(string, 256, stdin);
    	YY_BUFFER_STATE buffer = yy_scan_string(string);
    	yyparse();
    	yy_delete_buffer(buffer);

		if (strlen(error_msg) != 0) {
			fprintf(stderr, "ERROR: %s", error_msg);
			memset(error_msg, 0, 256);
		}
	}

	return 0;
}
