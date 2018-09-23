#include "rcl.h"

typedef struct S_CONN_DATA {
	char * str;
	uint32_t ctr;
} conn_data_t;

static conn_data_t conn_data = { "conn_str", 0 };

void conn_get(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self){
	if(in->type != T_NULL){
		rcl_copy(out, rcl_error("no parameter is needed"));
	} else {
		conn_data_t * data = ((conn_data_t *)self->data);

		rcl_copy(out, rcl_string(data->str));
	}

	RCL_RETURN( self );
}

void conn_start(rcl_type_t * out, rcl_type_t * in, rcle_process_t * self){
	if(in->type != T_LIST){
		rcl_copy(out, rcl_error("list parameter is needed"));
		RCL_RETURN( self );
	}

	conn_data_t * data = ((conn_data_t *)self->data);
	rcl_type_t * element = rcl_list_get(in, data->ctr++);
	if(element->type == T_ERR){
		rcl_copy(out, rcl_null());
		RCL_RETURN( self );
	} else {
		dbg_val( element );
	}
	rcl_free(element);
}

const rcl_attr_desc_t rcl_attr_fcns[] = {
	{ "conn",
	  (void *)&conn_data,
	  sizeof(conn_data),
	  conn_get,
	  NULL,
	  NULL,
	  conn_start,
	  NULL },

	/* end of structure, do not change! */
	{ NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL }
};
