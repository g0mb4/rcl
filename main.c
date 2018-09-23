/* Robot Control Language Interpreter */
#include <stdio.h>
#include <pthread.h>

#include <signal.h>
#include <unistd.h>

#include "rcl.h"

bool running = true;

void * rcle_loop(void * data);

void sig_handler(int signo){
	/* Ctrl + C */
	if (signo == SIGINT) {
		running = false;
	}
}

int main(int argc, char ** argv){
	pthread_t rcle_thread;
	char string[1024];
	struct S_RCL_TYPE * parse_output = rcl_null();
	int ret;

#ifdef _DEBUG
	printf("Debug mode. %s %s\n", __DATE__, __TIME__);
#endif
	printf("Robot Control Language Interpreter\n\n");

	ret = pthread_create(&rcle_thread, NULL, rcle_loop, NULL);
	if(ret){
		fprintf(stderr,"pthread_create() failed with %d\n", ret);
		return 1;
	}

	if(rcle_init() < 0){
		return 2;
	}

	printf("Type 'help' for the available attributes, 'quit' to quit.\n\n");
	for(;running;){
		printf("> ");
		fgets(string, 1024, stdin);

		if(strncmp(string, "quit", 4) == 0){
			running = false;
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
	}

	pthread_join(rcle_thread, NULL);

	rcl_free(parse_output);
	rcle_destroy();
	return 0;
}

void * rcle_loop(void * data){
	struct S_RCL_TYPE * fcn_output = rcl_null();
	for(;running;){
		rcle_main_step(fcn_output);
		if(fcn_output->type != T_NULL){
			str_buf_t * sb = str_buf_create();
			rcl_val_to_str_buf(sb, fcn_output);
			printf("%s\n", sb->str);
			str_buf_destroy(sb);
		}
		usleep(100000);	// 100 ms
	}
	rcl_free(fcn_output);

	return NULL;
}
