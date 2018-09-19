/* Robot Control Language Interpreter */

#include <stdio.h>

#include "rcl.h"

int main(int argc, char ** argv){
	char string[1024];
	struct S_RCL_TYPE * parse_output = rcl_null();
	struct S_RCL_TYPE * fcn_output = rcl_null();

#ifdef _DEBUG
	printf("Debug mode. %s %s\n", __DATE__, __TIME__);
#endif
	printf("Robot Control Language Interpreter\n\n");

	if(rcle_init() < 0){
		return 1;
	}

	printf("Type 'help' for the available attributes, 'quit' to quit.\n\n");
	for(;;){
		printf("> ");
		fgets(string, 1024, stdin);

		if(strncmp(string, "quit", 4) == 0){
			break;
		} else if(strncmp(string, "help", 4) == 0){
			rcle_print_attrs();
		} else {
			if(rcl_parse(parse_output, string) == 0){
				if(parse_output->type != T_NULL){
					str_buf_t * sb = str_buf_create();
					rcl_val_to_str_buf(sb, parse_output);
					printf("%s\n", sb->str);
					str_buf_destroy(sb);
				}
			}
		}

		rcle_main_step(fcn_output);
		if(fcn_output->type != T_NULL){
			str_buf_t * sb = str_buf_create();
			rcl_val_to_str_buf(sb, fcn_output);
			printf("%s\n", sb->str);
			str_buf_destroy(sb);
		}
	}

	rcl_free(parse_output);
	rcl_free(fcn_output);
	rcle_destroy();
	return 0;
}
