#include "rcl.h"

int rcl_parse(struct S_RCL_TYPE * parse_output, const char *s){
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
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	return var;
}

rcl_type_t * rcl_string(const char * s){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_STR;
	var->sval = strdup(s);
	return var;
}

rcl_type_t * rcl_int(int i){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_INT;
	var->ival = i;
	return var;
}

rcl_type_t * rcl_double(double d){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_DOUBLE;
	var->dval = d;
	return var;
}

rcl_type_t * rcl_uint(uint32_t u){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_UINT;
	var->uval = u;
	return var;
}

struct S_RCL_TYPE * rcl_bool(bool b){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_BOOL;
	var->bval = b;
	return var;
}

rcl_type_t * rcl_list(rcl_type_t * value){
	rcl_type_t * var = (rcl_type_t *) malloc(sizeof(rcl_type_t));
	if(var == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var, 0, sizeof(rcl_type_t));
	var->type = T_LIST;
	var->list = (rcl_list_t *)malloc(sizeof(rcl_list_t));
	if(var->list == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var->list, 0, sizeof(rcl_list_t));
	var->list->elements = (rcl_type_t **)malloc(sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	if(var->list->elements == NULL){
		fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__, __func__);
		return NULL;
	}

	memset(var->list->elements, 0, sizeof(rcl_type_t *) * RCL_LIST_INIT_SIZE);
	var->list->len = 0;
	var->list->cap = RCL_LIST_INIT_SIZE;
	var->list->elements[var->list->len++] = value;

#ifdef _DEBUG
	char str[1024];
	memset(str, 0, 1024);
	rcl_val_to_string(str, value);
	printf("created [%d, %d] = %s\n", (var->list->len - 1), var->list->cap, str);
#endif

	return var;
}

void rcl_copy(rcl_type_t * dst, rcl_type_t * src){
	uint32_t i;
	/* free unnecessary data */
	switch(dst->type){
	case T_STR :
		free(dst->sval);
		break;
	case T_LIST:
		for(i = 0; i < dst->list->len; i++){
			rcl_free(dst->list->elements[i]);
		}
		free(dst->list->elements);
		free(dst->list);
		break;
	}

	dst->type = src->type;

	switch(src->type){
	case T_INT :
		dst->ival = src->ival;
		break;
	case T_DOUBLE :
		dst->dval = src->dval;
		break;
	case T_UINT :
		dst->uval = src->uval;
		break;
	case T_BOOL :
		dst->bval = src->bval;
		break;
	case T_STR :
		dst->sval = strdup(src->sval);
		break;
	case T_LIST:
		/* TODO */
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
	case T_INT :
	case T_DOUBLE :
	case T_UINT :
	case T_BOOL :
		free(value);
		break;
	case T_STR :
		free(value->sval);
		free(value);
		break;
	case T_LIST:
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
				fprintf(stderr, "%s:%d:%s() : realloc() failed.\n", __FILE__, __LINE__, __func__);
				return NULL;
			}

			list->list->cap *= 2;
		}

		list->list->elements[list->list->len++] = value;

#ifdef _DEBUG
		char str[1024];
		memset(str, 0, 1024);
		rcl_val_to_string(str, value);
		printf("added [%d, %d] = %s\n", (list->list->len - 1), list->list->cap, str);
#endif

	return list;
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
	}

	return strdup("(unknown operation)");
}

void rcl_val_to_string(char * str, rcl_type_t * value){
	uint32_t i;
	char tmp[50];

	if(value == NULL){
		strcat(str, "(null pointer)");
		return;
	}

	switch(value->type){
	case T_NULL :
		strcat(str, "null");
		break;
	case T_INT :
		sprintf(tmp, "%d", value->ival);
		strcat(str, tmp);
		break;
	case T_DOUBLE :
		sprintf(str, "%.2f", value->dval);
		strcat(str, tmp);
		break;
	case T_UINT :
		sprintf(str, "%u (0x%X)", value->uval, value->uval);
		strcat(str, tmp);
		break;
	case T_BOOL :
		if(value->bval){
			strcat(str, "true");
		} else {
			strcat(str, "false");
		}
		break;
	case T_STR :
		strcat(str, "\"");
		strcat(str, value->sval);
		strcat(str, "\"");
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

int rcl_execute(rcl_type_t * parse_output, int op, const char * attr, rcl_type_t * param){
#ifdef _DEBUG
	char str[1024];
	memset(str, 0, 1024);
	rcl_val_to_string(str, param);
	printf(" EXEC: %s %s %s\n", rcl_op_to_string(op), attr, str);
#endif

	uint32_t i, ret = -1;
	for(i = 0; ; i++){
		rcl_attr_desc_t * desc = &rcl_attr_fcns[i];

		/* end of array */
		if(desc->attr == NULL){
			printf("not supported attribute: '%s'\n", attr);
			break;
		}

		if(strcmp(attr, desc->attr) == 0){
			switch(op){
			case OP_GET :
				if(desc->fcn_get){
					desc->fcn_get(parse_output, param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_SET :
				if(desc->fcn_set){
					desc->fcn_set(parse_output, param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_RESET :
				if(desc->fcn_reset){
					desc->fcn_reset(parse_output, param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_START :
				if(desc->fcn_start){
					desc->fcn_start(parse_output, param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			case OP_STOP:
				if(desc->fcn_stop){
					desc->fcn_stop(parse_output, param);
				} else {
					printf("not supported opearation '%s' on '%s'\n", rcl_op_to_string(op), attr);
				}
				break;
			}
			break;
		}
	}

	rcl_free(param);
	return ret;
}
