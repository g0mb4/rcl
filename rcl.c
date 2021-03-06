#include "rcl.h"

int rcl_parse(struct S_RCL_TYPE * parse_output, const char * s){
	int ret = 0;

	YY_BUFFER_STATE buffer = yy_scan_string(s);
	ret = yyparse(parse_output);
	yy_delete_buffer(buffer);

	if(ret != 0){
		fprintf(stderr, "rcl error: %s", rcl_error_msg);
		memset(rcl_error_msg, 0, RCL_MSG_SIZE);
		return -1;
	}

	return 0;
}

rcl_type_t * rcl_null(void){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	return var;
}

rcl_type_t * rcl_number(double n){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_NUM;
	var->number = n;
	return var;
}

rcl_type_t * rcl_byte(uint8_t b){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_BYTE;
	var->byte = b;
	return var;
}

struct S_RCL_TYPE * rcl_boolean(bool b){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_BOOL;
	var->boolean = b;
	return var;
}

rcl_type_t * rcl_string(const char * s){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_STR;
	var->string = strdup(s);
	return var;
}

struct S_RCL_TYPE * rcl_error(const char * s){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_ERR;
	var->string = strdup(s);
	return var;
}

rcl_type_t * rcl_list(rcl_type_t * value){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_LIST;
	var->list = (rcl_list_t *)malloc(sizeof(rcl_list_t));
	if(var->list == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var->list, 0, sizeof(rcl_list_t));
	var->list->elements = (rcl_type_t **)malloc(sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	if(var->list->elements == NULL){
		dbg_printf( "malloc() failed" );
		return NULL;
	}

	memset(var->list->elements, 0, sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	var->list->len = 0;
	var->list->cap = RCL_LIST_INIT_SIZE;
	var->list->elements[var->list->len++] = value;

	return var;
}

void rcl_copy(rcl_type_t * dst, const rcl_type_t * src){
	uint32_t i;
	/* free unnecessary data */
	switch(dst->type){
	case T_STR :
	case T_ERR :
		free(dst->string);
		break;
	case T_LIST :
		for(i = 0; i < dst->list->len; i++){
			rcl_free(dst->list->elements[i]);
		}
		free(dst->list->elements);
		free(dst->list);
		break;
	}

	dst->type = src->type;

	switch(src->type){
	case T_NUM :
		dst->number = src->number;
		break;
	case T_BYTE :
		dst->byte = src->byte;
		break;
	case T_BOOL :
		dst->boolean = src->boolean;
		break;
	case T_STR :
	case T_ERR :
		dst->string = strdup(src->string);
		break;
	case T_LIST :
		for(i = 0; i < src->list->len; i++){
			if(i == 0){
				dst->list = (rcl_list_t *)malloc(sizeof(rcl_list_t));
				if(dst->list == NULL){
					dbg_printf( "malloc() failed" );
					return NULL;
				}

				memset(dst->list, 0, sizeof(rcl_list_t));
				dst->list->elements = (rcl_type_t **)malloc(sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
				if(dst->list->elements == NULL){
					dbg_printf( "malloc() failed" );
					return NULL;
				}

				rcl_type_t * foo = rcl_null();
				rcl_copy(foo, src->list->elements[i]);

				memset(dst->list->elements, 0, sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
				dst->list->len = 0;
				dst->list->cap = RCL_LIST_INIT_SIZE;
				dst->list->elements[dst->list->len++] = foo;
			} else {
				rcl_type_t * foo = rcl_null();
				rcl_copy(foo, src->list->elements[i]);
				rcl_list_add(dst, foo);
			}
		}
		break;
	}
}

void rcl_free(rcl_type_t * value){
	uint32_t i;
	if(value == NULL){
		return;
	}

	switch(value->type){
	case T_NULL :
	case T_NUM :
	case T_BYTE :
	case T_BOOL :
		free(value);
		break;
	case T_STR :
	case T_ERR :
		free(value->string);
		free(value);
		break;
	case T_LIST :
		for(i = 0; i < value->list->len; i++){
			rcl_free(value->list->elements[i]);
		}
		free(value->list->elements);
		free(value->list);
		free(value);
		break;
	}
}

rcl_type_t * rcl_list_add(rcl_type_t * list, rcl_type_t * value){
		if(list->list->len == list->list->cap){
			list->list->elements = realloc(list->list->elements, sizeof(rcl_type_t *) * list->list->cap * 2);

			if(list->list->elements == NULL){
				dbg_printf( "realloc() failed" );
				return NULL;
			}

			list->list->cap *= 2;
		}

		list->list->elements[list->list->len++] = value;

	return list;
}

rcl_type_t * rcl_list_get(rcl_type_t * list, uint32_t index){
	rcl_type_t * val = rcl_null();

	if(list->type != T_LIST){
		rcl_copy(val, rcl_error("not a list"));
	} else if(index >= list->list->len){
		rcl_copy(val, rcl_error("index out of bounds"));
	} else {
		rcl_copy(val, list->list->elements[index]);
	}

	return val;
}

char * rcl_op_to_string(int op){
	switch(op){
	case OP_GET :
		return strdup("get");
	case OP_SET :
		return strdup("set");
	case OP_RESET :
		return strdup("reset");
	case OP_START :
		return strdup("start");
	case OP_STOP:
		return strdup("stop");
	default:
		return strdup("(unknown operation)");
	}
}

void rcl_val_to_string(char * str, const rcl_type_t * value){
	uint32_t i;
	char tmp[1024];

	if(value == NULL){
		strcat(str, "(null pointer)");
		return;
	}

	switch(value->type){
	case T_NULL :
		strcat(str, "null");
		break;
	case T_NUM :
		sprintf(tmp, "%.2f", value->number);
		strcat(str, tmp);
		break;
	case T_BYTE :
		sprintf(tmp, "0x%X (0b" RCL_FMT_BYTE_TO_BINARY_FMT ")", value->byte, RCL_FMT_BYTE_TO_BINARY(value->byte));
		strcat(str, tmp);
		break;
	case T_BOOL :
		if(value->boolean){
			strcat(str, "true");
		} else {
			strcat(str, "false");
		}
		break;
	case T_STR :
		strcat(str, "\"");
		strcat(str, value->string);
		strcat(str, "\"");
		break;
	case T_ERR :
		strcat(str, "error: ");
		strcat(str, value->string);
		break;
	case T_LIST:
		strcat(str, "[ ");
		for(i = 0; i < value->list->len; i++){
			rcl_val_to_string(str, value->list->elements[i]);

			if(i < value->list->len - 1){
				strcat(str, ", ");
			}
		}
		strcat(str, " ]");
		break;
	default:
		strcat(str, "(invalid type)");
	}
}

void rcl_val_to_str_buf(str_buf_t * sb, const struct S_RCL_TYPE * value){
	uint32_t i;
	char tmp[1024];

	if(value == NULL){
		str_buf_add(sb, "(null pointer)");
		return;
	}

	switch(value->type){
	case T_NULL :
		str_buf_add(sb, "null");
		break;
	case T_NUM :
		sprintf(tmp, "%.4f", value->number);
		str_buf_add(sb, tmp);
		break;
	case T_BYTE :
		sprintf(tmp, "0x%X (0b" RCL_FMT_BYTE_TO_BINARY_FMT ")", value->byte, RCL_FMT_BYTE_TO_BINARY(value->byte));
		str_buf_add(sb, tmp);
		break;
	case T_BOOL :
		if(value->boolean){
			str_buf_add(sb, "true");
		} else {
			str_buf_add(sb, "false");
		}
		break;
	case T_STR :
		str_buf_add(sb, "\"");
		str_buf_add(sb, value->string);
		str_buf_add(sb, "\"");
		break;
	case T_ERR :
		str_buf_add(sb, "error: ");
		str_buf_add(sb, value->string);
		break;
	case T_LIST:
		str_buf_add(sb, "[ ");
		for(i = 0; i < value->list->len; i++){
			rcl_val_to_str_buf(sb, value->list->elements[i]);

			if(i < value->list->len - 1){
				str_buf_add(sb, ", ");
			}
		}
		str_buf_add(sb, " ]");
		break;
	default:
		str_buf_add(sb, "(invalid type)");
	}
}

void rcl_execute(rcl_type_t * parse_output, int op, const char * attr, rcl_type_t * param){
#ifdef _DEBUG
{
	str_buf_t * sb = str_buf_create();
	rcl_val_to_str_buf(sb, param);
	printf(" EXEC: %s %s %s\n", rcl_op_to_string(op), attr, sb->str);
	str_buf_destroy(sb);
}
#endif

	uint32_t i;
	char str[1024];

	/* loop the array the end is */
	for(i = 0; ; i++){
		const rcl_attr_desc_t * desc = &rcl_attr_fcns[i];	// to solve warning (Initialization discards qualifiers from pointer target type)

		/* end of array */
		if(desc->attr == NULL){
			sprintf(str, "not supported attribute '%s'", attr);
			rcl_copy(parse_output, rcl_error(str));
			break;	// not found, break from the for loop
		}

		if(strcmp(attr, desc->attr) == 0){
			switch(op){
			case OP_GET :
				if(desc->fcn_get){
					if(rcle_add_fcn(create_process(desc->fcn_get, rcl_null(), param, desc->data, desc->data_size)) < 0){
						rcl_copy(parse_output, rcl_error("unable to add function"));
					} else {
						rcl_copy(parse_output, rcl_null());
					}
				} else {
					sprintf(str, "not supported opearation '%s' on '%s'", rcl_op_to_string(op), attr);
					rcl_copy(parse_output, rcl_error(str));
				}
				break;
			case OP_SET :
				if(desc->fcn_set){
					if(rcle_add_fcn(create_process(desc->fcn_set, rcl_null(), param, desc->data, desc->data_size)) < 0){
						rcl_copy(parse_output, rcl_error("unable to add function"));
					} else {
						rcl_copy(parse_output, rcl_null());
					}
				} else {
					sprintf(str, "not supported opearation '%s' on '%s'", rcl_op_to_string(op), attr);
					rcl_copy(parse_output, rcl_error(str));
				}
				break;
			case OP_RESET :
				if(desc->fcn_reset){
					if(rcle_add_fcn(create_process(desc->fcn_reset, rcl_null(), param, desc->data, desc->data_size)) < 0){
						rcl_copy(parse_output, rcl_error("unable to add function"));
					} else {
						rcl_copy(parse_output, rcl_null());
					}
				} else {
					sprintf(str, "not supported opearation '%s' on '%s'", rcl_op_to_string(op), attr);
					rcl_copy(parse_output, rcl_error(str));
				}
				break;
			case OP_START :
				if(desc->fcn_start){
					if(rcle_add_fcn(create_process(desc->fcn_start, rcl_null(), param, desc->data, desc->data_size)) < 0){
						rcl_copy(parse_output, rcl_error("unable to add function"));
					} else {
						rcl_copy(parse_output, rcl_null());
					}
				} else {
					sprintf(str, "not supported opearation '%s' on '%s'", rcl_op_to_string(op), attr);
					rcl_copy(parse_output, rcl_error(str));
				}
				break;
			case OP_STOP:
				if(desc->fcn_stop){
					if(rcle_add_fcn(create_process(desc->fcn_stop, rcl_null(), param, desc->data, desc->data_size)) < 0){
						rcl_copy(parse_output, rcl_error("unable to add function"));
					} else {
						rcl_copy(parse_output, rcl_null());
					}
				} else {
					sprintf(str, "not supported opearation '%s' on '%s'", rcl_op_to_string(op), attr);
					rcl_copy(parse_output, rcl_error(str));
				}
				break;
			}

			break;	// found, break from the for loop
		}
	}

	rcl_free(param);
}
