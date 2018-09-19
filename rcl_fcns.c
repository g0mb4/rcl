#include "rcl.h"

void conn_get(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self){
	if(in->type != T_NULL){
		rcl_copy(out, rcl_error("no parameter is needed"));
	} else {
		rcl_copy(out, rcl_string("works!"));
	}

	RCL_RETURN( self );
}

const rcl_attr_desc_t rcl_attr_fcns[] = {
	{ "conn",
	  conn_get,
	  NULL,
	  NULL,
	  NULL,
	  NULL },

	/* end of structure, do not change! */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};
