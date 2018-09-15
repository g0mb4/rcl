#include <stdio.h>

#include "rcl.h"

int main(int argc, char ** argv){
	char string[256];
	struct S_RCL_TYPE * parse_output = rcl_null();

#ifdef _DEBUG
	printf("Debug mode. %s %s\n", __DATE__, __TIME__);
#endif

	printf("Robot Control Language\n\n");
	for(;;){
		printf("> ");
		fgets(string, 256, stdin);
    	rcl_parse(parse_output, string);

		str_buf_t * sb = str_buf_create();
		rcl_val_to_str_buf(sb, parse_output);
		printf("%s\n", sb->str);
		str_buf_destroy(sb);
	}

	return 0;
}
