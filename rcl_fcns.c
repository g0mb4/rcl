#include "rcl.h"

void conn_get(rcl_type_t * out, rcl_type_t * in){
	/* no parameter is needed for this example */
	if(in != NULL){
		rcl_copy(out, rcl_null());
	}

	rcl_copy(out, rcl_string("works!"));
}

const rcl_attr_desc_t rcl_attr_fcns[] = {
	{ "conn", conn_get, NULL, NULL, NULL, NULL },

	/* end of structure, do not change! */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};
