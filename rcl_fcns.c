#include "rcl.h"

int conn_get(rcl_type_t * param){
	if(param != NULL){
		printf("conn: use without a parameter!\n");
		return -1;
	}

	printf("works!");
	return 0;
}

const rcl_attr_desc_t rcl_attr_fcns[] = {
	{ "conn", conn_get, NULL, NULL, NULL, NULL },

	/* end of structure, do not change! */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};
