#include <stdio.h>

#include "rcl.h"

int main(int argc, char ** argv){
	char string[256], parse_output_str[1024];
	struct S_RCL_TYPE * parse_output = rcl_null();

#ifdef _DEBUG
	printf("Debug mode. %s %s\n", __DATE__, __TIME__);
#endif

	memset(parse_output_str, 0, 1024);
	printf("Robot Control Language\n\n");
	for(;;){
		printf("> ");
		fgets(string, 256, stdin);
    		rcl_parse(parse_output, string);
		rcl_val_to_string(parse_output_str, parse_output);
		printf("%s\n", parse_output_str);
		memset(parse_output_str, 0, 1024);
	}

	return 0;
}
